#ifdef LMMS_BUILD_OHOS

#include "OhosPluginLoader.h"
#include "OhosFileSystem.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <dlfcn.h>

OhosPluginLoader::OhosPluginLoader( QObject * _parent ) :
	QObject( _parent ),
	m_pluginDir( OhosFileSystem::pluginDir() )
{
}

OhosPluginLoader::~OhosPluginLoader()
{
	unloadAll();
}

void OhosPluginLoader::setPluginDir( const QString & _dir )
{
	m_pluginDir = _dir;
}

const QString & OhosPluginLoader::pluginDir() const
{
	return m_pluginDir;
}

QStringList OhosPluginLoader::discover()
{
	QStringList found;
	QDir dir( m_pluginDir );

	if( !dir.exists() )
	{
		qWarning() << "OhosPluginLoader: Plugin directory does not exist:" << m_pluginDir;
		return found;
	}

	QStringList filters;
	filters << "lib*.so";
	QFileInfoList entries = dir.entryInfoList( filters, QDir::Files | QDir::NoDotAndDotDot );

	for( const QFileInfo & entry : entries )
	{
		found.append( entry.absoluteFilePath() );
		qDebug() << "OhosPluginLoader: Discovered plugin:" << entry.fileName();
	}

	qDebug() << "OhosPluginLoader: Discovered" << found.size() << "plugins in" << m_pluginDir;
	return found;
}

void * OhosPluginLoader::load( const QString & _path )
{
	if( m_loadedLibs.contains( _path ) )
	{
		return m_loadedLibs[_path];
	}

	void * handle = dlopen( _path.toLocal8Bit().constData(), RTLD_NOW );
	if( !handle )
	{
		qCritical() << "OhosPluginLoader: Failed to load plugin:" << _path
			<< "error:" << dlerror();
		return nullptr;
	}

	m_loadedLibs[_path] = handle;
	qDebug() << "OhosPluginLoader: Loaded plugin:" << QFileInfo( _path ).fileName();
	return handle;
}

bool OhosPluginLoader::unload( const QString & _path )
{
	if( !m_loadedLibs.contains( _path ) )
	{
		return false;
	}

	void * handle = m_loadedLibs.take( _path );
	int result = dlclose( handle );
	if( result != 0 )
	{
		qWarning() << "OhosPluginLoader: Failed to unload plugin:" << _path
			<< "error:" << dlerror();
		return false;
	}

	qDebug() << "OhosPluginLoader: Unloaded plugin:" << QFileInfo( _path ).fileName();
	return true;
}

void OhosPluginLoader::unloadAll()
{
	QStringList paths = m_loadedLibs.keys();
	for( const QString & path : paths )
	{
		unload( path );
	}
}

#endif // LMMS_BUILD_OHOS
