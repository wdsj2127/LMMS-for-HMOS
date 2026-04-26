#ifdef LMMS_BUILD_OHOS

#include "PluginCrashGuard.h"
#include "PluginFactory.h"

#include <QDebug>
#include <csignal>
#include <dlfcn.h>

PluginCrashGuard * PluginCrashGuard::s_instance = nullptr;

PluginCrashGuard::PluginCrashGuard( QObject * _parent ) :
	QObject( _parent )
{
	s_instance = this;
}

PluginCrashGuard::~PluginCrashGuard()
{
	removeHandlers();
	s_instance = nullptr;
}

PluginCrashGuard * PluginCrashGuard::instance()
{
	return s_instance;
}

void PluginCrashGuard::registerPlugin( const QString & _name, void * _handle )
{
	m_pluginHandles[_name] = _handle;
}

void PluginCrashGuard::unregisterPlugin( const QString & _name )
{
	m_pluginHandles.remove( _name );
}

void PluginCrashGuard::installHandlers()
{
	std::signal( SIGSEGV, crashSignalHandler );
	std::signal( SIGABRT, crashSignalHandler );
	qDebug() << "PluginCrashGuard: Signal handlers installed";
}

void PluginCrashGuard::removeHandlers()
{
	std::signal( SIGSEGV, SIG_DFL );
	std::signal( SIGABRT, SIG_DFL );
	qDebug() << "PluginCrashGuard: Signal handlers removed";
}

void PluginCrashGuard::crashSignalHandler( int _signal )
{
	if( !s_instance )
	{
		std::_Exit( 1 );
	}

	void * addr = __builtin_return_address( 0 );
	QString pluginName = s_instance->findPluginByAddress( addr );

	qCritical() << "PluginCrashGuard: Signal" << _signal
		<< "in plugin:" << ( pluginName.isEmpty() ? "unknown" : pluginName );

	if( !pluginName.isEmpty() )
	{
		void * handle = s_instance->m_pluginHandles.take( pluginName );
		if( handle )
		{
			dlclose( handle );
		}
		emit s_instance->pluginCrashed( pluginName );
	}

	std::_Exit( 1 );
}

QString PluginCrashGuard::findPluginByAddress( void * _addr )
{
	Q_UNUSED( _addr )
	return QString();
}

#endif // LMMS_BUILD_OHOS
