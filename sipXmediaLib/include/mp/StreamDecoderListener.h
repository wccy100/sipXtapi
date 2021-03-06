//  
// Copyright (C) 2006 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

#ifndef _StreamDecoderListener_h_
#define _StreamDecoderListener_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include "mp/StreamFormatDecoder.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

//:Class short description which may consist of multiple lines (note the ':')
// Class detailed description which may extend to multiple lines
class StreamDecoderListener
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{
   StreamDecoderListener();
     //:Default constructor

   virtual
   ~StreamDecoderListener();
     //:Destructor

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{
   virtual void decoderUpdate(StreamFormatDecoder* pDecoder, 
                              StreamDecoderEvent event) = 0 ;
     //: Informs the listener when the decoder has an event to publish.
     //! param pDecoder - Decoder publishing the state change
     //! param event - The new decoder event state

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

   StreamDecoderListener(const StreamDecoderListener& rStreamDecoderListener);
     //:Copy constructor

   StreamDecoderListener& operator=(const StreamDecoderListener& rhs);
     //:Assignment operator

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
};

/* ============================ INLINE METHODS ============================ */

#endif  // _StreamDecoderListener_h_
