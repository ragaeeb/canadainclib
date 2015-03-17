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
        var newValue = Math.floor(event.pinchRatio*current);
        newValue = Math.max(minValue, newValue);
        newValue = Math.min(newValue, maxValue);
        
        persist.saveValueFor(key, newValue);
    }
}