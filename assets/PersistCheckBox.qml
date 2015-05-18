import bb.cascades 1.0

CheckBox
{
    id: checkBox
    property string key
    signal valueChanged()
    
    onCheckedChanged: {
        var changed = persist.saveValueFor(key, checked ? 1 : 0);
        
        if (changed) {
            valueChanged();
        }
    }
    
    function onSettingChanged(newValue) {
        checked = newValue == 1;
    }
    
    onCreationCompleted: {
        persist.registerForSetting(checkBox, key);
    }
}