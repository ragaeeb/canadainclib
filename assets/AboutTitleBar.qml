import bb.cascades 1.0

TitleBar
{
    id: titleControl
    property bool initialized: false
    property string videoTutorialUri
    property alias expandedContent: expandedContainer.controls
    signal contentExpanded();
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
                textStyle.textAlign: TextAlign.Center
                textStyle.base: SystemDefaults.TextStyles.SmallText
                content.flags: TextContentFlag.ActiveText | TextContentFlag.EmoticonsOff
                multiline: true
                topMargin: 0
                
                onCreationCompleted: {
                    kindProperties.expandableArea.expandedChanged(kindProperties.expandableArea.expanded);
                    textStyle.color = appNameLabel.textStyle.color;
                }
            }
        }
        
        expandableArea
        {
            content: Container
            {
                horizontalAlignment: HorizontalAlignment.Fill
                leftPadding: 10; rightPadding: 10; topPadding: 10
                
                CheckBox
                {
                    checked: tutorial.suppressTutorials
                    text: qsTr("Suppress Tutorials") + Retranslate.onLanguageChanged
                    
                    onCheckedChanged: {
                        tutorial.suppressTutorials = checked;
                    }
                }
                
                CheckBox
                {
                    text: qsTr("Disable Update Checking") + Retranslate.onLanguageChanged
                    
                    onCheckedChanged: {
                        persist.setFlag("appLastUpdateCheck", new Date().getTime());
                    }
                    
                    onCreationCompleted: {
                        checked = persist.getFlag("appLastUpdateCheck") == -1;
                    }
                }
                
                Container
                {
                    id: expandedContainer
                    horizontalAlignment: HorizontalAlignment.Fill
                    bottomPadding: 10
                    
                    onCreationCompleted: {
                        visible = controls.length > 0;
                    }
                }
                
                Divider {
                    topMargin: 0;bottomMargin: 0
                }
            }
            
            onExpandedChanged: {
                if (expanded) {
                    body.text = qsTr("Please report all bugs to: support@canadainc.org");
                    contentExpanded();
                } else {
                    body.text = qsTr("(c) %1 %2. All Rights Reserved.").arg( new Date().getFullYear() ).arg(Application.organizationName);
                }

                if (!initialized)
                {
                    reporter.initPage(titleControl.parent);
                    titleControl.parent.addAction(channelAction);
                    
                    if (videoTutorialUri.length > 0) {
                        titleControl.parent.addAction(videoTutorialAction);
                        tutorial.execActionBar("openVideo", qsTr("To see a live tutorial and demo of how to use this app, please tap on the '%1' action.").arg(videoTutorialAction.title));
                    }
                    
                    tutorial.execActionBar("openChannel", qsTr("To stay up to date on the latest news on our apps or to reach out to us for support regarding any of our apps, tap on the '%1' action.").arg(channelAction.title), videoTutorialUri.length > 0 ? "r" : undefined);
                    
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
                reporter.record("OpenChannelTriggered");
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
                persist.openUri(videoTutorialUri);
                reporter.record("VideoTutorialTriggered");
            }
        }
    ]
}