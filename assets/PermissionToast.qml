import bb.cascades 1.0

ControlDelegate
{
    delegateActive: false
    property variant messages
    property variant icons
    property real bottomSpacing: 30
    property real rightSpacing: 50
    property real leftSpacing: 15
    property real topSpacing: 10
    property variant labelColor: Color.White
    visible: delegateActive
    
    sourceComponent: ComponentDefinition
    {
        Container
        {
            id: mainContainer
            horizontalAlignment: HorizontalAlignment.Fill
            translationX: 500
            maxWidth: 500
            maxHeight: 500
            layout: DockLayout {}
            
            function showNext()
            {
                var allMessages = messages;
                var allIcons = icons;
                
                warning.text = allMessages.pop();
                infoImage.imageSource = allIcons.pop();
                
                messages = allMessages;
                icons = allIcons;
                
                rotator.play();
            }
            
            ImageView {
                imageSource: "images/toast/permission_toast_bg.amd"
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
            }
            
            ScrollView
            {
                Container
                {
                    id: permissionContainer
                    horizontalAlignment: HorizontalAlignment.Fill
                    topPadding: topSpacing; leftPadding: leftSpacing; rightPadding: rightSpacing; bottomPadding: bottomSpacing
                    
                    ImageView
                    {
                        id: infoImage
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        
                        animations: [
                            RotateTransition {
                                id: rotator
                                fromAngleZ: 0
                                toAngleZ: 360
                                delay: 750
                                duration: 1000
                                easingCurve: StockCurve.QuarticOut
                            }
                        ]
                    }
                    
                    Label
                    {
                        id: warning
                        topMargin: 0
                        textStyle.color: labelColor
                        multiline: true
                        verticalAlignment: VerticalAlignment.Fill
                        textStyle.fontSize: FontSize.XXSmall
                    }
                }
            }
            
            animations: [
                TranslateTransition {
                    id: tt
                    fromX: 500
                    toX: 0
                    easingCurve: StockCurve.SineIn
                    duration: 1000
                }
            ]
            
            onCreationCompleted: {
                tt.play();
                showNext();
                
                if ( "navigation" in permissionContainer ) {
                    var nav = permissionContainer.navigation;
                    nav.focusPolicy = 0x2;
                }
            }
            
            gestureHandlers: [
                TapHandler
                {
                    onTapped: {
                        console.log("UserEvent: WarningTapped");
                        var allMessages = messages;
                        
                        if (allMessages.length >= 1) {
                            mainContainer.showNext();
                        } else {
                            persist.launchAppPermissionSettings();
                            dth.doubleTapped(undefined);
                        }
                        
                        analytics.record("WarningTapped");
                    }
                },
                
                DoubleTapHandler
                {
                    id: dth
                    
                    onDoubleTapped: {
                        console.log("UserEvent: WarningDoubleTapped")
                        tt.fromX = 0;
                        tt.toX = 500;
                        tt.duration = 500;
                        tt.play();
                        
                        analytics.record("WarningDoubleTapped");
                    }
                }
            ]
        }
    }
}