import bb.cascades 1.0

MenuDefinition
{
    id: menuDef
    property bool allowDonations: false
    property string bbWorldID
    property string projectName
    property alias help: helpActionItem
    property alias settings: settingsActionItem
    
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
    
    function onFinished(confirm, remember)
    {
        if (confirm) {
            persist.openBlackBerryWorld(bbWorldID);
        }
        
        if (remember) {
            persist.setFlag("appLastUpdateCheck", -1);
        } else {
            persist.setFlag("appLastUpdateCheck", new Date().getTime());
        }
        
        analytics.record( "PerformAppUpdate", confirm.toString() );
    }
    
    function onLatestVersionFound(latestVersion)
    {
        var currentVersion = Application.applicationVersion;
        var isOlder = currentVersion.localeCompare(latestVersion) < 0;
        console.log("latestVersionFound", latestVersion, currentVersion, isOlder);

        if (isOlder && !persist.isBlocked) {// if it's an older client, and we are not blocked
            persist.showDialog( menuDef, qsTr("Update Available"), qsTr("%1 %2 is available (you have %3). Would you like to visit BlackBerry World to download the latest version?").arg(Application.applicationName).arg(latestVersion).arg(currentVersion), qsTr("Yes"), qsTr("No"), qsTr("Don't Show Again") );
        } else if (!isOlder) { // if it's a newer client, then don't check for a while
            persist.setFlag("appLastUpdateCheck", new Date().getTime() );
        }
    }
    
    function asyncWork()
    {
        if ( !persist.getFlag("promoted") ) {
            persist.openChannel();
            persist.setFlag("promoted", 1);
        }

        if (allowDonations) {
            var donator = donateDefinition.createObject();
            addAction(donator);
        }
        
        if ( persist.isUpdateNeeded("appLastUpdateCheck") ) // 30 days have passed since last update check
        {
            reporter.latestAppVersionFound.connect(onLatestVersionFound);
            reporter.checkForUpdate(projectName);
        }
    }
    
    onCreationCompleted: {
        app.lazyInitComplete.connect(asyncWork);
    }
    
    settingsAction: SettingsActionItem
    {
        id: settingsActionItem
        property variant settingsPage
        
        onTriggered:
        {
            console.log("UserEvent: SettingsPage");
            
            if (!settingsPage) {
                definition.source = "SettingsPage.qml"
                settingsPage = definition.createObject()
            }
            
            launchPage(settingsPage);
            
            analytics.record("SettingsPage");
        }
    }
    
    actions: [
        ActionItem {
            property variant bugReportPage
            title: qsTr("Bug Reports") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            
            onTriggered: {
                console.log("UserEvent: BugReportPage");
                
                if (!bugReportPage) {
                    definition.source = "BugReportPage.qml"
                    bugReportPage = definition.createObject()
                }
                
                bugReportPage.projectName = projectName;
                launchPage(bugReportPage);
                
                analytics.record("BugReportPage");
            }
        },
        
        ActionItem
        {
            title: qsTr("Review") + Retranslate.onLanguageChanged
            imageSource: "images/ic_review.png"
            
            onTriggered: {
                console.log("UserEvent: ReviewApp");
                persist.reviewApp();
                
                analytics.record("ReviewApp");
            }
        }
    ]
    
    helpAction: HelpActionItem
    {
        id: helpActionItem
        property variant helpPage
        
        onTriggered:
        {
            console.log("UserEvent: HelpPage");
            
            if (!helpPage) {
                definition.source = "HelpPage.qml"
                helpPage = definition.createObject()
            }
            
            launchPage(helpPage);
            
            analytics.record("HelpPage");
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
                    console.log("UserEvent: Donate");
                    persist.openUri("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dar.as.sahaba@hotmail.com&currency_code=CAD&no_shipping=1&tax=0&lc=CA&bn=PP-DonationsBF&item_name=Da'wah Activities, Rent and Utility Expenses for the Musalla (please do not use credit cards)");
                    
                    analytics.record("DonateTriggered");
                }
            }
        }
    ]
}