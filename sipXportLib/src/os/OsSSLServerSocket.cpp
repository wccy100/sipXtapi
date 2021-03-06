//
// Copyright (C) 2006-2017 SIPez LLC.  All rights reserved.
//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

#if defined(HAVE_SSL)

// SYSTEM INCLUDES
#include "os/OsIntTypes.h"
#include <stdio.h>

#define OsSS_CONST
#if defined(_WIN32)
#   include <winsock2.h>
#undef OsSS_CONST
#define OsSS_CONST const
#elif defined(_VXWORKS)
#   include <inetLib.h>
#   include <sockLib.h>
#   include <unistd.h>
#elif defined(__pingtel_on_posix__)
#   include <netinet/in.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <unistd.h>
#   include <resolv.h>
#else
#error Unsupported target platform.
#endif

// APPLICATION INCLUDES
#include "os/OsSSL.h"
#include "os/OsSSLServerSocket.h"
#include "os/OsDefs.h"
#include "os/OsSysLog.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES

// CONSTANTS

#define SOCKET_LEN_TYPE
#ifdef __pingtel_on_posix__
#undef SOCKET_LEN_TYPE
#define SOCKET_LEN_TYPE (socklen_t *)
#endif

// STATIC VARIABLE INITIALIZATIONS

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Constructor
OsSSLServerSocket::OsSSLServerSocket(int connectionQueueSize, int serverPort)
   : OsServerSocket(connectionQueueSize,serverPort)
{
   OsSysLog::add(FAC_KERNEL, PRI_DEBUG, "OsSSLServerSocket::_ %p", this );
}

// Destructor
OsSSLServerSocket::~OsSSLServerSocket()
{
  close();
}

/* ============================ MANIPULATORS ============================== */

// Assignment operator
OsSSLServerSocket&
OsSSLServerSocket::operator=(const OsSSLServerSocket& rhs)
{
   if (this == &rhs)            // handle the assignment to self case
      return *this;

   return *this;
}

OsConnectionSocket* OsSSLServerSocket::accept()
{
   OsSSLConnectionSocket* newSocket = NULL;
   
   if (socketDescriptor == OS_INVALID_SOCKET_DESCRIPTOR)
   {
      OsSysLog::add(FAC_KERNEL, PRI_ERR
                    , "OsSSLServerSocket: accept exiting because socketDescriptor is %d"
                    ,(int)socketDescriptor);
   }
   else
   {
      /* Block while waiting for a client to connect. */
      struct sockaddr_in clientSocketAddr;
      int clientAddrLength = sizeof clientSocketAddr;
      int clientSocket = ::accept(socketDescriptor,
                                  (struct sockaddr*) &clientSocketAddr,
                                  SOCKET_LEN_TYPE &clientAddrLength);

      if (clientSocket < 0)
      {
         int error = OsSocketGetERRNO();
         if (0 != error)
         {
             OsSysLog::add(FAC_KERNEL, PRI_ERR, 
                           "OsSSLServerSocket: accept call failed with error: %d=%x",
                           error, error);
             socketDescriptor = OS_INVALID_SOCKET_DESCRIPTOR;
          }
      }
      else
      {
         OsSysLog::add(FAC_KERNEL, PRI_DEBUG, 
                       "OsSSLServerSocket::accept socket accepted: %d",
                       clientSocket);

         // TODO: allow this to be a non-shared context...
         SSL* pSSL = OsSharedSSL::get()->getServerConnection();
         if (pSSL)
         {
            SSL_set_fd (pSSL, clientSocket);

            newSocket = new OsSSLConnectionSocket(pSSL,clientSocket);
            if (newSocket)
            {
               int result = SSL_accept(pSSL);
               if (1 == result)
               {
                  OsSSL::logConnectParams(FAC_KERNEL, PRI_DEBUG
                                          ,"OsSSLServerSocket::accept"
                                          ,pSSL);

                  OsSysLog::add(FAC_KERNEL, PRI_DEBUG,
                                "OsSSLServerSocket::accept connection %p",
                                this
                                );
                  // test and cache the peer identity
                  newSocket->peerIdentity(NULL, NULL);
               }
               else
               {
                  OsSSL::logError(FAC_KERNEL, PRI_ERR,
                                  (  result == 0
                                   ? "OsSSLServerSocket SSL_accept - incompatible client?"
                                   : "OsSSLServerSocket SSL_accept SSL handshake error"
                                   ),
                                  SSL_get_error(pSSL, result));

                  // SSL failed, so clear this out.
                  delete newSocket;
                  newSocket = NULL;
               }
            }
            else
            {
               OsSysLog::add(FAC_KERNEL, PRI_ERR,
                             "OsSSLServerSocket::accept - new OsSSLConnectionSocket failed"
                             );
            }            
         }
         else
         {
            OsSysLog::add(FAC_KERNEL, PRI_ERR
                          , "OsSSLConnectionSocket::accept - Error creating new SSL connection.");
         }
      }
   }
   
   return(newSocket);
}

// HZM: This override is unnecessary, the parent method already does this check.
void OsSSLServerSocket::close()
{

   if(socketDescriptor > OS_INVALID_SOCKET_DESCRIPTOR)
   {
      OsServerSocket::close();
   }
}

/* ============================ ACCESSORS ================================= */

int OsSSLServerSocket::getLocalHostPort() const
{
   return(localHostPort);
}

/* ============================ INQUIRY =================================== */

OsSocket::IpProtocolSocketType OsSSLServerSocket::getIpProtocol() const
{
    return(OsSocket::SSL_SOCKET);
}

UtlBoolean OsSSLServerSocket::isOk() const
{
    return(socketDescriptor != OS_INVALID_SOCKET_DESCRIPTOR);
}

/* //////////////////////////// PROTECTED ///////////////////////////////// */

/* //////////////////////////// PRIVATE /////////////////////////////////// */

/* ============================ FUNCTIONS ================================= */
#endif // HAVE_SSL
