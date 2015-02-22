import bb.cascades 1.0

DropDown
{
    property string key
    signal valueChanged(bool diff);
    horizontalAlignment: HorizontalAlignment.Fill
    
    onKeyChanged: {
        var primary = persist.getValueFor(key)
        
        for (var i = 0; i < options.length; i ++) {
            if (options[i].value == primary) {
                options[i].selected = true
                break;
            }
        }
    }
    
    onCreationCompleted: {
        if (key.length > 0) {
            keyChanged();
        }
    }
    
    onSelectedValueChanged: {
        if (key.length > 0)
        {
            var diff = persist.saveValueFor(key, selectedValue);
            valueChanged(diff);
        }
    }
}