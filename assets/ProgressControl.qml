import bb.cascades 1.0

ControlDelegate
{
    property variant asset
    delegateActive: false
    horizontalAlignment: HorizontalAlignment.Center
    verticalAlignment: VerticalAlignment.Center
    visible: delegateActive
    
    sourceComponent: ComponentDefinition
    {
        ImageView
        {
            horizontalAlignment: HorizontalAlignment.Center
            imageSource: asset
            loadEffect: ImageViewLoadEffect.FadeZoom
            
            animations: [
                RotateTransition {
                    id: rt
                    delay: 0
                    easingCurve: StockCurve.SineOut
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
    }
}