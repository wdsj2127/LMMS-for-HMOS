export interface QtAppConfig {
  APP_LIBRARY_NAME: string;
  QT_PLUGIN_PATH: string;
  QT_QPA_PLATFORM: string;
  QT_PLATFORM_PLUGIN: string;
}

export const QtAppConstants: QtAppConfig = {
  APP_LIBRARY_NAME: 'liblmms.so',
  QT_PLUGIN_PATH: '/data/storage/el1/bundle/libs/arm64',
  QT_QPA_PLATFORM: 'ohos',
  QT_PLATFORM_PLUGIN: 'libqtaf_ohos.so',
};

export default QtAppConstants;
