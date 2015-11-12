import bb.cascades 1.0

ControlDelegate
{
    delegateActive: !reporter.online
    signal imageTapped();
    visible: delegateActive
    property variant graphic: "images/common/ic_offline.png"
    
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
                imageSource: graphic
                scalingMethod: ScalingMethod.AspectFit
                loadEffect: ImageViewLoadEffect.FadeZoom
                
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            console.log("UserEvent: OfflineDelegateTapped");
                            imageTapped();
                            
                            reporter.record("OfflineDelegateTapped");
                        }
                    }
                ]
                
                onCreationCompleted: {
                    if ( "navigation" in imageView ) {
                        imageView.navigation.focusPolicy = 0x2;
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