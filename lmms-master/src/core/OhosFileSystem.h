#ifndef OHOS_FILE_SYSTEM_H
#define OHOS_FILE_SYSTEM_H

#ifdef LMMS_BUILD_OHOS

#include <QString>
#include <QStringList>

class OhosFileSystem
{
public:
	OhosFileSystem() = delete;

	static QString appDataDir();
	static QString appCacheDir();
	static QString appResourceDir();
	static QString pluginDir();
	static QString lmmsDataDir();
	static QString samplesDir();
	static QString presetsDir();
	static QString configDir();
	static QString mapPath( const QString & originalPath );

	static void initialize( const QString & bundleDataDir,
				const QString & bundleCacheDir,
				const QString & bundleResourceDir );

	static bool isInitialized();

private:
	static QString s_appDataDir;
	static QString s_appCacheDir;
	static QString s_appResourceDir;
	static bool s_initialized;
};

#endif // LMMS_BUILD_OHOS

#endif // OHOS_FILE_SYSTEM_H
