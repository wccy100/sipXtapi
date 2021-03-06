//
// Copyright (C) 2007-2017 SIPez LLC  All rights reserved.
//
// Copyright (C) 2004-2008 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////
// Author: Dan Petrie (dpetrie AT SIPez DOT com)

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include <utl/UtlString.h>
#include <utl/UtlHashBagIterator.h>
#include <os/OsSysLog.h>
#include <os/OsTimer.h>
#include <os/OsDateTime.h>
#include <net/SipSubscriptionMgr.h>
#include <net/SipMessage.h>
#include <net/SipDialogMgr.h>
#include <net/SipDialog.h>
#include <net/NetMd5Codec.h>


// Private class to contain callback for eventTypeKey
class SubscriptionServerState : public UtlString
{
public:
    SubscriptionServerState();

    virtual ~SubscriptionServerState();

    // Parent UtlString contains the dialog handle
    UtlString mResourceId;
    UtlString mEventTypeKey;
    UtlString mAcceptHeaderValue;
    long mExpirationDate; // epoch time
    SipMessage* mpLastSubscribeRequest;
    OsTimer* mpExpirationTimer;

private:
    //! DISALLOWED accidental copying
    SubscriptionServerState(const SubscriptionServerState& rSubscriptionServerState);
    SubscriptionServerState& operator=(const SubscriptionServerState& rhs);
};

class SubscriptionServerStateIndex : public UtlString
{
public:
    SubscriptionServerStateIndex();

    virtual ~SubscriptionServerStateIndex();

    // Parent UtlString contains the dialog handle
    SubscriptionServerState* mpState;


private:
    //! DISALLOWED accidental copying
    SubscriptionServerStateIndex(const SubscriptionServerStateIndex& rSubscriptionServerStateIndex);
    SubscriptionServerStateIndex& operator=(const SubscriptionServerStateIndex& rhs);
};


// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

SubscriptionServerState::SubscriptionServerState()
{
    mExpirationDate = -1;
    mpLastSubscribeRequest = NULL;
    mpExpirationTimer = NULL;
}
SubscriptionServerState::~SubscriptionServerState()
{
    if(mpLastSubscribeRequest)
    {
        delete mpLastSubscribeRequest;
        mpLastSubscribeRequest = NULL;
    }

    if(mpExpirationTimer)
    {
        // Timer should have been stopped and the the task upon
        // which the fired timer queues its message need to have
        // synchronized to make sure it does not get touched after
        // it is deleted here.
        delete mpExpirationTimer;
        mpExpirationTimer = NULL;
    }
}

SubscriptionServerStateIndex::SubscriptionServerStateIndex()
{
    mpState = NULL;
}

SubscriptionServerStateIndex::~SubscriptionServerStateIndex()
{
    // Do not delete mpState, it is freed else where
}

// Constructor
SipSubscriptionMgr::SipSubscriptionMgr()
: mSubscriptionMgrMutex(OsMutex::Q_FIFO)
{
    mEstablishedDialogCount = 0;
    mMinExpiration = 32;
    mDefaultExpiration = 3600;
    mMaxExpiration = 86400;
}


// Copy constructor NOT IMPLEMENTED
SipSubscriptionMgr::SipSubscriptionMgr(const SipSubscriptionMgr& rSipSubscriptionMgr)
: mSubscriptionMgrMutex(OsMutex::Q_FIFO)
{
}


// Destructor
SipSubscriptionMgr::~SipSubscriptionMgr()
{
    // Iterate through and delete all the dialogs
    // TODO:
}

/* ============================ MANIPULATORS ============================== */

// Assignment operator
SipSubscriptionMgr& 
SipSubscriptionMgr::operator=(const SipSubscriptionMgr& rhs)
{
   if (this == &rhs)            // handle the assignment to self case
      return *this;

   return *this;
}

