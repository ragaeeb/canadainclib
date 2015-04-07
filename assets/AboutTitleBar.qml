import bb.cascades 1.0

TitleBar
{
    id: titleControl
    property bool initialized: false
    property string videoTutorialUri
    property alias expandedContent: expandedContainer.controls
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
                id: appNameLabel
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.color: 'Signature' in ActionBarPlacement && Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? Color.Black : Color.White
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.BodyText
                
                onCreationCompleted: {
                    text = "%1 %2".arg(Application.applicationName).arg(Application.applicationVersion)
                }
            }
            
            Label
            {
                id: body
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.color: appNameLabel.textStyle.color
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
                horizontalAlignment: HorizontalAlignment.Fill
                leftPadding: 10; rightPadding: 10; topPadding: 10
                
                PersistCheckBox
                {
                    text: qsTr("Suppress Tutorials") + Retranslate.onLanguageChanged
                    key: "suppressTutorials"
                }
                
                Container
                {
                    id: expandedContainer
                    horizontalAlignment: HorizontalAlignment.Fill
                    visible: controls.length > 0
                    bottomPadding: 10
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
                
                if (!initialized)
                {
                    reporter.initPage(titleControl.parent);
                    titleControl.parent.addAction(channelAction);
                    
                    if (videoTutorialUri.length > 0) {
                        titleControl.parent.addAction(videoTutorialAction);
                    }
                    
                    initialized = true;
                }
            }
        }
    }
    
    attachedObjects: [
        ActionItem
        {
            id: channelAction
            imageSource: "images/menu/ic_channel.png"
            title: qsTr("Our BBM Channel") + Retranslate.onLanguageChanged
            ActionBar.placement: 'Signature' in ActionBarPlacement && videoTutorialUri.length == 0 ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
            
            onTriggered: {
                console.log("UserEvent: OpenChannel");
                persist.openChannel();
            }
        },
        
        ActionItem
        {
            id: videoTutorialAction
            imageSource: "images/menu/ic_video_tutorial.png"
            title: qsTr("Video Tutorial") + Retranslate.onLanguageChanged
            ActionBar.placement: 'Signature' in ActionBarPlacement && videoTutorialUri.length > 0 ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
            
            onTriggered: {
                console.log("UserEvent: VideoTutorial");
                persist.tutorialVideo(videoTutorialUri, false);
            }
        }
    ]
}