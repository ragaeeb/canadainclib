import bb.cascades 1.0

Sheet
{
    id: root
    property alias uri: webView.url
    property alias title: tb.title
    property string key
    property string twitter
    property string name
    property string email
    property string pin
    property string instagram
    property string facebook
    
    Page
    {
        actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll

        actions: [
            ActionItem
            {
                enabled: twitter.length > 0
                ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
                imageSource: "images/common/list/site_twitter.png"
                title: qsTr("Twitter") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    console.log("UserEvent: "+name+"Twitter");
                    persist.openUri(twitter);
                    
                    reporter.record(name+"Twitter");
                }
            },

            ActionItem
            {
                enabled: facebook.length > 0
                ActionBar.placement: ActionBarPlacement.OnBar
                imageSource: "images/common/list/site_facebook.png"
                title: qsTr("Facebook") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    console.log("UserEvent: "+name+"Facebook");
                    persist.openUri(facebook);
                    
                    reporter.record(name+"Facebook");
                }
            },
            
            ActionItem
            {
                enabled: email.length > 0
                ActionBar.placement: ActionBarPlacement.OnBar
                imageSource: "images/common/list/ic_email.png"
                title: qsTr("Email") + Retranslate.onLanguageChanged

                onTriggered: {
                    console.log("UserEvent: "+name+"Email");
                    persist.invoke("", "", "", "mailto:"+email);
                    
                    reporter.record(name+"Email");
                }
            },
            
            ActionItem
            {
                enabled: instagram.length > 0
                imageSource: "images/common/list/ic_instagram.png"
                title: qsTr("Instagram") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    console.log("UserEvent: "+name+"IG");
                    persist.openUri(instagram);
                    
                    reporter.record(name+"IG");
                }
            }
        ]
        
        titleBar: TitleBar
        {
            id: tb
            
            acceptAction: ActionItem
            {
                enabled: pin.length > 0
                imageSource: "file:///usr/share/icons/ic_start_bbm_chat.png"
                title: qsTr("BBM") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    console.log("UserEvent: "+name+"BBM");
                    persist.invoke("", "", "", "pin:"+pin);

                    reporter.record(name+"BBM");
                }
            }

            dismissAction: ActionItem
            {
                title: qsTr("Back") + Retranslate.onLanguageChanged
                imageSource: "images/common/ic_prev.png"

                onTriggered: {
                    console.log("UserEvent: "+name+"Back");
                    persist.setFlag(key, 1);
                    
                    reporter.record(name+"Back");
                    root.close();
                }
            }
        }

        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            background: Color.White
            layout: DockLayout {}

            ScrollView
            {
                id: scrollView
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                scrollViewProperties.scrollMode: ScrollMode.Both
                scrollViewProperties.pinchToZoomEnabled: true
                scrollViewProperties.initialScalingMethod: ScalingMethod.AspectFill

                WebView
                {
                    id: webView
                    settings.zoomToFitEnabled: true
                    settings.activeTextEnabled: false
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    onLoadProgressChanged: {
                        progressIndicator.value = loadProgress;
                    }

                    onLoadingChanged: {
                        if (loadRequest.status == WebLoadStatus.Started) {
                            progressIndicator.visible = true;
                            progressIndicator.state = ProgressIndicatorState.Progress;
                        } else if (loadRequest.status == WebLoadStatus.Succeeded) {
                            progressIndicator.visible = false;
                            progressIndicator.state = ProgressIndicatorState.Complete;
                        } else if (loadRequest.status == WebLoadStatus.Failed) {
                            root.close();
                        }
                    }
                }
            }

            ProgressIndicator
            {
                id: progressIndicator
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                value: 0
                fromValue: 0
                toValue: 100
                opacity: value/100
                state: ProgressIndicatorState.Pause
            }
        }
    }
    
    onClosed: {
        destroy();
    }
}