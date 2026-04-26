#ifndef MIDI_OH_MIDI_H
#define MIDI_OH_MIDI_H

#ifdef LMMS_BUILD_OHOS

#include "MidiClient.h"
#include "MidiPort.h"

#include <QObject>
#include <QMap>
#include <QString>

class MidiOhMidi : public QObject, public MidiClient
{
	Q_OBJECT
public:
	MidiOhMidi();
	~MidiOhMidi() override;

	void startProcessing() override;
	void stopProcessing() override;

	QString sourcePortName( const MidiPort * _port ) const override;
	QString destinationPortName( const MidiPort * _port ) const override;

	void applyPortMode( MidiPort * _port ) override;
	void removePort( MidiPort * _port ) override;

	void updatePortList() override;

	static QString probeDevice();
	static QStringList probeDevices();

private:
	void registerPort( MidiPort * _port );
	void unregisterPort( MidiPort * _port );

	bool m_running;
	QMap<const MidiPort *, QString> m_portDeviceMap;
};

#endif // LMMS_BUILD_OHOS

#endif // MIDI_OH_MIDI_H
