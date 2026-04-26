#ifdef LMMS_BUILD_OHOS

#include "AudioOhAudioSetupWidget.h"
#include "AudioOhAudio.h"
#include "ConfigManager.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

AudioOhAudioSetupWidget::AudioOhAudioSetupWidget( QWidget * _parent ) :
	AudioDeviceSetupWidget( AudioOhAudio::name(), _parent ),
	m_sampleRateCombo( nullptr ),
	m_bufferSizeCombo( nullptr )
{
	auto * mainLayout = new QVBoxLayout( this );
	auto * configGroup = new QGroupBox( tr( "OHAudio Configuration" ), this );
	auto * formLayout = new QFormLayout( configGroup );

	m_sampleRateCombo = new QComboBox( this );
	m_sampleRateCombo->addItem( "44100", 44100 );
	m_sampleRateCombo->addItem( "48000", 48000 );
	formLayout->addRow( tr( "Sample Rate:" ), m_sampleRateCombo );

	m_bufferSizeCombo = new QComboBox( this );
	m_bufferSizeCombo->addItem( "128", 128 );
	m_bufferSizeCombo->addItem( "256", 256 );
	m_bufferSizeCombo->addItem( "512", 512 );
	m_bufferSizeCombo->addItem( "1024", 1024 );
	formLayout->addRow( tr( "Buffer Size:" ), m_bufferSizeCombo );

	mainLayout->addWidget( configGroup );
	mainLayout->addStretch();

	loadSettings();
}

AudioOhAudioSetupWidget::~AudioOhAudioSetupWidget()
{
}

void AudioOhAudioSetupWidget::saveSettings()
{
	ConfigManager::inst()->setValue( "audioohaudio", "samplerate",
		QString::number( m_sampleRateCombo->currentData().toInt() ) );
	ConfigManager::inst()->setValue( "audioohaudio", "buffersize",
		QString::number( m_bufferSizeCombo->currentData().toInt() ) );
}

void AudioOhAudioSetupWidget::loadSettings()
{
	int sampleRate = ConfigManager::inst()->value( "audioohaudio", "samplerate", "44100" ).toInt();
	int bufferSize = ConfigManager::inst()->value( "audioohaudio", "buffersize", "256" ).toInt();

	int srIdx = m_sampleRateCombo->findData( sampleRate );
	if( srIdx >= 0 ) m_sampleRateCombo->setCurrentIndex( srIdx );

	int bsIdx = m_bufferSizeCombo->findData( bufferSize );
	if( bsIdx >= 0 ) m_bufferSizeCombo->setCurrentIndex( bsIdx );
}

#endif // LMMS_BUILD_OHOS
