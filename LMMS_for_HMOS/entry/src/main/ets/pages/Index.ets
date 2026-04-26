import QtAppConstants from '../common/QtAppConstants';

@Entry
@Component
struct Index {
    @State statusMessage: string = 'LMMS for HarmonyOS';
    @State statusDetail: string = 'Loading Qt Application...';
    @State isQtLoaded: boolean = false;

    aboutToAppear(): void {
        this.initQtApp();
    }

    private initQtApp(): void {
        const qtLibName: string = AppStorage.get<string>('qtAppLibName') ?? '';
        if (qtLibName.length > 0) {
            this.statusDetail = 'Qt app: ' + qtLibName;
            this.isQtLoaded = true;
        } else {
            this.statusDetail = QtAppConstants.APP_LIBRARY_NAME + ' - Qt runtime pending.';
        }
    }

    build() {
        Column() {
            Text(this.statusMessage)
                .fontSize(28)
                .fontWeight(FontWeight.Bold)
                .fontColor('#FFFFFF')
                .margin({ bottom: 16 })

            Text(this.statusDetail)
                .fontSize(16)
                .fontColor('#AAAAAA')
                .margin({ bottom: 32 })

            if (!this.isQtLoaded) {
                Column() {
                    Text('Setup Instructions:')
                        .fontSize(14)
                        .fontWeight(FontWeight.Bold)
                        .fontColor('#CCCCCC')
                        .margin({ bottom: 8 })

                    Text('1. Build Qt 5.15.16 for HarmonyOS')
                        .fontSize(12)
                        .fontColor('#999999')
                        .margin({ bottom: 4 })

                    Text('2. Cross-compile LMMS with ohos-clang')
                        .fontSize(12)
                        .fontColor('#999999')
                        .margin({ bottom: 4 })

                    Text('3. Copy .so files to entry/libs/arm64-v8a/')
                        .fontSize(12)
                        .fontColor('#999999')
                        .margin({ bottom: 4 })

                    Text('4. Set APP_LIBRARY_NAME in QtAppConstants.ets')
                        .fontSize(12)
                        .fontColor('#999999')
                }
                .alignItems(HorizontalAlign.Start)
                .padding(16)
                .backgroundColor('#2A2A2A')
                .borderRadius(8)
            }

            LoadingProgress()
                .color('#4CAF50')
                .width(48)
                .height(48)
                .margin({ top: 24 })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .backgroundColor('#1A1A1A')
        .padding(24)
    }
}
