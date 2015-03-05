import bb.cascades 1.0

CheckBox
{
    property bool isBool: false
    property string key
    property int value: 1
    signal valueChanged()

    checked: {
        var actual = persist.getValueFor(key);
        return isBool ? actual == true || actual == "true" : actual == value; 
    }
    
    onCheckedChanged: {
        var changed = false;
        
        if (isBool) {
            changed = persist.saveValueFor(key, checked);
        } else {
            changed = persist.saveValueFor(key, checked ? 1 : 0);
        }
        
        if (changed) {
            valueChanged();
        }
    }
}