import bb.cascades 1.0

ControlDelegate
{
    property variant graphic
    property string labelText
    delegateActive: false
    signal imageTapped();
    
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
                            imageTapped();
                        }
                    }
                ]
                
                onCreationCompleted: {
                    if ( "navigation" in imageView ) {
                        var nav = imageView.navigation;
                        nav.focusPolicy = 0x2;
                    }
                }
            }
            
            Label {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                multiline: true
                textStyle.fontSize: FontSize.Large
                textStyle.textAlign: TextAlign.Center
                text: labelText
            }
        }
    }
}