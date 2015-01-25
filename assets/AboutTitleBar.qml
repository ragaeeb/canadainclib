import bb.cascades 1.0

TitleBar
{
    id: titleControl
    property string channelTitle: qsTr("Our BBM Channel") + Retranslate.onLanguageChanged
    property variant textColor: 'Signature' in ActionBarPlacement ? Color.Black : Color.White
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
                text: qsTr("%1 %2").arg(Application.applicationName).arg(Application.applicationVersion)
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.color: textColor
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.BodyText
            }
            
            Label
            {
                id: body
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.color: labelColor
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.SmallText
                content.flags: TextContentFlag.ActiveText | TextContentFlag.EmoticonsOff
                multiline: true
                topMargin: 0
                
                onCreationCompleted: {
                    kindProperties.expandableArea.expandedChanged(kindProperties.expandableArea.expanded);
                }
            }
        }
        
        expandableArea
        {
            content: Container
            {
                leftPadding: 10; rightPadding: 10; topPadding: 10
                
                PersistCheckBox
                {
                    text: qsTr("Suppress Tutorials") + Retranslate.onLanguageChanged
                    key: "suppressTutorials"
                }
                
                Divider {
                    topMargin: 0;bottomMargin: 0
                }
            }
            
            onExpandedChanged: {
                if (expanded) {
                    body.text = qsTr("Please report all bugs to: support@canadainc.org");
                } else {
                    body.text = qsTr("(c) %1 %2. All Rights Reserved.").arg( new Date().getFullYear() ).arg(Application.organizationName);
                }

                console.log("UserEvent: AboutTitleExpanded", expanded);
            }
        }
    }
}