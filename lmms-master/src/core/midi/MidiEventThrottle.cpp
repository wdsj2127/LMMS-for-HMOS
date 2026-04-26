#ifdef LMMS_BUILD_OHOS

#include "MidiEventThrottle.h"

#include <chrono>

MidiEventThrottle::MidiEventThrottle( uint32_t maxEventsPerSecond ) :
	m_eventCount( 0 ),
	m_maxEventsPerSecond( maxEventsPerSecond ),
	m_droppedCount( 0 ),
	m_lastResetTimeMs( 0 )
{
	auto now = std::chrono::steady_clock::now();
	m_lastResetTimeMs.store(
		std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ).count()
	);
}

bool MidiEventThrottle::shouldProcess()
{
	auto now = std::chrono::steady_clock::now();
	int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch() ).count();

	int64_t lastReset = m_lastResetTimeMs.load( std::memory_order_relaxed );
	if( nowMs - lastReset >= 1000 )
	{
		m_eventCount.store( 0, std::memory_order_relaxed );
		m_lastResetTimeMs.store( nowMs, std::memory_order_relaxed );
	}

	uint32_t count = m_eventCount.fetch_add( 1, std::memory_order_relaxed );
	if( count < m_maxEventsPerSecond.load( std::memory_order_relaxed ) )
	{
		return true;
	}

	m_droppedCount.fetch_add( 1, std::memory_order_relaxed );
	return false;
}

void MidiEventThrottle::reset()
{
	m_eventCount.store( 0, std::memory_order_relaxed );
	m_droppedCount.store( 0, std::memory_order_relaxed );
}

void MidiEventThrottle::setMaxEventsPerSecond( uint32_t max )
{
	m_maxEventsPerSecond.store( max, std::memory_order_relaxed );
}

uint32_t MidiEventThrottle::maxEventsPerSecond() const
{
	return m_maxEventsPerSecond.load( std::memory_order_relaxed );
}

uint32_t MidiEventThrottle::droppedCount() const
{
	return m_droppedCount.load( std::memory_order_relaxed );
}

#endif // LMMS_BUILD_OHOS
