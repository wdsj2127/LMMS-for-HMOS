#ifdef LMMS_BUILD_OHOS

#include "OhosFileSystem.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

QString OhosFileSystem::s_appDataDir;
QString OhosFileSystem::s_appCacheDir;
QString OhosFileSystem::s_appResourceDir;
bool OhosFileSystem::s_initialized = false;

void OhosFileSystem::initialize( const QString & bundleDataDir,
				const QString & bundleCacheDir,
				const QString & bundleResourceDir )
{
	s_appDataDir = bundleDataDir;
	s_appCacheDir = bundleCacheDir;
	s_appResourceDir = bundleResourceDir;
	s_initialized = true;

	QDir().mkpath( appDataDir() );
	QDir().mkpath( configDir() );
	QDir().mkpath( appCacheDir() );

	qDebug() << "OhosFileSystem initialized:";
	qDebug() << "  appDataDir:" << s_appDataDir;
	qDebug() << "  appCacheDir:" << s_appCacheDir;
	qDebug() << "  appResourceDir:" << s_appResourceDir;
}

bool OhosFileSystem::isInitialized()
{
	return s_initialized;
}

QString OhosFileSystem::appDataDir()
{
	if( s_initialized )
	{
		return s_appDataDir;
	}
	return "/data/storage/el2/base/files";
}

QString OhosFileSystem::appCacheDir()
{
	if( s_initialized )
	{
		return s_appCacheDir;
	}
	return "/data/storage/el2/base/cache";
}

QString OhosFileSystem::appResourceDir()
{
	if( s_initialized )
	{
		return s_appResourceDir;
	}
	return "/data/storage/el1/bundle/files";
}

QString OhosFileSystem::pluginDir()
{
	return appResourceDir() + "/plugins";
}

QString OhosFileSystem::lmmsDataDir()
{
	return appResourceDir() + "/lmms";
}

QString OhosFileSystem::samplesDir()
{
	return lmmsDataDir() + "/samples";
}

QString OhosFileSystem::presetsDir()
{
	return lmmsDataDir() + "/presets";
}

QString OhosFileSystem::configDir()
{
	return appDataDir() + "/.lmms";
}

QString OhosFileSystem::mapPath( const QString & originalPath )
{
	if( originalPath.startsWith( "~/.lmms" ) || originalPath.startsWith( "$HOME/.lmms" ) )
	{
		return configDir() + originalPath.mid( originalPath.indexOf( "/.lmms" ) + 6 );
	}
	if( originalPath.startsWith( "/usr/share/lmms" ) || originalPath.startsWith( "/usr/local/share/lmms" ) )
	{
		return lmmsDataDir() + originalPath.mid( originalPath.indexOf( "/lmms" ) + 5 );
	}
	if( originalPath.startsWith( "plugins" ) || originalPath.startsWith( "plugins/" ) )
	{
		return pluginDir() + originalPath.mid( 7 );
	}
	if( originalPath.startsWith( "data" ) || originalPath.startsWith( "data/" ) )
	{
		return appResourceDir() + "/" + originalPath;
	}
	return originalPath;
}

#endif // LMMS_BUILD_OHOS
