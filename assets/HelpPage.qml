import bb.cascades 1.0

Page
{
    id: helpPage
    property bool initialized: false
    property string videoTutorialUri
    property alias expandedContent: expandedContainer.controls
    property alias clearCacheImage: clearCacheAction.imageSource
    signal contentExpanded();
    signal clearCacheTriggered();
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    
    function cleanUp() {}
    
    onActionMenuVisualStateChanged: {
        if (actionMenuVisualState == ActionMenuVisualState.VisibleFull)
        {
            tutorial.execOverFlow( "clearCache", qsTr("If you notice the app taking up a lot of space, you should choose '%1' from the overflow menu."), clearCacheAction );
            reporter.record("HelpMenuShown");
        }
    }
    
    actions: [
        ActionItem
        {
            id: channelAction
            imageSource: "images/common/ic_channel.png"
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
            enabled: videoTutorialUri.length > 0
            imageSource: "images/common/ic_video_tutorial.png"
            title: qsTr("Video Tutorial") + Retranslate.onLanguageChanged
            ActionBar.placement: 'Signature' in ActionBarPlacement && videoTutorialUri.length > 0 ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
            
            onTriggered: {
                console.log("UserEvent: VideoTutorial");
                persist.openUri(videoTutorialUri);
                reporter.record("VideoTutorialTriggered");
            }
        },
        
        DeleteActionItem
        {
            id: clearCacheAction
            imageSource: "images/common/ic_clear_cache.png"
            title: qsTr("Clear Cache") + Retranslate.onLanguageChanged
            
            function onFinished(ok)
            {
                if (ok)
                {
                    reporter.record("ClearCacheConfirm");
                    persist.clearCache();
                    clearCacheTriggered();
                    persist.showToast( qsTr("Cache was successfully cleared!"), imageSource.toString() );
                }
            }
            
            onTriggered: {
                console.log("UserEvent: ClearCache");
                reporter.record("ClearCache");
                persist.showConfirmDialog( clearCacheAction, qsTr("Are you sure you want to clear the application cache?") );
            }
        }
    ]
    
    titleBar: TitleBar
    {
        id: titleControl
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
                        
                        contextActions: [
                            ActionSet {
                                title: qsTr("Tutorials") + Retranslate.onLanguageChanged
                                
                                DeleteActionItem
                                {
                                    id: resetTutorials
                                    title: qsTr("Reset") + Retranslate.onLanguageChanged
                                    
                                    onTriggered: {
                                        console.log("UserEvent: ResetTutorials");
                                        reporter.record("ResetTutorials");
                                        persist.resetTutorials();
                                        
                                        persist.showToast( qsTr("Tutorials reset!"), "images/common/ic_clear_cache.png" );
                                    }
                                }
                            }
                        ]
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
                }
            }
        }
    }
    
    onCreationCompleted: {
        if (videoTutorialAction.enabled) {
            tutorial.execActionBar("openVideo", qsTr("To see a live tutorial and demo of how to use this app, please tap on the '%1' action.").arg(videoTutorialAction.title));
        }
        
        tutorial.execActionBar("openChannel", qsTr("To stay up to date on the latest news on our apps or to reach out to us for support regarding any of our apps, tap on the '%1' action.").arg(channelAction.title), videoTutorialUri.length > 0 ? "r" : undefined);
        appNameLabel.text = "%1 %2".arg(Application.applicationName).arg(Application.applicationVersion)
        body.textStyle.color = appNameLabel.textStyle.color;
        titleControl.kindProperties.expandableArea.expandedChanged(titleControl.kindProperties.expandableArea.expanded);
    }
}