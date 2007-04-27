//  
// Copyright (C) 2006-2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2006-2007 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Dan Petrie <dpetrie AT SIPez DOT com>

#ifndef _MprRtpOutputAudioConnectionConstructor_h_
#define _MprRtpOutputAudioConnectionConstructor_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include <mp/MpAudioResourceConstructor.h>
#include <mp/MpRtpOutputAudioConnection.h>
#include <mp/MprToSpkr.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
*  @brief MprRtpOutputAudioConnectionConstructor is used to contruct a ToOutputDevice resource
*
*/
class MprRtpOutputAudioConnectionConstructor : public MpAudioResourceConstructor
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

    /** Constructor
     */
    MprRtpOutputAudioConnectionConstructor(int samplesPerFrame = 80, 
                           int samplesPerSecond = 8000) :
      MpAudioResourceConstructor(DEFAULT_RTP_OUTPUT_RESOURCE_TYPE,
                                 0, //minInputs,
                                 1, //maxInputs,
                                 0, //minOutputs,
                                 0, //maxOutputs,
                                 samplesPerFrame,
                                 samplesPerSecond)
    {
    };

    /** Destructor
     */
    virtual ~MprRtpOutputAudioConnectionConstructor(){};

/* ============================ MANIPULATORS ============================== */

    /// Create a new resource
    virtual MpResource* newResource(const UtlString& resourceName)
    {
        assert(mSamplesPerFrame > 0);
        assert(mSamplesPerSecond > 0);

        // TODO: use MprToOutputDevice instead
        return(new MpRtpOutputAudioConnection(resourceName,
                                              999, 
                                              mSamplesPerFrame, 
                                              mSamplesPerSecond));
    }

/* ============================ ACCESSORS ================================= */

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

    /** Disabled copy constructor
     */
    MprRtpOutputAudioConnectionConstructor(const MprRtpOutputAudioConnectionConstructor& rMprRtpOutputAudioConnectionConstructor);


    /** Disable assignment operator
     */
    MprRtpOutputAudioConnectionConstructor& operator=(const MprRtpOutputAudioConnectionConstructor& rhs);

};

/* ============================ INLINE METHODS ============================ */

#endif  // _MprRtpOutputAudioConnectionConstructor_h_