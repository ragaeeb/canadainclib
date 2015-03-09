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
        console.log(current, event.pinchRatio);
        var newValue = Math.floor(event.pinchRatio*current);
        console.log(newValue);
        newValue = Math.max(minValue, newValue);
        console.log(newValue);
        newValue = Math.min(newValue, maxValue);
        console.log(newValue);
        
        persist.saveValueFor(key, newValue);
    }
}