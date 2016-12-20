import bb.cascades 1.2

Tab
{
    property string qml
    delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
    newContentAvailable: unreadContentCount > 0
    
    function getParams() {
        return undefined;
    }
    
    delegate: Delegate {
        sourceComponent: navDef
    }
    
    attachedObjects: [
        ComponentDefinition
        {
            id: navDef
            
            NavigationPane
            {
                onPopTransitionEnded: {
                    page.destroy();
                }
                
                onCreationCompleted: {
                    var x = Qt.initQml( qml, getParams() );
                    insert(0, x);
                }
            }
        }
    ]
    
    onTriggered: {
        console.log("UserEvent: "+title)
    }
}