#ifndef MIDI_OH_MIDI_H
#define MIDI_OH_MIDI_H

#ifdef LMMS_BUILD_OHOS

#include "MidiClient.h"
#include "MidiPort.h"
#include "MidiEventThrottle.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QThread>

#include <ohmidi/native_midi_client.h>
#include <ohmidi/native_midi_input_port.h>
#include <ohmidi/native_midi_output_port.h>
#include <ohmidi/native_midi_device_info.h>

namespace lmms
{

class MidiOhMidi : public QThread, public MidiClient
{
	Q_OBJECT
public:
	MidiOhMidi();
	~MidiOhMidi() override;

	void processOutEvent( const MidiEvent & _me,
				const TimePos & _time,
				const MidiPort * _port ) override;

	void applyPortMode( MidiPort * _port ) override;
	void applyPortName( MidiPort * _port ) override;
	void removePort( MidiPort * _port ) override;

	QStringList readablePorts() const override;
	QStringList writablePorts() const override;

	QString sourcePortName( const MidiPort * _port ) const override;
	QString destinationPortName( const MidiPort * _port ) const override;

	void subscribeReadablePort( MidiPort * _port, const QString & _dest, bool _subscribe = true ) override;
	void subscribeWritablePort( MidiPort * _port, const QString & _dest, bool _subscribe = true ) override;

	void updatePortList() override;

	void run() override;

	static QString probeDevice();
	static QStringList probeDevices();

private:
	void registerPort( MidiPort * _port );
	void unregisterPort( MidiPort * _port );
	MidiEvent mapMidiEvent( int32_t opcode, int32_t channel, int32_t param1, int32_t param2 );
	void processMidiInput();
	void enumerateMidiDevices();

	static void midiInputCallback( OH_MidiInputPort * port,
					OH_MidiInputEventData * data,
					void * userData );

	OH_MidiClient * m_midiClientHandle;
	QStringList m_inputDevices;
	QStringList m_outputDevices;
	QMap<const MidiPort *, QString> m_portDeviceMap;
	QMap<QString, int32_t> m_inputDeviceIds;
	QMap<QString, int32_t> m_outputDeviceIds;
	MidiEventThrottle m_eventThrottle;
	QMutex m_mutex;
	bool m_running;
};

} // namespace lmms

#endif // LMMS_BUILD_OHOS

#endif // MIDI_OH_MIDI_H
