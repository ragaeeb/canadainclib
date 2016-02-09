import bb.cascades 1.0

ControlDelegate
{
    delegateActive: !reporter.online
    visible: delegateActive
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Center
    
    sourceComponent: ComponentDefinition
    {
        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            leftPadding: 80
            rightPadding: 80
            
            ImageView
            {
                id: imageView
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                imageSource: "images/common/ic_offline.png"
                scalingMethod: ScalingMethod.AspectFit
                loadEffect: ImageViewLoadEffect.FadeZoom
                
                gestureHandlers: [
                    TapHandler
                    {
                        onTapped: {
                            console.log("UserEvent: OfflineDelegateTapped");
                            persist.launchSettingsApp("networkconnections");
                            reporter.record("OfflineDelegateTapped");
                        }
                    }
                ]
                
                onCreationCompleted: {
                    if ( "navigation" in imageView )
                    {
                        var ivNav = imageView.navigation;
                        ivNav.focusPolicy = 0x2;
                    }
                }
            }
            
            Label {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                multiline: true
                textStyle.fontSize: FontSize.Small
                textStyle.textAlign: TextAlign.Center
                text: qsTr("You are currently offline. Please check your network connection.") + Retranslate.onLanguageChanged
            }
        }
    }
}