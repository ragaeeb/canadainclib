import bb.cascades 1.0

TitleBar
{
    property alias backgroundImage: bgImageView.imageSource
    property alias logoImage: logoView.imageSource
    property alias titleImage: titleText.imageSource
    
    kind: TitleBarKind.FreeForm
    kindProperties: FreeFormTitleBarKindProperties
    {
        Container
        {
            id: titleBar
            layout: DockLayout {}
            
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Top
            
            ImageView {
                id: bgImageView
                imageSource: "images/title_bg.png"
                topMargin: 0
                leftMargin: 0
                rightMargin: 0
                bottomMargin: 0
                
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top
            }
            
            Container
            {
                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Center
                leftPadding: 20
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                ImageView {
                    id: logoView
                    imageSource: "images/logo.png"
                    topMargin: 0
                    leftMargin: 0
                    rightMargin: 0
                    bottomMargin: 0
                    
                    horizontalAlignment: HorizontalAlignment.Left
                    verticalAlignment: VerticalAlignment.Center
                }
                
                ImageView {
                    id: titleText
                    imageSource: "images/title_text.png"
                    topMargin: 0
                    leftMargin: 0
                    rightMargin: 0
                    bottomMargin: 0
                    
                    horizontalAlignment: HorizontalAlignment.Left
                    verticalAlignment: VerticalAlignment.Center
                }
                
                animations: [
                    ParallelAnimation {
                        id: translateFade
                        
                        FadeTransition {
                            easingCurve: StockCurve.CubicIn
                            fromOpacity: 0
                            toOpacity: 1
                            duration: 1000
                        }
                        
                        TranslateTransition {
                            toX: 0
                            fromX: -300
                            duration: 1000
                        }
                    }
                ]
                
                onCreationCompleted: {
                    translateFade.play();
                }
            }
        }
    }
}