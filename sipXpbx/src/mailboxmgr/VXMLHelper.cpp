// 
// 
// Copyright (C) 2007 VoiceWorks Sp. z o.o.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include "mailboxmgr/VXMLDefs.h"
#include "mailboxmgr/VXMLHelper.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/* ============================ CREATORS ================================== */

VXMLHelper::VXMLHelper() 
{
  m_lang = NULL;
}

UtlString VXMLHelper::getLangAttr() 
{
  if (m_lang && m_lang != "") 
    return "xml:lang=\"" + m_lang + "\" ";
  return ""; 
}

VXMLHelper::~VXMLHelper()
{}

void VXMLHelper::setLang(UtlString lang) 
{ 
  m_lang = lang; 
}

UtlString VXMLHelper::getVXMLHeader(UtlString application)
{
  UtlString vxmlhdr;
  if (application != NULL) {
    vxmlhdr = VXML_BEGIN_WITH_ROOT + application + "\"" + getLangAttr() + "> \n";
  } else {
    vxmlhdr = VXML_BEGIN_WITH_LANG + getLangAttr() + "> \n";
  }
  return vxmlhdr;
}
