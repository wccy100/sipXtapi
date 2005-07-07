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

#ifndef _OsSSLServerSocket_h_
#define _OsSSLServerSocket_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <os/OsConnectionSocket.h>
#include <os/OsServerSocket.h>
#include <os/OsSSLConnectionSocket.h>
#include <os/OsFS.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

//: Implements TCP server for accepting TCP connections
// This class provides the implementation of the UDP datagram-based 
// socket class which may be instantiated. 

class OsSSLServerSocket : public OsServerSocket
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

   OsSSLServerSocket(int connectionQueueSize, int serverPort=-1);

   //:Constructor to set up TCP socket server
   // Sets the socket connection queue and starts listening on the
   // port for requests to connect.
   // 
   //!param: connectionQueueSize - The maximum number of outstanding connection requests which are allowed before subsequent requests are turned away.
   //!param: serverPort - The port on which the server will listen to accept connection requests.  -1 (default) means let OS pick port.

   OsSSLServerSocket& operator=(const OsSSLServerSocket& rhs);
     //:Assignment operator

  virtual
   ~OsSSLServerSocket();
     //:Destructor


/* ============================ MANIPULATORS ============================== */

   static OsStatus setSSLPublicCertificateLocation(const UtlString &rPublicCertificatePath);
   static OsStatus setSSLPrivateKeyLocation(const UtlString &rPrivateKeyPath);
   static OsStatus setSSLPrivateKeyPassword(const UtlString &rPrivateKeyPassword);

   virtual OsConnectionSocket* accept();
   //:Blocking accept of next connection
   // Blocks and waits for the next TCP connection request.
   //!returns: Returns a socket connected to the client requesting the
   //!returns: connection.  If an error occurs returns NULL.


   void close();
   //: Close down the server

/* ============================ ACCESSORS ================================= */

   virtual int getLocalHostPort() const;
   //:Return the local port number
   // Returns the port to which this socket is bound on this host.

/* ============================ INQUIRY =================================== */
   virtual int getIpProtocol() const;
   //: Returns the protocol type of this socket

   virtual UtlBoolean isOk() const;
   //: Server socket is in ready to accept incoming conection requests.

   int isConnectionReady();
   //: Poll to see if connections are waiting
   //!returns: 1 if one or call to accept() will not block <br>
   //!returns: 0 if no connections are ready (i.e. accept() will block).

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
   static UtlString sPublicCertificateLocation; 
   static UtlString sPrivateKeyLocation; 
   static UtlString sPrivateKeyPassword;
   
   static UtlBoolean sbSSLInit;
   //: whether or not we have initialized the SSL layer

   SSL_CTX* mCTX;
   SSL_METHOD *mMeth;

   void initializeCTX();

   static int pemPasswdCallbackFunc(char *buf, int size, int rwflag, void *userdata);
    //:Callback used to set the private key password for decrypting the key
    //:buf is the buffer to fill with a password
    //:size is the maximum size of the buffer 
   OsSSLServerSocket(const OsSSLServerSocket& rOsSSLServerSocket);
     //:Disable copy constructor

   OsSSLServerSocket();
     //:Disable default constructor


};

/* ============================ INLINE METHODS ============================ */

#endif  // _OsSSLServerSocket_h_