UtlBoolean SipSubscriptionMgr::updateDialogInfo(const SipMessage& subscribeRequest,
                                                const UtlString& resourceId,
                                                const UtlString& eventTypeKey,
                                                OsMsgQ* subscriptionTimeoutQueue,
                                                UtlString& subscribeDialogHandle,
                                                UtlBoolean& isNew,
                                                UtlBoolean& isSubscriptionExpired,
                                                SipMessage& subscribeResponse)
{
    isNew = FALSE;
    UtlBoolean subscriptionSucceeded = FALSE;
    UtlString dialogHandle;
    subscribeRequest.getDialogHandle(dialogHandle);
    SubscriptionServerState* state = NULL;
    int expiration = -1;
    isSubscriptionExpired = TRUE;

    // If this is an early dialog we need to make it an established dialog.
    if(SipDialog::isEarlyDialog(dialogHandle))
    {
        UtlString establishedDialogHandle;
        if(mDialogMgr.getEstablishedDialogHandleFor(dialogHandle, establishedDialogHandle))
        {
            OsSysLog::add(FAC_SIP, PRI_WARNING,
                "Incoming early SUBSCRIBE dialog: %s matches established dialog: %s",
                dialogHandle.data(), establishedDialogHandle.data());
        }

        // make up a To tag and set it
        UtlString toTagClearText;
        // Should probably add something like the local IP address and SIP port
        toTagClearText.append(dialogHandle);
        char numBuffer[20];
        lock();
        mEstablishedDialogCount++;
        sprintf(numBuffer, "%d", mEstablishedDialogCount);
        unlock();
        toTagClearText.append(numBuffer);
        UtlString toTag;
        NetMd5Codec::encode(toTagClearText, toTag);

        // Get and validate the expires period
        // This potentially should be delegated to the event handler specifics
        if(!subscribeRequest.getExpiresField(&expiration))
        {
            expiration = mDefaultExpiration;
        }

        else if(expiration > mMaxExpiration)
        {
            expiration = mMaxExpiration;
        }

        // Acceptable expiration, create a subscription and dialog
        if(expiration >= mMinExpiration ||
           expiration == 0 ||
           // :WORKAROUND:  Also allow expiration == 1, to support the
           // 1-second subscriptions the pick-up agent makes because
           // current Snom phones do not respond to 0-second subscriptions.
           // See XPB-399 and ENG-319.
           expiration == 1)
        {
            // Create a dialog and subscription state even if
            // the expiration is zero as we need the dialog info
            // to route the one time NOTIFY.  The immediately
            // expired dialog will be garbage collected.

            SipMessage* subscribeCopy = new SipMessage(subscribeRequest);
            subscribeCopy->setToFieldTag(toTag);

            // Re-get the dialog handle now that the To tag is set
            subscribeCopy->getDialogHandle(dialogHandle);

            // Create the dialog
            mDialogMgr.createDialog(*subscribeCopy, FALSE, dialogHandle);
            isNew = TRUE;

            // Create a subscription state
            state = new SubscriptionServerState();
            *((UtlString*)state) = dialogHandle;
            state->mEventTypeKey = eventTypeKey;
            state->mpLastSubscribeRequest = subscribeCopy;
            state->mResourceId = resourceId;
            subscribeCopy->getAcceptField(state->mAcceptHeaderValue);

            long now = OsDateTime::getSecsSinceEpoch();
            state->mExpirationDate = now + expiration;

            // TODO: currently the SipSubsribeServer does not handle timeout
            // events to send notifications that the subscription has ended.
            // So we do not set a timer at the end of the subscription
            state->mpExpirationTimer = NULL;

            // Create the index by resourceId and eventTypeKey key
            SubscriptionServerStateIndex* stateKey = new SubscriptionServerStateIndex;
            *((UtlString*)stateKey) = resourceId;
            stateKey->append(eventTypeKey);
            stateKey->mpState = state;

            // Set the contact to the same request URI that came in
            UtlString contact;
            subscribeRequest.getRequestUri(&contact);

            // Add the angle brackets for contact
            Url url(contact);
            url.includeAngleBrackets();
            contact = url.toString();

            subscribeResponse.setResponseData(subscribeCopy, 
                                            SIP_ACCEPTED_CODE,
                                            SIP_ACCEPTED_TEXT, 
                                            contact);
            subscribeResponse.setExpiresField(expiration);
            subscribeCopy->getDialogHandle(subscribeDialogHandle);

            lock();
            mSubscriptionStatesByDialogHandle.insert(state);
            mSubscriptionStateResourceIndex.insert(stateKey);
	    if (OsSysLog::willLog(FAC_SIP, PRI_DEBUG))
	    {
	       UtlString requestContact;
	       subscribeRequest.getContactField(0, requestContact);
	       OsSysLog::add(FAC_SIP, PRI_DEBUG,
			     "SipSubscriptionMgr::updateDialogInfo insert early-dialog subscription for key '%s', contact '%s', mExpirationDate %ld",
			     stateKey->data(), requestContact.data(), state->mExpirationDate);
            }

            // Not safe to touch these after we unlock
            stateKey = NULL;
            state = NULL;
            subscribeCopy = NULL;
            unlock();

            subscriptionSucceeded = TRUE;

            // One time subscribe?
            isSubscriptionExpired = expiration == 0;
        }
        // Expiration too small
        else
        {
            // Set expiration too small error
            subscribeResponse.setResponseData(&subscribeRequest, 
                                                SIP_TOO_BRIEF_CODE,
                                                SIP_SUB_TOO_BRIEF_TEXT);
            subscribeResponse.setMinExpiresField(mMinExpiration);
            isSubscriptionExpired = TRUE;
        }

    }

    // The dialog for this message should already exist
    else
    {
        // Get and validate the expires period
        // This potentially should be delegated to the event handler specifics
        if(!subscribeRequest.getExpiresField(&expiration))
        {
            expiration = mDefaultExpiration;
        }

        else if(expiration > mMaxExpiration)
        {
            expiration = mMaxExpiration;
        }

        // Acceptable expiration, create a subscription and dialog
        if(expiration > mMinExpiration ||
           expiration == 0)
        {
            // Update the dialog state
            mDialogMgr.updateDialog(subscribeRequest, dialogHandle);

            // Get the subscription state and update that
            // TODO:  This assumes that no one reuses the same dialog
            // to subscribe to more than one event type.  mSubscriptionStatesByDialogHandle
            // will need to be changed to a HashBag and we will need to
            // search through to find a matching event type
            lock();
            state = (SubscriptionServerState*)
                mSubscriptionStatesByDialogHandle.find(&dialogHandle);
            if(state)
            {
                long now = OsDateTime::getSecsSinceEpoch();
                state->mExpirationDate = now + expiration;
                if(state->mpLastSubscribeRequest)
                {
                    delete state->mpLastSubscribeRequest;
                }
                state->mpLastSubscribeRequest = new SipMessage(subscribeRequest);
                subscribeRequest.getAcceptField(state->mAcceptHeaderValue);

                // Set the contact to the same request URI that came in
                UtlString contact;
                subscribeRequest.getRequestUri(&contact);

                // Add the angle brackets for contact
                Url url(contact);
                url.includeAngleBrackets();
                contact = url.toString();

                subscribeResponse.setResponseData(&subscribeRequest, 
                                                SIP_ACCEPTED_CODE,
                                                SIP_ACCEPTED_TEXT, 
                                                contact);
                subscribeResponse.setExpiresField(expiration);
                subscriptionSucceeded = TRUE;
                isSubscriptionExpired = FALSE;
                subscribeDialogHandle = dialogHandle;
            }

            // No state, basically assume this is a new subscription
            else
            {
                SipMessage* subscribeCopy = new SipMessage(subscribeRequest);

                // Create the dialog
                mDialogMgr.createDialog(*subscribeCopy, FALSE, dialogHandle);
                isNew = TRUE;

                // Create a subscription state
                state = new SubscriptionServerState();
                *((UtlString*)state) = dialogHandle;
                state->mEventTypeKey = eventTypeKey;
                state->mpLastSubscribeRequest = subscribeCopy;
                state->mResourceId = resourceId;
                subscribeCopy->getAcceptField(state->mAcceptHeaderValue);

                long now = OsDateTime::getSecsSinceEpoch();
                state->mExpirationDate = now + expiration;
                // TODO: currently the SipSubsribeServer does not handle timeout
                // events to send notifications that the subscription has ended.
                // So we do not set a timer at the end of the subscription
                state->mpExpirationTimer = NULL;

                // Create the index by resourceId and eventTypeKey key
                SubscriptionServerStateIndex* stateKey = new SubscriptionServerStateIndex;
                *((UtlString*)stateKey) = resourceId;
                stateKey->append(eventTypeKey);
                stateKey->mpState = state;
                mSubscriptionStatesByDialogHandle.insert(state);
                mSubscriptionStateResourceIndex.insert(stateKey);
                if (OsSysLog::willLog(FAC_SIP, PRI_DEBUG))
	        {
		   UtlString requestContact;
		   subscribeRequest.getContactField(0, requestContact);
		   OsSysLog::add(FAC_SIP, PRI_DEBUG,
				 "SipSubscriptionMgr::updateDialogInfo insert subscription for key '%s', contact '%s', mExpirationDate %ld",
				 stateKey->data(), requestContact.data(), state->mExpirationDate);
                }
                   
                // Not safe to touch these after we unlock
                stateKey = NULL;
                state = NULL;
                subscribeCopy = NULL;

                // Set the contact to the same request URI that came in
                UtlString contact;
                subscribeRequest.getRequestUri(&contact);

                // Add the angle brackets for contact
                Url url(contact);
                url.includeAngleBrackets();
                contact = url.toString();

                subscribeResponse.setResponseData(&subscribeRequest, 
                                                SIP_ACCEPTED_CODE,
                                                SIP_ACCEPTED_TEXT, 
                                                contact);
                subscribeResponse.setExpiresField(expiration);
                subscriptionSucceeded = TRUE;
                // Unsubscribe
                if(expiration == 0)
                {
                    isSubscriptionExpired = TRUE;
                }
                else
                {
                    isSubscriptionExpired = FALSE;
                }
                subscribeDialogHandle = dialogHandle;
            }
            unlock();
        }

        // Expiration too small
        else
        {
            // Set expiration too small error
            subscribeResponse.setResponseData(&subscribeRequest, 
                                                SIP_TOO_BRIEF_CODE,
                                                SIP_SUB_TOO_BRIEF_TEXT);
            subscribeResponse.setMinExpiresField(mMinExpiration);
            isSubscriptionExpired = isExpired(dialogHandle);
        }
        
    }

    return(subscriptionSucceeded);
}

