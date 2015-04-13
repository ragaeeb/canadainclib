import bb.cascades 1.4

DeviceShortcut
{
    id: ds
    property variant parentControl
    type: DeviceShortcuts.BackTap
    
    onParentControlChanged: {
        if (parentControl) {
            parentControl.addShortcut(ds);
        }
    }
}