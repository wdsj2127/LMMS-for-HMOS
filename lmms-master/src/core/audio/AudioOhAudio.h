#ifndef AUDIO_OH_AUDIO_H
#define AUDIO_OH_AUDIO_H

#include "AudioDevice.h"

#ifdef LMMS_BUILD_OHOS

#include <ohaudio/native_audio_channel.h>
#include <ohaudio/native_audio_common.h>
#include <ohaudio/native_audio_renderer.h>
#include <ohaudio/native_audio_stream.h>

class AudioOhAudio : public AudioDevice
{
	Q_OBJECT
public:
	AudioOhAudio( bool & _success_ful, Mixer * _mixer );
	~AudioOhAudio() override;

	void startProcessing() override;
	void stopProcessing() override;

	QString name() const override
	{
		return "OHAudio";
	}

	static void setupAudioDeviceSetupWidget( AudioDeviceSetupWidget * _widget );
	static AudioDevice * instantiate( bool & _success_ful, Mixer * _mixer,
						const ch_cnt_t _channels );

private:
	void writeBuffer( AudioSampleBuffer * _abuf, const fpp_t _frames,
				const ch_cnt_t _channels ) override;

	static int32_t audioRendererCallback( OH_AudioRenderer * renderer,
						void * userData,
						int32_t bufferLen );
	int32_t processAudioData( float * buffer, int32_t bufferLen );

	OH_AudioRenderer * m_renderer;
	OH_AudioStreamBuilder * m_builder;
	bool m_running;
	uint32_t m_bufferUnderrunCount;
};

#endif // LMMS_BUILD_OHOS

#endif // AUDIO_OH_AUDIO_H