UtlBoolean SipSubscriptionMgr::getNotifyDialogInfo(const UtlString& subscribeDialogHandle,
                                                   SipMessage& notifyRequest)
{
    UtlBoolean notifyInfoSet = FALSE;
    lock();
    SubscriptionServerState* state = (SubscriptionServerState*)
        mSubscriptionStatesByDialogHandle.find(&subscribeDialogHandle);

    if(state)
    {
        notifyInfoSet = mDialogMgr.setNextLocalTransactionInfo(notifyRequest, 
                                                             SIP_NOTIFY_METHOD,
                                                             subscribeDialogHandle);

        // Set the event header, if we know what it is.
        if(state->mpLastSubscribeRequest)
        {
            UtlString eventHeader;
            state->mpLastSubscribeRequest->getEventField(eventHeader);
            notifyRequest.setEventField(eventHeader);
        }

        // Set the subscription-state header.
        long expires =
           state->mExpirationDate - OsDateTime::getSecsSinceEpoch();
        char buffer[30];
        sprintf(buffer,
                (expires > 0 ? "active;expires=%ld" : "terminated;reason=timeout"),
                expires);
        notifyRequest.setHeaderValue(SIP_SUBSCRIPTION_STATE_FIELD, buffer, 0);
    }
    unlock();

    return(notifyInfoSet);
}

