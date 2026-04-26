#ifndef PLUGIN_CRASH_GUARD_H
#define PLUGIN_CRASH_GUARD_H

#ifdef LMMS_BUILD_OHOS

#include <QObject>
#include <QString>
#include <QMap>

class PluginCrashGuard : public QObject
{
	Q_OBJECT
public:
	explicit PluginCrashGuard( QObject * _parent = nullptr );
	~PluginCrashGuard();

	void registerPlugin( const QString & _name, void * _handle );
	void unregisterPlugin( const QString & _name );
	void installHandlers();
	void removeHandlers();

	static PluginCrashGuard * instance();

signals:
	void pluginCrashed( const QString & _pluginName );

private:
	static void crashSignalHandler( int _signal );
	QString findPluginByAddress( void * _addr );

	QMap<QString, void *> m_pluginHandles;
	static PluginCrashGuard * s_instance;
};

#endif // LMMS_BUILD_OHOS

#endif // PLUGIN_CRASH_GUARD_H
