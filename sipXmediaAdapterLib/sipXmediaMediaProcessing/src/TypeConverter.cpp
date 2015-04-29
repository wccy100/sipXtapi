#include <TypeConverter.h>
#include <os/OsSysLog.h>

OsStatus TypeConverter::translateRecordingFormat(CpMediaInterface::CpAudioFileFormat cpFileFormat,
	MprRecorder::RecordFileFormat & recordFormat)
{
	recordFormat = MprRecorder::WAV_PCM_16;
	switch (cpFileFormat)
	{
	case CpMediaInterface::CpAudioFileFormat::CP_WAVE_PCM_16:
		recordFormat = MprRecorder::WAV_PCM_16;
		return OS_SUCCESS;

	case CpMediaInterface::CpAudioFileFormat::CP_WAVE_GSM:
		recordFormat = MprRecorder::WAV_GSM;
		return OS_SUCCESS;

	case CpMediaInterface::CpAudioFileFormat::CP_WAVE_ALAW:
		recordFormat = MprRecorder::WAV_ALAW;
		return OS_SUCCESS;

	case CpMediaInterface::CpAudioFileFormat::CP_WAVE_MULAW:
		recordFormat = MprRecorder::WAV_MULAW;
		return OS_SUCCESS;

	default:
		OsSysLog::add(FAC_CP, PRI_ERR,
			"Invalid record file format: %d",
			cpFileFormat);
		OsSysLog::flush();
		assert(0);
		return OS_INVALID_ARGUMENT;
	}
}