UtlBoolean SipSubscriptionMgr::createNotifiesDialogInfo(const char* resourceId,
                                                        const char* eventTypeKey,
                                                        int& numNotifiesCreated,
                                                        UtlString**& acceptHeaderValuesArray,
                                                        SipMessage**& notifyArray)
{
    UtlString contentKey(resourceId);
    contentKey.append(eventTypeKey);

    OsSysLog::add(FAC_SIP, PRI_DEBUG,
                 "SipSubscriptionMgr::createNotifiesDialogInfo try to find contentKey '%s' in mSubscriptionStateResourceIndex (%"PRIuPTR" entries)",
                 contentKey.data(), mSubscriptionStateResourceIndex.entries());

    lock();
    UtlHashBagIterator iterator(mSubscriptionStateResourceIndex, &contentKey);
    int count = 0;
    int index = 0;
    acceptHeaderValuesArray = NULL;
    notifyArray = NULL;

    while(iterator())
    {
        count++;
    }

    if(count > 0)
    {
        SubscriptionServerStateIndex* contentTypeIndex = NULL;
        acceptHeaderValuesArray = new UtlString*[count];
        notifyArray = new SipMessage*[count];
        iterator.reset();
        long now = OsDateTime::getSecsSinceEpoch();

        while((contentTypeIndex = (SubscriptionServerStateIndex*)iterator()))
        {
            OsSysLog::add(FAC_SIP, PRI_DEBUG,
                          "SipSubscriptionMgr::createNotifiesDialogInfo now %ld, mExpirationDate %ld",
                          now, contentTypeIndex->mpState->mExpirationDate);

            // Should not happen, the container is supposed to be locked
            if(index >= count)
            {
                OsSysLog::add(FAC_SIP, PRI_ERR,
                    "SipSubscriptionMgr::createNotifiesDialogInfo iterator elements count changed from: %d to %d while locked",
                    count, index);
            }
            // Should not happen, the index should be created and
            // deleted with the state
            else if(contentTypeIndex->mpState == NULL)
            {
                OsSysLog::add(FAC_SIP, PRI_ERR,
                    "SipSubscriptionMgr::createNotifiesDialogInfo SubscriptionServerStateIndex with NULL mpState");
            }

            // If not expired yet
            else if(contentTypeIndex->mpState->mExpirationDate >= now)
            {
                // Get the accept value.
                acceptHeaderValuesArray[index] = 
                    new UtlString(contentTypeIndex->mpState->mAcceptHeaderValue);
                // Create the NOTIFY message.
                notifyArray[index] = new SipMessage;
                mDialogMgr.setNextLocalTransactionInfo(*(notifyArray[index]),
                                                         SIP_NOTIFY_METHOD, 
                                                         *(contentTypeIndex->mpState));

                // Set the event header, if we know what it is.
                UtlString eventHeader;
                if(contentTypeIndex->mpState->mpLastSubscribeRequest)
                {
                    contentTypeIndex->mpState->mpLastSubscribeRequest->getEventField(eventHeader);
                }
                notifyArray[index]->setEventField(eventHeader);

                // Set the subscription-state header.
                char buffer[30];
                sprintf(buffer, "active;expires=%ld",
                        contentTypeIndex->mpState->mExpirationDate - now);
                notifyArray[index]->setHeaderValue(SIP_SUBSCRIPTION_STATE_FIELD,
                                                   buffer, 0);
                OsSysLog::add(FAC_SIP, PRI_DEBUG,
                              "SipSubscriptionMgr::createNotifiesDialogInfo index %d, mAcceptHeaderValue '%s', getEventField '%s'",
                              index, acceptHeaderValuesArray[index]->data(),
                              eventHeader.data());

                 index++;
            }
        }
    }
    unlock();

    numNotifiesCreated = index;

    return(index > 0);
}

