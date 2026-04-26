#ifdef LMMS_BUILD_OHOS

#include "AudioOhAudio.h"
#include "AudioEngine.h"
#include "ConfigManager.h"
#include "Engine.h"
#include "Mixer.h"
#include "MainWindow.h"

#include <QDebug>
#include <QMessageBox>

AudioOhAudio::AudioOhAudio( bool & _success_ful, Mixer * _mixer ) :
	AudioDevice( _mixer ),
	m_renderer( nullptr ),
	m_builder( nullptr ),
	m_running( false ),
	m_bufferUnderrunCount( 0 )
{
	_success_ful = false;

	OH_AudioStream_Result result = OH_AudioStreamBuilder_Create( &m_builder, AUDIOSTREAM_TYPE_RENDERER );
	if( result != AUDIOSTREAM_SUCCESS )
	{
		qCritical() << "AudioOhAudio: Failed to create audio stream builder, result =" << result;
		return;
	}

	sample_rate_t sr = sampleRate();
	OH_AudioStreamBuilder_SetSamplingRate( m_builder, sr );
	OH_AudioStreamBuilder_SetChannelCount( m_builder, channels() );
	OH_AudioStreamBuilder_SetSampleFormat( m_builder, AUDIOSTREAM_SAMPLEFORMAT_FLOAT32 );
	OH_AudioStreamBuilder_SetRendererCallback( m_builder, audioRendererCallback, this );
	OH_AudioStreamBuilder_SetLatencyMode( m_builder, AUDIOSTREAM_LATENCY_MODE_LOW );

	int64_t bufferDurationNs = static_cast<int64_t>( bufferSize() ) * 1000000000LL / sr;
	OH_AudioStreamBuilder_SetBufferDuration( m_builder, bufferDurationNs );

	result = OH_AudioStreamBuilder_GenerateRenderer( m_builder, &m_renderer );
	if( result != AUDIOSTREAM_SUCCESS )
	{
		qCritical() << "AudioOhAudio: Failed to generate audio renderer, result =" << result;
		OH_AudioStreamBuilder_Destroy( m_builder );
		m_builder = nullptr;
		return;
	}

	_success_ful = true;
	qDebug() << "AudioOhAudio: Initialized successfully"
		<< "sampleRate:" << sr
		<< "channels:" << channels()
		<< "bufferSize:" << bufferSize()
		<< "bufferDurationNs:" << bufferDurationNs;
}

AudioOhAudio::~AudioOhAudio()
{
	stopProcessing();

	if( m_renderer )
	{
		OH_AudioRenderer_Destroy( m_renderer );
		m_renderer = nullptr;
	}
	if( m_builder )
	{
		OH_AudioStreamBuilder_Destroy( m_builder );
		m_builder = nullptr;
	}
}

void AudioOhAudio::startProcessing()
{
	if( m_running || !m_renderer )
	{
		return;
	}

	OH_AudioStream_Result result = OH_AudioRenderer_Start( m_renderer );
	if( result == AUDIOSTREAM_SUCCESS )
	{
		m_running = true;
		m_bufferUnderrunCount = 0;
		qDebug() << "AudioOhAudio: Audio rendering started";
	}
	else
	{
		qCritical() << "AudioOhAudio: Failed to start audio renderer, result =" << result;
	}
}

void AudioOhAudio::stopProcessing()
{
	if( !m_running || !m_renderer )
	{
		return;
	}

	OH_AudioStream_Result result = OH_AudioRenderer_Stop( m_renderer );
	if( result == AUDIOSTREAM_SUCCESS )
	{
		m_running = false;
		qDebug() << "AudioOhAudio: Audio rendering stopped"
			<< "bufferUnderrunCount:" << m_bufferUnderrunCount;
	}
	else
	{
		qCritical() << "AudioOhAudio: Failed to stop audio renderer, result =" << result;
	}
}

void AudioOhAudio::writeBuffer( AudioSampleBuffer * _abuf, const fpp_t _frames,
				const ch_cnt_t _channels )
{
}

int32_t AudioOhAudio::audioRendererCallback( OH_AudioRenderer * renderer,
						void * userData,
						int32_t bufferLen )
{
	auto * self = static_cast<AudioOhAudio *>( userData );
	float * buffer = nullptr;
	OH_AudioRenderer_GetCurrentTime( renderer, nullptr );
	return self->processAudioData( buffer, bufferLen );
}

int32_t AudioOhAudio::processAudioData( float * buffer, int32_t bufferLen )
{
	const fpp_t frames = static_cast<fpp_t>( bufferLen / channels() );

	const AudioSampleBuffer * rendered = mixer()->renderNextBuffer();
	if( !rendered || !buffer )
	{
		if( buffer )
		{
			memset( buffer, 0, bufferLen * sizeof( float ) );
		}
		m_bufferUnderrunCount++;
		if( m_bufferUnderrunCount % 100 == 0 )
		{
			qWarning() << "AudioOhAudio: Buffer underrun count:" << m_bufferUnderrunCount;
		}
		return 0;
	}

	const ch_cnt_t ch = std::min( channels(), rendered->channels() );
	for( fpp_t frame = 0; frame < frames; ++frame )
	{
		for( ch_cnt_t c = 0; c < ch; ++c )
		{
			buffer[frame * channels() + c] = rendered->data()[c][frame];
		}
		for( ch_cnt_t c = ch; c < channels(); ++c )
		{
			buffer[frame * channels() + c] = 0.0f;
		}
	}

	return 0;
}

AudioDevice * AudioOhAudio::instantiate( bool & _success_ful, Mixer * _mixer,
						const ch_cnt_t _channels )
{
	return new AudioOhAudio( _success_ful, _mixer );
}

void AudioOhAudio::setupAudioDeviceSetupWidget( AudioDeviceSetupWidget * _widget )
{
}

#endif // LMMS_BUILD_OHOS
