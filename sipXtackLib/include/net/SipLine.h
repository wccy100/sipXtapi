// 
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SIPLINE_H__6AD8439B_AA8A_4F09_B5C4_44A3BA9C7AC6__INCLUDED_)
#define AFX_SIPLINE_H__6AD8439B_AA8A_4F09_B5C4_44A3BA9C7AC6__INCLUDED_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <utl/UtlHashBag.h>
#include "HttpMessage.h"
#include "net/Url.h"


// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS

//STATE TRANSITION//
/*
							|----1/2refresh time--|
							|--------------refresh time-------------|
 ---------             ------------           --------          ---------
 | TRYING| --------->  |REGISTERED| --------->|FAILED| -------->|EXPIRED|
 ---------             ------------           --------          ---------
	|							|	^								^
	|							|___|								|
	|																|
    |______________________failed the first time____________________|

*/

class SipLine
{
public:

   //utility functions
	enum LineStates
	{
		LINE_STATE_UNKNOWN  = 0,
		LINE_STATE_REGISTERED,	//sucessfull registeration , registration no expried on server
		LINE_STATE_DISABLED,	//not registering
		LINE_STATE_FAILED,		//failed registration
		LINE_STATE_PROVISIONED, //dont send registration , but enabled because server provisioned it.
		LINE_STATE_TRYING,		//registration message sent , awaiting response
		LINE_STATE_EXPIRED		//registration expried on server
	};

	SipLine(Url userEnteredUrl = "",
		Url identityUri = "",
     	UtlString user ="",
		UtlBoolean visible = TRUE,
		int state = LINE_STATE_REGISTERED,
		UtlBoolean isAutoEnabled = TRUE,
		UtlBoolean useCallHandling = FALSE);

	virtual ~SipLine();
	SipLine(const SipLine& rSipLine);
	SipLine& operator=(const SipLine& rSipLine);


    UtlBoolean isDeviceLine() ;
    //: Determine if this line is a device line.  Presently, a line is
    //  considered a device line if it's user is "Device"

  	UtlBoolean IsDuplicateRealm(const UtlString realm , const UtlString scheme = HTTP_DIGEST_AUTHENTICATION);

	UtlString& getLineId();
   

	int getState() ;
	void setState(int state) ;

	//who does the line belong to
	UtlString& getUser();
	void setUser(const UtlString user);

	//line identily or name - just the Uri
   void getIdentityAndUrl(Url &identity, Url &userEnteredUrl) ;
  	void setIdentityAndUrl(Url identity, Url userEnteredUrl) ;
   Url& getUserEnteredUrl() ;
   Url& getIdentity() ;
   Url& getCanonicalUrl() ;

	//if line will get enabled after reboot - ( ie it is persitant)
	void setAutoEnableStatus( UtlBoolean isAutoEnabled);
	UtlBoolean getAutoEnableStatus();

	//to use the call handling of the owner or not
	UtlBoolean getCallHandling();
	void setCallHandling( UtlBoolean useCallHandling = TRUE);

	//if line is visible in the line list to the user
	UtlBoolean getVisibility();
	void setVisibility( UtlBoolean isEnable = TRUE);

	//for use from GUI
  	int GetNumOfCredentials();

	UtlBoolean addCredentials(const UtlString& strRealm,
		const UtlString& strUserID,
		const UtlString& strPassword ,
		const UtlString& Type);

	UtlBoolean getCredentials(const UtlString& type /*[in]*/,
		const UtlString& realm /*[in]*/,
		UtlString* userID /*[out]*/,
		UtlString* MD5_token /*[out]*/);

   UtlBoolean getAllCredentials( int MaxEnteries/*[in]*/ ,
      int& actualEnteries /*[out/int]*/,
      UtlString realms[]/*[out/int]*/,
      UtlString UserId[]/*[out/int]*/,
	  UtlString type[]/*[out/int]*/,
      UtlString passtoken[]/*[out/int]*/);

	//removes credetials for a particular realm
	void removeCredential(const UtlString* realm);
	//removes all credentials for this line
	void removeAllCredentials();

    void setPreferredContactUri(const Url& preferredContactUri) ; 
      //: Set the preferred host/ip for the contact in subsequent registers
    UtlBoolean getPreferredContactUri(Url& preferredContactUri) const ; 
      //: Get Preferred host/ip for the contact in subsequent registers

protected:


	UtlBoolean mIsVisible;
	UtlBoolean mIsAutoEnabled;
	UtlBoolean mIsUsingCallHandling;
	Url mIdentity; //line key which is the canonical URL stripped of display name, angle brackets and field parameters (basically the URI).
    Url mUserEnteredUrl; //user entered URL string (could be incomplete "sip:444@")
    Url mCanonicalUrl; //The canonical URL which is the URL string with the host and port filled in if they were missing.

	UtlString mUser;
    UtlString mLineId;
	int mCurrentState;
    Url mPreferredContactUri ;

	UtlHashBag mCredentials;

    void copyCredentials(const SipLine& rSipLine);
    void generateLineID(UtlString& lineId);
};

#endif // !defined(AFX_SIPLINE_H__6AD8439B_AA8A_4F09_B5C4_44A3BA9C7AC6__INCLUDED_)