void SipSubscriptionMgr::freeNotifies(int numNotifies,
                                      UtlString** acceptHeaderValues,
                                      SipMessage** notifiesArray)
{
   if (notifiesArray && acceptHeaderValues && numNotifies > 0)
        {
            for(int index = 0; index < numNotifies; index++)
            {
                if(acceptHeaderValues[index])
                {
                    delete acceptHeaderValues[index];
                }
                if(notifiesArray[index])
                {
                    delete notifiesArray[index];
                }
            }
            delete[] acceptHeaderValues;
            delete[] notifiesArray;
        }
}

UtlBoolean SipSubscriptionMgr::endSubscription(const UtlString& dialogHandle)
{
    UtlBoolean subscriptionFound = FALSE;

    lock();
    SubscriptionServerState* state = (SubscriptionServerState*)
        mSubscriptionStatesByDialogHandle.find(&dialogHandle);
    if(state)
    {
        SubscriptionServerStateIndex* stateIndex = NULL;
        UtlString contentKey(state->mResourceId);
        contentKey.append(state->mEventTypeKey);
        UtlHashBagIterator iterator(mSubscriptionStateResourceIndex, &contentKey);
        while((stateIndex = (SubscriptionServerStateIndex*) iterator()))
        {
            if(stateIndex->mpState == state)
            {
                mSubscriptionStatesByDialogHandle.removeReference(state);
                mSubscriptionStateResourceIndex.removeReference(stateIndex);
                if (OsSysLog::willLog(FAC_SIP, PRI_DEBUG))
	        {
		   UtlString requestContact;
		   state->mpLastSubscribeRequest->getContactField(0, requestContact);
		   OsSysLog::add(FAC_SIP, PRI_DEBUG,
				 "SipSubscriptionMgr::endSubscription delete subscription for key '%s', contact '%s', mExpirationDate %ld",
				 stateIndex->data(), requestContact.data(),
				 state->mExpirationDate);
                }

                delete state;
                delete stateIndex;
                subscriptionFound = TRUE;

                break;
            }
        }

        // Could not find the state index that cooresponded to the state
        // SHould not happen, there should always be one of each
        if(!subscriptionFound)
        {
            OsSysLog::add(FAC_SIP, PRI_ERR,
                "SipSubscriptionMgr::endSubscription could not find SubscriptionServerStateIndex for state with dialog: %s",
                dialogHandle.data());
        }
    }

    unlock();

    // Remove the dialog
    mDialogMgr.deleteDialog(dialogHandle);

    return(subscriptionFound);
}

