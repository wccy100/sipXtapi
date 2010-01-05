#
# Copyright (C) 2009 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
#
# Copyright (C) 2009 SIPez LLC.
# Licensed to SIPfoundry under a Contributor Agreement.
#
#
#//////////////////////////////////////////////////////////////////////////
#
# Author: Dan Petrie (dpetrie AT SIPez DOT com)
#
#
# This Makefile is for building sipXmediaLib as a part of Android NDK.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Set up the target identity.
# LOCAL_MODULE/_CLASS are required for local-intermediates-dir to work.
LOCAL_MODULE := libsipXmedia
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
#intermediates := $(call local-intermediates-dir)

fail_to_compile := \
    src/mp/mpau.cpp \
    src/mp/MpAudioAbstract.cpp \
    src/mp/MpAudioFileOpen.cpp \
    src/mp/MpAudioFileUtils.cpp \
    src/mp/MpAudioUtils.cpp \
    src/mp/MpAudioWaveFileRead.cpp \
    src/mp/MprFromFile.cpp \
    src/mp/MprFromStream.cpp \
    src/mp/MprToSpkr.cpp \


LOCAL_SRC_FILES := \
    src/mp/dft.cpp \
    src/mp/dmaTaskPosix.cpp \
    src/mp/DSPlib.cpp \
    src/mp/dtmflib.cpp \
    src/mp/FilterBank.cpp \
    src/mp/HandsetFilterBank.cpp \
    src/mp/MpAgcBase.cpp \
    src/mp/MpAgcSimple.cpp \
    src/mp/MpArrayBuf.cpp \
    src/mp/MpAudioBuf.cpp \
    src/mp/MpAudioFileDecompress.cpp \
    src/mp/MpAudioOutputConnection.cpp \
    src/mp/MpAudioResource.cpp \
    src/mp/MpBridgeAlgLinear.cpp \
    src/mp/MpBridgeAlgSimple.cpp \
    src/mp/MpBuf.cpp \
    src/mp/MpBufPool.cpp \
    src/mp/MpBufferMsg.cpp \
    src/mp/MpCallFlowGraph.cpp \
    src/mp/MpCodec.cpp \
    src/mp/MpCodecFactory.cpp \
    src/mp/MpDataBuf.cpp \
    src/mp/MpDecoderBase.cpp \
    src/mp/MpDecoderPayloadMap.cpp \
    src/mp/MpDspUtils.cpp \
    src/mp/MpDTMFDetector.cpp \
    src/mp/MpEncoderBase.cpp \
    src/mp/MpFlowGraphBase.cpp \
    src/mp/MpFlowGraphMsg.cpp \
    src/mp/mpG711.cpp \
    src/mp/MpInputDeviceDriver.cpp \
    src/mp/MpInputDeviceManager.cpp \
    src/mp/MpJbeFixed.cpp \
    src/mp/MpJitterBuffer.cpp \
    src/mp/MpJitterBufferEstimation.cpp \
    src/mp/MprHook.cpp \
    src/mp/MpMediaTask.cpp \
    src/mp/MpMediaTaskMsg.cpp \
    src/mp/MpMisc.cpp \
    src/mp/MpMMTimer.cpp \
    src/mp/MpMMTimerPosix.cpp \
    src/mp/MpodBufferRecorder.cpp \
    src/mp/MpOutputDeviceDriver.cpp \
    src/mp/MpOutputDeviceManager.cpp \
    src/mp/MpPlayer.cpp \
    src/mp/MpPlayerEvent.cpp \
    src/mp/MprBridge.cpp \
    src/mp/MprDecode.cpp \
    src/mp/MprDejitter.cpp \
    src/mp/MprDelay.cpp \
    src/mp/MprEchoSuppress.cpp \
    src/mp/MprEncode.cpp \
    src/mp/MpResampler.cpp \
    src/mp/MpResamplerSpeex.cpp \
    src/mp/MpResource.cpp \
    src/mp/MpResourceFactory.cpp \
    src/mp/MpResourceMsg.cpp \
    src/mp/MpResourceSortAlg.cpp \
    src/mp/MpResourceTopology.cpp \
    src/mp/MpResNotificationMsg.cpp \
    src/mp/MpRtpBuf.cpp \
    src/mp/MpUdpBuf.cpp \
    src/mp/MprAudioFrameBuffer.cpp \
    src/mp/MprFromInputDevice.cpp \
    src/mp/MprFromMic.cpp \
    src/mp/MprFromNet.cpp \
    src/mp/MprMixer.cpp \
    src/mp/MprnDTMFMsg.cpp \
    src/mp/MprnIntMsg.cpp \
    src/mp/MprnProgressMsg.cpp \
    src/mp/MprnRtpStreamActivityMsg.cpp \
    src/mp/MprNull.cpp \
    src/mp/MprNullAec.cpp \
    src/mp/MprRecorder.cpp \
    src/mp/MprRtpDispatcher.cpp \
    src/mp/MprRtpDispatcherActiveSsrcs.cpp \
    src/mp/MprRtpDispatcherIpAffinity.cpp \
    src/mp/MprSpeakerSelector.cpp \
    src/mp/MpSineWaveGeneratorDeviceDriver.cpp \
    src/mp/MpStaticCodecInit.cpp \
    src/mp/MprSpeexEchoCancel.cpp \
    src/mp/MprSpeexPreProcess.cpp \
    src/mp/MprSplitter.cpp \
    src/mp/MprToneGen.cpp \
    src/mp/MprToNet.cpp \
    src/mp/MprToOutputDevice.cpp \
    src/mp/MpRtpInputConnection.cpp \
    src/mp/MpRtpOutputConnection.cpp \
    src/mp/MprVad.cpp \
    src/mp/MprVoiceActivityNotifier.cpp \
    src/mp/MpStreamFeeder.cpp \
    src/mp/MpStreamMsg.cpp \
    src/mp/MpStreamPlayer.cpp \
    src/mp/MpStreamPlaylistPlayer.cpp \
    src/mp/MpStreamQueuePlayer.cpp \
    src/mp/MpSpeakerSelectBase.cpp \
    src/mp/MpPlcBase.cpp \
    src/mp/MpPlcSilence.cpp \
    src/mp/MpPlgStaffV1.cpp \
    src/mp/MpTopologyGraph.cpp \
    src/mp/MpTypes.cpp \
    src/mp/MpVadBase.cpp \
    src/mp/MpVadSimple.cpp \
    src/mp/NetInTask.cpp \
    src/mp/StreamBufferDataSource.cpp \
    src/mp/StreamDataSource.cpp \
    src/mp/StreamDataSourceListener.cpp \
    src/mp/StreamDecoderListener.cpp \
    src/mp/StreamFileDataSource.cpp \
    src/mp/StreamFormatDecoder.cpp \
    src/mp/StreamHttpDataSource.cpp \
    src/mp/StreamQueueingFormatDecoder.cpp \
    src/mp/StreamQueueMsg.cpp \
    src/mp/StreamRAWFormatDecoder.cpp \
    src/mp/StreamWAVFormatDecoder.cpp \
    src/rtcp/BaseClass.cpp \
    src/rtcp/ByeReport.cpp \
    src/rtcp/Message.cpp \
    src/rtcp/MsgQueue.cpp \
    src/rtcp/NetworkChannel.cpp \
    src/rtcp/ReceiverReport.cpp \
    src/rtcp/RTCManager.cpp \
    src/rtcp/RTCPConnection.cpp \
    src/rtcp/RTCPHeader.cpp \
    src/rtcp/RTCPRender.cpp \
    src/rtcp/RTCPSession.cpp \
    src/rtcp/RTCPSource.cpp \
    src/rtcp/RTCPTimer.cpp \
    src/rtcp/RTPHeader.cpp \
    src/rtcp/SenderReport.cpp \
    src/rtcp/SourceDescription.cpp

# Not immediately needed on Android
FOO_DONT_BUILD := \


LOCAL_CXXFLAGS += -D__pingtel_on_posix__ \
                  -DANDROID \
                  -DDEFINE_S_IREAD_IWRITE \
                  -DSIPX_TMPDIR=\"/usr/var/tmp\" -DSIPX_CONFDIR=\"/etc/sipxpbx\"

#ifeq ($(TARGET_ARCH),arm)
#	LOCAL_CFLAGS += -DARMv5_ASM
#endif

#ifeq ($(TARGET_BUILD_TYPE),debug)
#	LOCAL_CFLAGS += -DDEBUG
#endif

LOCAL_C_INCLUDES += \
    $(SIPX_HOME)/libpcre \
    $(SIPX_HOME)/sipXportLib/include \
    $(SIPX_HOME)/sipXsdpLib/include \
    $(SIPX_HOME)/sipXtackLib/include \
    $(SIPX_HOME)/sipXmediaLib/include

#LOCAL_SHARED_LIBRARIES := libpcre libsipXport libsipXsdp libsipXtack

LOCAL_STATIC_LIBRARIES := libsipXtack libsipXsdp libsipXport libpcre

LOCAL_LDLIBS += -lstdc++ -ldl

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
