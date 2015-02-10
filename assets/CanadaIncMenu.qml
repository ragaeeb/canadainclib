import bb.cascades 1.0

MenuDefinition
{
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
    
    function onLatestVersionFound(latestVersion)
    {
        var currentVersion = Application.applicationVersion;
        var isOlder = currentVersion.localeCompare(latestVersion) < 0;
        console.log("latestVersionFound", latestVersion, currentVersion, isOlder);

        if (isOlder && !persist.isBlocked) // if it's an older client, and we are not blocked
        {
            var result = persist.showBlockingDialogWithRemember( qsTr("Update Available"), qsTr("%1 %2 is available (you have %3). Would you like to visit BlackBerry World to download the latest version?").arg(Application.applicationName).arg(latestVersion).arg(currentVersion), qsTr("Don't Show Again") );
            
            if (result[0]) {
                persist.openBlackBerryWorld(bbWorldID);
            }
            
            if (result[1]) {
                persist.saveValueFor("appLastUpdateCheck", -1);
            } else {
                persist.saveValueFor("appLastUpdateCheck", new Date().getTime());
            }
        } else if (!isOlder) { // if it's a newer client, then don't check for a while
            persist.saveValueFor("appLastUpdateCheck", new Date().getTime());
        }
    }
    
    function asyncWork()
    {
        persist.openChannel(true);
        
        if (allowDonations) {
            var donator = donateDefinition.createObject();
            addAction(donator);
        }
        
        var lastUpdateCheck = persist.contains("appLastUpdateCheck") ? persist.getValueFor("appLastUpdateCheck") : 0;
        var diff = new Date().getTime() - lastUpdateCheck;
        
        if (lastUpdateCheck >= 0 && diff > 1000*60*60*24*30) { // 30 days have past since last update check
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
            }
        },
        
        ActionItem
        {
            title: qsTr("Review") + Retranslate.onLanguageChanged
            imageSource: "images/ic_review.png"
            
            onTriggered: {
                console.log("UserEvent: ReviewApp");
                persist.reviewApp();
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
                    persist.donate();
                }
            }
        }
    ]
}