int SipSubscriptionMgr::dumpOldSubscriptions(long oldEpochTimeSeconds)
{
    int totalStates = 0;
    int oldStates = 0;
    int stateIndicesWithNoState = 0;
    lock();
    UtlHashBagIterator iterator(mSubscriptionStateResourceIndex);
    SubscriptionServerStateIndex* stateIndex = NULL;
    while((stateIndex = (SubscriptionServerStateIndex*) iterator()))
    {
        totalStates++;
        if(stateIndex->mpState)
        {
            OsSysLog::add(FAC_SIP, PRI_DEBUG,
                    "substate: %s expires: %ld old date: %ld",
                    stateIndex->mpState->data(), 
                    stateIndex->mpState->mExpirationDate,
                    oldEpochTimeSeconds);
            if(stateIndex->mpState->mExpirationDate < oldEpochTimeSeconds)
            {
                if (OsSysLog::willLog(FAC_SIP, PRI_DEBUG))
	            {
		            UtlString requestContact;
		            stateIndex->mpState->mpLastSubscribeRequest->
		            getContactField(0, requestContact);
		            OsSysLog::add(FAC_SIP, PRI_DEBUG,
				        "SipSubscriptionMgr::removeOldSubscriptions old subscription for key '%s', contact '%s', mExpirationDate %ld",
				    stateIndex->data(), requestContact.data(),
				    stateIndex->mpState->mExpirationDate);
                }
                oldStates++;
            }
        }
        else
        {
            OsSysLog::add(FAC_SIP, PRI_ERR,
                "SipSubscriptionMgr::removeOldSubscriptions SubscriptionServerStateIndex with NULL mpState, should be removed");
            OsSysLog::add(FAC_SIP, PRI_DEBUG,
                          "SipSubscriptionMgr::removeOldSubscriptions should remove subscription for key '%s'",
                          stateIndex->data());
            stateIndicesWithNoState++;
        }
    }

    unlock();
    OsSysLog::add(FAC_SIP, PRI_DEBUG,
            "SipSubscriptionMgr::removeOldSubscriptions states removed: %d indices w/o state: %d total states: %d",
            oldStates, stateIndicesWithNoState, totalStates);
    return(oldStates);
}
    
