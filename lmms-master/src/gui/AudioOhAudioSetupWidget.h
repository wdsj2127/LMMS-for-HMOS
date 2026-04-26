#ifndef AUDIO_OH_AUDIO_SETUP_WIDGET_H
#define AUDIO_OH_AUDIO_SETUP_WIDGET_H

#ifdef LMMS_BUILD_OHOS

#include "AudioDeviceSetupWidget.h"

class QComboBox;

class AudioOhAudioSetupWidget : public AudioDeviceSetupWidget
{
	Q_OBJECT
public:
	AudioOhAudioSetupWidget( QWidget * _parent = nullptr );
	~AudioOhAudioSetupWidget() override;

	void saveSettings() override;
	void loadSettings() override;

private:
	QComboBox * m_sampleRateCombo;
	QComboBox * m_bufferSizeCombo;
};

#endif // LMMS_BUILD_OHOS

#endif // AUDIO_OH_AUDIO_SETUP_WIDGET_H
