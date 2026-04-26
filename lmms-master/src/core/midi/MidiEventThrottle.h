#ifndef MIDI_EVENT_THROTTLE_H
#define MIDI_EVENT_THROTTLE_H

#ifdef LMMS_BUILD_OHOS

#include <atomic>
#include <cstdint>

class MidiEventThrottle
{
public:
	explicit MidiEventThrottle( uint32_t maxEventsPerSecond = 10000 );

	bool shouldProcess();
	void reset();

	void setMaxEventsPerSecond( uint32_t max );
	uint32_t maxEventsPerSecond() const;
	uint32_t droppedCount() const;

private:
	std::atomic<uint32_t> m_eventCount;
	std::atomic<uint32_t> m_maxEventsPerSecond;
	std::atomic<uint32_t> m_droppedCount;
	std::atomic<int64_t> m_lastResetTimeMs;
};

#endif // LMMS_BUILD_OHOS

#endif // MIDI_EVENT_THROTTLE_H
