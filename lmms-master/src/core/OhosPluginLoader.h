#ifndef OHOS_PLUGIN_LOADER_H
#define OHOS_PLUGIN_LOADER_H

#ifdef LMMS_BUILD_OHOS

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>

class Plugin;

class OhosPluginLoader : public QObject
{
	Q_OBJECT
public:
	explicit OhosPluginLoader( QObject * _parent = nullptr );
	~OhosPluginLoader();

	void setPluginDir( const QString & _dir );
	const QString & pluginDir() const;

	QStringList discover();
	void * load( const QString & _path );
	bool unload( const QString & _path );
	void unloadAll();

private:
	QString m_pluginDir;
	QMap<QString, void *> m_loadedLibs;
};

#endif // LMMS_BUILD_OHOS

#endif // OHOS_PLUGIN_LOADER_H