int SipSubscriptionMgr::removeOldSubscriptions(long oldEpochTimeSeconds)
{
    int totalStates = 0;
    int removedStates = 0;
    int stateIndicesWithNoState = 0;
    lock();
    UtlHashBagIterator iterator(mSubscriptionStateResourceIndex);
    SubscriptionServerStateIndex* stateIndex = NULL;
    while((stateIndex = (SubscriptionServerStateIndex*) iterator()))
    {
        totalStates++;
        if(stateIndex->mpState)
        {
            if(stateIndex->mpState->mExpirationDate < oldEpochTimeSeconds)
            {
                if (OsSysLog::willLog(FAC_SIP, PRI_DEBUG))
                {
                    UtlString requestContact;
                    stateIndex->mpState->mpLastSubscribeRequest->
                    getContactField(0, requestContact);
                    OsSysLog::add(FAC_SIP, PRI_DEBUG,
                        "SipSubscriptionMgr::removeOldSubscriptions delete subscription for key '%s', contact '%s', mExpirationDate %ld",
                        stateIndex->data(), requestContact.data(),
                        stateIndex->mpState->mExpirationDate);
                }
                mDialogMgr.deleteDialog(*(stateIndex->mpState));
                mSubscriptionStatesByDialogHandle.removeReference(stateIndex->mpState);
                removedStates++;
                delete stateIndex->mpState;
                stateIndex->mpState = NULL;
                mSubscriptionStateResourceIndex.removeReference(stateIndex);
                delete stateIndex;
            }
        }
        else
        {
            OsSysLog::add(FAC_SIP, PRI_ERR,
                "SipSubscriptionMgr::removeOldSubscriptions SubscriptionServerStateIndex with NULL mpState, deleting");
            mSubscriptionStateResourceIndex.removeReference(stateIndex);
            OsSysLog::add(FAC_SIP, PRI_DEBUG,
                          "SipSubscriptionMgr::removeOldSubscriptions delete subscription for key '%s'",
                          stateIndex->data());
            stateIndicesWithNoState++;
            delete stateIndex;
        }
    }

    unlock();
    OsSysLog::add(FAC_SIP, PRI_DEBUG,
            "SipSubscriptionMgr::removeOldSubscriptions states removed: %d indices w/o state: %d total states: %d",
            removedStates, stateIndicesWithNoState, totalStates);
    return(removedStates);
}

void SipSubscriptionMgr::setMaxExpiration(int maxExpiration)
{
    if(maxExpiration > 0)
    {
        mMaxExpiration = maxExpiration;
    }
}

/* ============================ ACCESSORS ================================= */

SipDialogMgr* SipSubscriptionMgr::getDialogMgr()
{
    return &mDialogMgr;
}

int SipSubscriptionMgr::getStateCount()
{
    int count = 0;
    lock();
    count = mSubscriptionStatesByDialogHandle.entries();
    unlock();
    return(count);
}

/* ============================ INQUIRY =================================== */

UtlBoolean SipSubscriptionMgr::dialogExists(UtlString& dialogHandle)
{
    UtlBoolean subscriptionFound = FALSE;

    lock();
    SubscriptionServerState* state = (SubscriptionServerState*)
        mSubscriptionStatesByDialogHandle.find(&dialogHandle);
    if(state)
    {
        subscriptionFound = TRUE;
    }
    unlock();

    return(subscriptionFound);
}

UtlBoolean SipSubscriptionMgr::isExpired(UtlString& dialogHandle)
{
    UtlBoolean subscriptionExpired = TRUE;

    lock();
    SubscriptionServerState* state = (SubscriptionServerState*)
        mSubscriptionStatesByDialogHandle.find(&dialogHandle);
    if(state)
    {
        long now = OsDateTime::getSecsSinceEpoch();

        if(now <= state->mExpirationDate)
        {
            subscriptionExpired = FALSE;
        }
    }
    unlock();

    return(subscriptionExpired);
}

/* //////////////////////////// PROTECTED ///////////////////////////////// */

/* //////////////////////////// PRIVATE /////////////////////////////////// */


void SipSubscriptionMgr::lock()
{
    mSubscriptionMgrMutex.acquire();
}

void SipSubscriptionMgr::unlock()
{
    mSubscriptionMgrMutex.release();
}

/* ============================ FUNCTIONS ================================= */
