import bb.cascades 1.0

ControlDelegate
{
    horizontalAlignment: HorizontalAlignment.Center
    delegateActive: false
    property string cookieId
    
    function onProgressChanged(current, total) {
        delegateActive = current != total;
        
        if (delegateActive) {
            control.value = current;
            control.toValue = total;
        }
    }
    
    function onNetworkProgressChanged(cookie, current, total)
    {
        if (cookie == cookieId) {
            onProgressChanged(current,total);
        }
    }
    
    sourceComponent: ComponentDefinition
    {
        ProgressIndicator {
            fromValue: 0
            horizontalAlignment: HorizontalAlignment.Center
            state: ProgressIndicatorState.Progress
        }
    }
}