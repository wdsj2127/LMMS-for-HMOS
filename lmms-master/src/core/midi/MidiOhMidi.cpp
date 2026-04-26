#ifdef LMMS_BUILD_OHOS

#include "MidiOhMidi.h"
#include "MidiEventToByteSeq.h"
#include "Engine.h"
#include "ConfigManager.h"

#include <QDebug>

MidiOhMidi::MidiOhMidi() :
	QObject(),
	MidiClient(),
	m_running( false )
{
	qDebug() << "MidiOhMidi: Initializing HarmonyOS MIDI client";
}

MidiOhMidi::~MidiOhMidi()
{
	stopProcessing();
	qDebug() << "MidiOhMidi: Destroyed";
}

void MidiOhMidi::startProcessing()
{
	if( m_running )
	{
		return;
	}
	m_running = true;
	qDebug() << "MidiOhMidi: Processing started";
}

void MidiOhMidi::stopProcessing()
{
	if( !m_running )
	{
		return;
	}
	m_running = false;
	qDebug() << "MidiOhMidi: Processing stopped";
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

void MidiOhMidi::removePort( MidiPort * _port )
{
	unregisterPort( _port );
}

void MidiOhMidi::updatePortList()
{
	qDebug() << "MidiOhMidi: Updating port list";
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

#endif // LMMS_BUILD_OHOS
