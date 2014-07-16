import bb.cascades 1.0

CheckBox
{
    property bool isBool: false
    property string key
    property int value: 1

    checked: {
        var actual = persist.getValueFor(key);
        return isBool ? actual == true || actual == "true" : actual == value; 
    }
    
    onCheckedChanged: {
        if (isBool) {
            persist.saveValueFor(key, checked);
        } else {
            persist.saveValueFor(key, checked ? 1 : 0);
        }
    }
}