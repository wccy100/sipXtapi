//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifdef HAVE_GIPS /* [ */

#ifndef _MpdGIPSiPCMA_h_
#define _MpdGIPSiPCMA_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "mp/MpDecoderBase.h"
#include "mp/GIPS/gips_typedefs.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS

/// Derived class for GIPS iPCMA decoder.
class MpdGIPSiPCMA: public MpDecoderBase
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// Constructor
   MpdGIPSiPCMA( int payloadType ///< (in) RTP payload type associated with
                                 ///<  this decoder
               );

     /// Destructor
   virtual
   ~MpdGIPSiPCMA(void);

     /// Initializes a codec data structure for use as a decoder
  virtual OsStatus initDecode(MpAudioConnection* pConnection);
     /**<
     *  @param pConnection - (in) Pointer to the MpAudioConnection container
     *  @returns <b>OS_SUCCESS</b> - Success
     *  @returns <b>OS_NO_MEMORY</b> - Memory allocation failure
     */

     /// Frees all memory allocated to the decoder by <i>initDecode</i>
   virtual OsStatus freeDecode(void);
     /**<
     *  @returns <b>OS_SUCCESS</b> - Success
     *  @returns <b>OS_DELETED</b> - Object has already been deleted
     */

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{
//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
   static const MpCodecInfo smCodecInfo;  ///< static information about the codec
   EG711A_inst* pDecoderState;
   JB_inst* mpJBState;
};

#endif  // _MpdGIPSiPCMA_h_
#endif /* HAVE_GIPS ] */
