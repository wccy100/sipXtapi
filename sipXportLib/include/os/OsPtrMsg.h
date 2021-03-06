//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _OsPtrMsg_h_
#define _OsPtrMsg_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "os/OsDefs.h"
#include "utl/UtlContainable.h"
#include "os/OsMsg.h"


// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

//:Base class for message queue buffers

class OsPtrMsg : public OsMsg
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:
   static const UtlContainableType TYPE ;    /** < Class type used for runtime checking */

/* ============================ CREATORS ================================== */

   OsPtrMsg(const unsigned char msgType, 
            const unsigned char msgSubType, 
            void* pData, 
            void* pData2 = NULL);
     //:Constructor

   OsPtrMsg(const OsPtrMsg& rOsMsg);
     //:Copy constructor

   virtual OsMsg* createCopy(void) const;
     //:Create a copy of this msg object (which may be of a derived type)

/* ============================ MANIPULATORS ============================== */

   OsPtrMsg& operator=(const OsPtrMsg& rhs);
     //:Assignment operator


/* ============================ ACCESSORS ================================= */
   void* getPtr();
   void* getPtr2();
   

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
   void* mpData;
   void* mpData2;

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
   

};

/* ============================ INLINE METHODS ============================ */

#endif  // _OsPtrMsg_h_
