import bb.cascades 1.2

CheckBox
{
    property string key
    property int value: 1
    checked: persist.getValueFor(key) == value;
    
    onCheckedChanged: {
        persist.saveValueFor(key, checked ? 1 : 0);
    }
}