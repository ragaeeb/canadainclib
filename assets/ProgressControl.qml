import bb.cascades 1.0

ControlDelegate
{
    property variant asset
    property string loadingText
    property variant curve: StockCurve.SineOut
    delegateActive: false
    horizontalAlignment: HorizontalAlignment.Center
    verticalAlignment: VerticalAlignment.Center
    visible: delegateActive
    
    sourceComponent: ComponentDefinition
    {
        Container
        {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Center
            
            ImageView
            {
                horizontalAlignment: HorizontalAlignment.Center
                imageSource: asset
                loadEffect: ImageViewLoadEffect.FadeZoom
                scalingMethod: ScalingMethod.AspectFit
                
                animations: [
                    RotateTransition {
                        id: rt
                        delay: 0
                        easingCurve: curve
                        fromAngleZ: 0
                        toAngleZ: 360
                        duration: 1000
                        repeatCount: AnimationRepeatCount.Forever
                        
                        onCreationCompleted: {
                            play();
                        }
                    }
                ]
            }
            
            Label {
                text: loadingText
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                textStyle.fontSize: FontSize.XXSmall
                opacity: 0.7
            }
        }
    }
}