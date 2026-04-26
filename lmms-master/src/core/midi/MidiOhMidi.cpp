#ifdef LMMS_BUILD_OHOS

#include "MidiOhMidi.h"
#include "MidiEventToByteSeq.h"
#include "Engine.h"
#include "ConfigManager.h"

#include <QDebug>
#include <QByteArray>

namespace lmms
{

MidiOhMidi::MidiOhMidi() :
	QThread(),
	MidiClient(),
	m_midiClientHandle( nullptr ),
	m_eventThrottle( 10000 ),
	m_running( false )
{
	qDebug() << "MidiOhMidi: Initializing HarmonyOS MIDI client";

	enumerateMidiDevices();

	OH_MidiClient_Create( &m_midiClientHandle, "LMMS_MIDI_Client" );
	if( m_midiClientHandle )
	{
		qDebug() << "MidiOhMidi: MIDI client created successfully";
		m_running = true;
		start();
	}
	else
	{
		qWarning() << "MidiOhMidi: Failed to create MIDI client - MIDI functionality disabled";
	}
}

MidiOhMidi::~MidiOhMidi()
{
	m_running = false;
	wait( 2000 );

	if( m_midiClientHandle )
	{
		OH_MidiClient_Destroy( m_midiClientHandle );
		m_midiClientHandle = nullptr;
	}

	qDebug() << "MidiOhMidi: Destroyed";
}

void MidiOhMidi::run()
{
	qDebug() << "MidiOhMidi: MIDI processing thread started";

	while( m_running )
	{
		msleep( 1 );
		processMidiInput();
	}

	qDebug() << "MidiOhMidi: MIDI processing thread stopped";
}

void MidiOhMidi::processOutEvent( const MidiEvent & _me,
				const TimePos & _time,
				const MidiPort * _port )
{
	Q_UNUSED( _time );

	if( !m_midiClientHandle || !_port )
	{
		return;
	}

	QString destName = m_portDeviceMap.value( _port );
	if( destName.isEmpty() )
	{
		return;
	}

	int32_t deviceId = m_outputDeviceIds.value( destName, -1 );
	if( deviceId < 0 )
	{
		return;
	}

	OH_MidiOutputPort * outputPort = nullptr;
	OH_MidiOutputPort_Create( m_midiClientHandle, deviceId, &outputPort );
	if( !outputPort )
	{
		return;
	}

	OH_MidiEvent ohEvent;
	ohEvent.timestamp = 0;

	uint8_t midiBytes[3];
	std::size_t byteCount = writeToByteSeq( _me, midiBytes, 3 );
	if( byteCount == 0 )
	{
		OH_MidiOutputPort_Destroy( outputPort );
		return;
	}

	ohEvent.opcode = midiBytes[0] & 0xF0;
	ohEvent.channel = midiBytes[0] & 0x0F;
	if( byteCount >= 2 ) ohEvent.data[0] = midiBytes[1];
	if( byteCount >= 3 ) ohEvent.data[1] = midiBytes[2];

	OH_MidiOutputPort_SendEvent( outputPort, &ohEvent );
	OH_MidiOutputPort_Destroy( outputPort );
}

MidiEvent MidiOhMidi::mapMidiEvent( int32_t opcode, int32_t channel, int32_t param1, int32_t param2 )
{
	MidiEventTypes type = static_cast<MidiEventTypes>( opcode );

	switch( opcode )
	{
		case 0x90:
			if( param2 == 0 )
			{
				type = MidiNoteOff;
			}
			else
			{
				type = MidiNoteOn;
			}
			return MidiEvent( type, channel, param1, param2, MidiEvent::Source::External );

		case 0x80:
			return MidiEvent( MidiNoteOff, channel, param1, param2, MidiEvent::Source::External );

		case 0xA0:
			return MidiEvent( MidiKeyPressure, channel, param1, param2, MidiEvent::Source::External );

		case 0xB0:
			return MidiEvent( MidiControlChange, channel, param1, param2, MidiEvent::Source::External );

		case 0xC0:
			return MidiEvent( MidiProgramChange, channel, param1, 0, MidiEvent::Source::External );

		case 0xD0:
			return MidiEvent( MidiChannelPressure, channel, param1, 0, MidiEvent::Source::External );

		case 0xE0:
		{
			int pitchBendValue = ( param2 << 7 ) | param1;
			return MidiEvent( MidiPitchBend, channel, pitchBendValue, 0, MidiEvent::Source::External );
		}

		default:
			return MidiEvent( MidiMetaEvent, channel, param1, param2, MidiEvent::Source::External );
	}
}

void MidiOhMidi::processMidiInput()
{
}

void MidiOhMidi::midiInputCallback( OH_MidiInputPort * port,
					OH_MidiInputEventData * data,
					void * userData )
{
	auto * self = static_cast<MidiOhMidi *>( userData );
	if( !self || !data )
	{
		return;
	}

	if( !self->m_eventThrottle.shouldProcess() )
	{
		return;
	}

	int32_t opcode = data->opcode;
	int32_t channel = data->channel;
	int32_t param1 = data->data[0];
	int32_t param2 = data->data[1];

	MidiEvent event = self->mapMidiEvent( opcode, channel, param1, param2 );

	const QMutexLocker ml( &self->m_mutex );
	for( MidiPort * midiPort : self->m_midiPorts )
	{
		midiPort->processInEvent( event );
	}
}

void MidiOhMidi::enumerateMidiDevices()
{
	m_inputDevices.clear();
	m_outputDevices.clear();
	m_inputDeviceIds.clear();
	m_outputDeviceIds.clear();

	if( !m_midiClientHandle )
	{
		return;
	}

	int32_t deviceCount = OH_MidiClient_EnumerateDevices( m_midiClientHandle );
	qDebug() << "MidiOhMidi: Enumerated" << deviceCount << "MIDI devices";

	for( int32_t i = 0; i < deviceCount; ++i )
	{
		OH_MidiDeviceInfo deviceInfo;
		if( OH_MidiClient_GetDeviceInfo( m_midiClientHandle, i, &deviceInfo ) == 0 )
		{
			QString name = QString::fromUtf8( deviceInfo.name );
			if( deviceInfo.isInput )
			{
				m_inputDevices.append( name );
				m_inputDeviceIds[name] = i;
				qDebug() << "MidiOhMidi: Input device:" << name << "id:" << i;
			}
			if( deviceInfo.isOutput )
			{
				m_outputDevices.append( name );
				m_outputDeviceIds[name] = i;
				qDebug() << "MidiOhMidi: Output device:" << name << "id:" << i;
			}
		}
	}
}

QStringList MidiOhMidi::readablePorts() const
{
	return m_inputDevices;
}

QStringList MidiOhMidi::writablePorts() const
{
	return m_outputDevices;
}

QString MidiOhMidi::sourcePortName( const MidiPort * _port ) const
{
	return m_portDeviceMap.value( _port, QString() );
}

QString MidiOhMidi::destinationPortName( const MidiPort * _port ) const
{
	return m_portDeviceMap.value( _port, QString() );
}

void MidiOhMidi::applyPortMode( MidiPort * _port )
{
	if( _port )
	{
		registerPort( _port );
	}
}

void MidiOhMidi::applyPortName( MidiPort * _port )
{
	Q_UNUSED( _port );
}

void MidiOhMidi::removePort( MidiPort * _port )
{
	unregisterPort( _port );
	MidiClient::removePort( _port );
}

void MidiOhMidi::subscribeReadablePort( MidiPort * _port, const QString & _dest, bool _subscribe )
{
	Q_UNUSED( _port );
	Q_UNUSED( _dest );
	Q_UNUSED( _subscribe );
}

void MidiOhMidi::subscribeWritablePort( MidiPort * _port, const QString & _dest, bool _subscribe )
{
	Q_UNUSED( _port );
	Q_UNUSED( _dest );
	Q_UNUSED( _subscribe );
}

void MidiOhMidi::updatePortList()
{
	const QMutexLocker ml( &m_mutex );
	enumerateMidiDevices();
}

void MidiOhMidi::registerPort( MidiPort * _port )
{
	if( _port && !m_portDeviceMap.contains( _port ) )
	{
		m_portDeviceMap[_port] = _port->name();
		qDebug() << "MidiOhMidi: Registered port" << _port->name();
	}
}

void MidiOhMidi::unregisterPort( MidiPort * _port )
{
	if( _port )
	{
		m_portDeviceMap.remove( _port );
		qDebug() << "MidiOhMidi: Unregistered port" << _port->name();
	}
}

QString MidiOhMidi::probeDevice()
{
	return "HarmonyOS MIDI";
}

QStringList MidiOhMidi::probeDevices()
{
	QStringList devices;
	devices << "HarmonyOS MIDI";
	return devices;
}

} // namespace lmms

#endif // LMMS_BUILD_OHOS
