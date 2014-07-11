import bb.cascades 1.0

Dialog
{
    id: root
    property bool canClose: true
    property alias dialogContent: dialogContainer.controls
    property alias display: displayInfo
    signal closing();
    
    onOpened: {
        dialogContainer.opacity = 1;
    }
    
    Container
    {
        id: dialogContainer
        preferredWidth: Infinity
        preferredHeight: Infinity
        background: Color.create(0.0, 0.0, 0.0, 0.5)
        layout: DockLayout {}
        opacity: 0
        
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    console.log("UserEvent: FullScreenDialog Tapped");
                    
                    if (event.propagationPhase == PropagationPhase.AtTarget && canClose)
                    {
                        closing();
                        root.close();
                    }
                }
            }
        ]
    }
    
    onClosed: {
        root.destroy();
    }
}