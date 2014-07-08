import bb 1.0
import bb.cascades 1.0

TitleBar
{
    id: titleControl
    property string channelTitle: qsTr("Our BBM Channel") + Retranslate.onLanguageChanged
    kind: TitleBarKind.FreeForm
    scrollBehavior: TitleBarScrollBehavior.NonSticky
    kindProperties: FreeFormTitleBarKindProperties
    {
        id: ftbkp
        
        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            topPadding: 10
            
            Label {
                text: qsTr("%1 %2").arg(appInfo.title).arg(appInfo.version)
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.color: Color.White
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.BodyText
                
                attachedObjects: [
                    ApplicationInfo {
                        id: appInfo
                    }
                ]
            }
            
            Label
            {
                id: body
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.color: Color.White
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.SmallText
                content.flags: TextContentFlag.ActiveText | TextContentFlag.EmoticonsOff
                multiline: true
                
                attachedObjects: [
                    PackageInfo {
                        id: packageInfo
                    }
                ]
                
                onCreationCompleted: {
                    kindProperties.expandableArea.expandedChanged(kindProperties.expandableArea.expanded);
                }
            }
        }
        
        expandableArea
        {
            content: Container {}
            
            onExpandedChanged: {
                if (expanded) {
                    body.text = qsTr("Please report all bugs to: support@canadainc.org");
                } else {
                    body.text = qsTr("(c) %1 %2. All Rights Reserved.").arg( new Date().getFullYear() ).arg(packageInfo.author);
                }
            }
        }
    }
}