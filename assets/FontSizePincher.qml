import bb.cascades 1.0

PinchHandler
{
    property string key
    property string userEventId
    property int minValue
    property int maxValue
    
    onPinchEnded: {
        console.log("UserEvent: "+userEventId);
        var current = persist.getValueFor(key);
        
        if ( isNaN(current) || current <= 0 ) {
            current = (maxValue+minValue)/2;
        }
        
        var newValue = Math.floor(event.pinchRatio*current);
        newValue = Math.max(minValue, newValue);
        newValue = Math.min(newValue, maxValue);
        persist.saveValueFor(key, newValue);
        
        reporter.record(userEventId);
    }
}