import bb.cascades 1.0

ActionSet {
    id: actionSet
    
    ActionItem {
        title: qsTr("Copy") + Retranslate.onLanguageChanged
        imageSource: "images/ic_copy.png"
        onTriggered: {
            parent.parent.ListItem.view.copyItem(ListItemData)
        }
    }
    
    InvokeActionItem {
        id: iai
        title: qsTr("Share") + Retranslate.onLanguageChanged
        
        query {
            mimeType: "text/plain"
            invokeActionId: "bb.action.SHARE"
        }
        
        onTriggered: {
            iai.data = parent.parent.ListItem.view.shareItem(ListItemData)
        }
    }
}