import bb.cascades 1.0

MenuDefinition
{
    property string projectName
    property alias settings: settingsActionItem
    property alias help: helpActionItem
    property bool allowDonations: false
    property bool promoteChannel: false
    property string bbWorldID
    
    function launchPage(page)
    {
        if (parent.activePane) {
            parent.activePane.push(page);
        } else if (parent.activeTab) {
            parent.activeTab.content.push(page);
        } else {
            parent.push(page);
        }
    }
    
    onCreationCompleted: {
        if (allowDonations) {
            var donator = donateDefinition.createObject();
            addAction(donator);
        }
        
        if (bbWorldID.length > 0) {
            var reviewer = reviewDefinition.createObject();
            addAction(reviewer);
        }
    }
    
    settingsAction: SettingsActionItem
    {
        id: settingsActionItem
        property variant settingsPage
        
        onTriggered:
        {
            if (!settingsPage) {
                definition.source = "SettingsPage.qml"
                settingsPage = definition.createObject()
            }
            
            launchPage(settingsPage);
        }
    }
    
    actions: [
        ActionItem {
            property variant bugReportPage
            title: qsTr("Bug Reports") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            
            onTriggered: {
                if (!bugReportPage) {
                    definition.source = "BugReportPage.qml"
                    bugReportPage = definition.createObject()
                }
                
                bugReportPage.projectName = projectName;
                launchPage(bugReportPage);
            }
        }
    ]
    
    helpAction: HelpActionItem
    {
        id: helpActionItem
        property variant helpPage
        
        onTriggered:
        {
            if (!helpPage) {
                definition.source = "HelpPage.qml"
                helpPage = definition.createObject()
            }
            
            launchPage(helpPage);
        }
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: definition
        },
        
        ComponentDefinition
        {
            id: donateDefinition
            
            ActionItem
            {
                title: qsTr("Donate") + Retranslate.onLanguageChanged
                imageSource: "images/ic_donate.png"
                
                onTriggered: {
                    donator.trigger("bb.action.OPEN");
                }
                
                attachedObjects: [
                    Invocation {
                        id: donator
                        
                        query: InvokeQuery {
                            mimeType: "text/html"
                            uri: "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dar.as.sahaba@hotmail.com&currency_code=CAD&no_shipping=1&tax=0&lc=CA&bn=PP-DonationsBF&item_name=Da'wah Activities, Rent and Utility Expenses for the Musalla (please do not use credit cards)"
                        }
                    }
                ]
            }
        },
        
        ComponentDefinition
        {
            id: reviewDefinition
            
            ActionItem
            {
                title: qsTr("Review") + Retranslate.onLanguageChanged
                imageSource: "images/ic_review.png"
                
                onTriggered: {
                    reviewer.trigger("bb.action.OPEN");
                }
                
                attachedObjects: [
                    Invocation {
                        id: reviewer
                        
                        query {
                            mimeType: "text/html"
                            uri: "http://appworld.blackberry.com/webstore/content/"+bbWorldID
                        }
                    }
                ]
            }
        },
        
        Invocation
        {
            query: InvokeQuery {
                invokeTargetId: "sys.bbm.channels.card.previewer"
                uri: "bbmc:C0034D28B"
            }
            
            onArmed: {
                if ( promoteChannel && !persist.contains("promoted") ) {
                    trigger("bb.action.OPENBBMCHANNEL");
                    persist.saveValueFor("promoted", 1);
                }
            }
        }
    ]
}