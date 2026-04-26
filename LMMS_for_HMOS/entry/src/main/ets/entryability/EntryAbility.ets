import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import QtAppConstants from '../common/QtAppConstants';

const DOMAIN = 0x0000;
const TAG = 'LMMS_EntryAbility';

export default class EntryAbility extends UIAbility {
    onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
        try {
            this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
        } catch (err) {
            hilog.error(DOMAIN, TAG, 'Failed to set colorMode: %{public}s', JSON.stringify(err));
        }
        hilog.info(DOMAIN, TAG, 'LMMS for HarmonyOS onCreate');

        AppStorage.setOrCreate<string>('qtAppLibName', QtAppConstants.APP_LIBRARY_NAME);
        AppStorage.setOrCreate<string>('qtQpaPlatform', QtAppConstants.QT_QPA_PLATFORM);
        AppStorage.setOrCreate<string>('qtPluginPath', QtAppConstants.QT_PLUGIN_PATH);

        hilog.info(DOMAIN, TAG, 'Qt app configured: %{public}s', QtAppConstants.APP_LIBRARY_NAME);
    }

    onDestroy(): void {
        hilog.info(DOMAIN, TAG, 'LMMS for HarmonyOS onDestroy');
    }

    onWindowStageCreate(windowStage: window.WindowStage): void {
        hilog.info(DOMAIN, TAG, 'Ability onWindowStageCreate');

        windowStage.loadContent('pages/Index', (err) => {
            if (err.code) {
                hilog.error(DOMAIN, TAG, 'Failed to load content: %{public}s', JSON.stringify(err));
                return;
            }
            hilog.info(DOMAIN, TAG, 'Succeeded in loading content');
        });
    }

    onWindowStageDestroy(): void {
        hilog.info(DOMAIN, TAG, 'Ability onWindowStageDestroy');
    }

    onForeground(): void {
        hilog.info(DOMAIN, TAG, 'Ability onForeground');
    }

    onBackground(): void {
        hilog.info(DOMAIN, TAG, 'Ability onBackground');
    }
}
