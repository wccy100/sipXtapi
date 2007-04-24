//  
// Copyright (C) 2006-2007 SIPez LLC. 
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


// SYSTEM INCLUDES
#include <assert.h>

// APPLICATION INCLUDES
// #include "os/OsDefs.h"
// #include "mp/MpMisc.h"
#include "mp/MpBuf.h"
#include "mp/MprBridge.h"
#include "mp/MpMisc.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Constructor
MprBridge::MprBridge(const UtlString& rName,
                     int maxInOutputs,
                     int samplesPerFrame, 
                     int samplesPerSec)
:  MpAudioResource(rName, 
                   1, maxInOutputs, 
                   1, maxInOutputs, 
                   samplesPerFrame, 
                   samplesPerSec)
{
    handleEnable();
}

// Destructor
MprBridge::~MprBridge()
{
}

/* ============================ MANIPULATORS ============================== */

/* ============================ ACCESSORS ================================= */

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */

UtlBoolean MprBridge::doMix(MpAudioBufPtr inBufs[], int inBufsSize,
                            MpAudioBufPtr &out, int samplesPerFrame)
{
    int inputsConnected;  // Number of connected ports
    int inputsValid;      // Number of ports with available data
    int inputsActive;     // Number of ports with active speech
    int lastConnected;    // Port number of last connected port
    int lastValid;        // Port number of last port with available data
    int lastActive;       // Port number of last port with active speech

    // Count contributing inputs
    inputsConnected = 0;
    inputsValid = 0;
    inputsActive = 0;
    lastConnected = -1;
    lastValid = -1;
    lastActive = -1;
    for (int inIdx=0; inIdx < inBufsSize; inIdx++) {
       if (isPortActive(inIdx))
       {
          inputsConnected++;
          lastConnected = inIdx;
          if (inBufs[inIdx].isValid())
          {
             inputsValid++;
             lastValid = inIdx;
             if (inBufs[inIdx]->isActiveAudio())
             {
                inputsActive++;
                lastActive = inIdx;
             }
          }
       }
    }

    // If there is only one input we could skip all processing and copy it
    // to output. Special case for single input is needed because other case
    // make decision make its choice depending on voice activity, which lead
    // to unwanted silence insertion. Someday this function will get smarter...
    if (inputsValid == 1) {
       out = inBufs[lastValid];
    } else if (inputsActive == 1) {
       // If only one active input then just return it
       out = inBufs[lastActive];
    } else if (inputsActive > 1) {
        // Compute a logarithmic scale factor to renormalize (approximately)
        int scale = 0;
        while (inputsActive > 1) {
            scale++;
            inputsActive = inputsActive >> 1;
        }

        // Get new buffer for mixed output
        out = MpMisc.RawAudioPool->getBuffer();
        if (!out.isValid())
            return FALSE;
        out->setSamplesNumber(samplesPerFrame);

        // Fill output buffer with silence
        MpAudioSample* outstart = out->getSamples();
        memset((char *) outstart, 0, samplesPerFrame * sizeof(MpAudioSample));

        // Mix them all
        for (int inIdx=0; inIdx < inBufsSize; inIdx++) {
            if (isPortActive(inIdx)) {
                MpAudioSample* output = outstart;
                // Mix only non-silent audio
                if(inBufs[inIdx].isValid() && inBufs[inIdx]->isActiveAudio()) { 
                    MpAudioSample* input = inBufs[inIdx]->getSamples();
                    int n = min(inBufs[inIdx]->getSamplesNumber(), samplesPerFrame);
                    for (int i=0; i<n; i++)
                        *output++ += (*input++) >> scale;
                }
            }
        }
    } else {
       // Ipse: Disabled CN output. No input - no output.

       // Local output==comfort noise if all remote inputs are disabled or silent
//       out = MpMisc.comfortNoise;
    }
    return TRUE;
}

/* //////////////////////////// PRIVATE /////////////////////////////////// */

//Check whether this port is connected to both input and output
UtlBoolean MprBridge::isPortActive(int portIdx)
{
   return (isInputConnected(portIdx) && isOutputConnected(portIdx));
}

UtlBoolean MprBridge::doProcessFrame(MpBufPtr inBufs[],
                                     MpBufPtr outBufs[],
                                     int inBufsSize,
                                     int outBufsSize,
                                     UtlBoolean isEnabled,
                                     int samplesPerFrame,
                                     int samplesPerSecond)
{
   MpAudioBufPtr in;
   UtlBoolean ret = FALSE;
   MpAudioBufPtr* inAudioBufs;

   if (outBufsSize == 0)
      return FALSE;

   if (inBufsSize == 0)
      return FALSE;

   // We want correct in/out pairs
   if (inBufsSize != outBufsSize)
      return FALSE;

   inAudioBufs = new MpAudioBufPtr[inBufsSize];
   for (int i=0; i<inBufsSize; i++) {
       inAudioBufs[i].swap(inBufs[i]);
   }

   // If disabled, mix all remote inputs onto local speaker, and copy
   // our local microphone to all remote outputs.
   if (!isEnabled)
   {
       printf("Bridge disabled\n");
      // Move local mic data to all remote parties
      in.swap(inAudioBufs[0]);
      for (int outIdx=1; outIdx < outBufsSize; outIdx++) {
         if (isPortActive(outIdx)) {
            outBufs[outIdx] = in;
         }
      }

      // Copy mixed remote inputs to local speaker and exit
      MpAudioBufPtr out;
      ret = doMix(inAudioBufs, inBufsSize, out, samplesPerFrame);
      outBufs[0] = out;
   } 
   else
   {
      MpAudioBufPtr temp;

      // Enabled.  Mix together inputs onto outputs, with the requirement
      // that no output receive its own input.
      for (int outIdx=0; outIdx < outBufsSize; outIdx++) {
         // Skip unconnected outputs
         if (!isPortActive(outIdx))
            continue;

         // Exclude current input from mixing
         temp.swap(inAudioBufs[outIdx]);

         // Mix all inputs except outIdx together and put to the output
         MpAudioBufPtr out;
         ret = doMix(inAudioBufs, inBufsSize, out, samplesPerFrame);
         outBufs[outIdx] = out;

         // Return current input to input buffers vector
         temp.swap(inAudioBufs[outIdx]);
      }
   }

   // Cleanup temporary buffers
   delete[] inAudioBufs;

   return ret;
}

/* ============================ FUNCTIONS ================================= */

