import bb.cascades 1.0

ActionSet {
    id: actionSet
    
    ActionItem {
        title: qsTr("Copy") + Retranslate.onLanguageChanged
        imageSource: "images/menu/ic_copy.png"
        onTriggered: {
            console.log("UserEvent: PlainTextAction Copy");
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
            console.log("UserEvent: PlainTextAction Share");
            iai.data = parent.parent.ListItem.view.shareItem(ListItemData)
        }
    }
}