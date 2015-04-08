import bb.cascades 1.0

CheckBox
{
    property bool isBool: false
    property string key
    property int value: 1
    signal valueChanged()

    function getValue()
    {
        var actual = persist.getValueFor(key);
        return isBool ? actual == true || actual == "true" : actual == value;
    }

    checked: getValue()
    
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
    
    function onSettingChanged(settingKey)
    {
        if (settingKey == key) {
            checked = getValue();
        }
    }
    
    onCreationCompleted: {
        persist.settingChanged.connect(onSettingChanged);
    }
}