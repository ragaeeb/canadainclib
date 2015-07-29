import bb.cascades 1.2

Dialog
{
    id: root
    property bool canClose: true
    property alias mainContainer: dialogContainer
    property alias dialogContent: dialogContainer.controls
    signal closing();
    
    onOpened: {
        dialogContainer.opacity = 1;
    }
    
    function dismiss()
    {
        closing();
        root.close();
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
                    if (event.propagationPhase == PropagationPhase.AtTarget && canClose) {
                        dismiss();
                    }
                }
            }
        ]
        
        attachedObjects: [
            Delegate {
                source: "ClassicBackDelegate.qml"
                
                onCreationCompleted: {
                    active = 'locallyFocused' in dialogContainer && canClose;
                }
                
                onObjectChanged: {
                    if (object) {
                        object.parentControl = dialogContainer;
                        object.triggered.connect(root.dismiss);
                    }
                }
            }
        ]
    }
    
    onClosed: {
        root.destroy();
    }
}