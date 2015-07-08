import bb.cascades 1.0

MenuDefinition
{
    id: menuDef
    property bool allowDonations: false
    property string bbWorldID
    property string projectName
    property alias help: helpActionItem
    property alias settings: settingsActionItem
    property alias compDef: definition
    signal finished()
    
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
        
        reporter.record( "PerformAppUpdate", confirm.toString() );
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
    
    function onSwipeDown() {
        reporter.record("SwipeDown");
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
        
        Application.swipeDown.connect(onSwipeDown);
        
        var now = new Date().getTime();
        reporter.record("AppLaunch", now);
        
        if ( !persist.containsFlag("firstInstall") ) {
            persist.setFlag("firstInstall", now);
        }
        
        finished();
    }
    
    onCreationCompleted: {
        app.lazyInitComplete.connect(asyncWork);
    }
    
    settingsAction: SettingsActionItem
    {
        id: settingsActionItem
        
        function onDestroyed() {
            enabled = true;
        }
        
        onTriggered:
        {
            console.log("UserEvent: SettingsPage");
            
            definition.source = "SettingsPage.qml"
            var p = definition.createObject();
            persist.registerForDestroyed(p, settingsActionItem);
            enabled = false;
            launchPage(p);
            
            reporter.record("SettingsPage");
        }
    }
    
    actions: [
        ActionItem
        {
            id: bugReportActionItem
            title: qsTr("Bug Reports") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            
            function onDestroyed() {
                enabled = true;
            }
            
            onTriggered: {
                console.log("UserEvent: BugReportPage");

                definition.source = "BugReportPage.qml"
                var p = definition.createObject();
                p.projectName = projectName;
                persist.registerForDestroyed(p, bugReportActionItem);
                launchPage(p);
                enabled = false;
                
                reporter.record("BugReportPage");
            }
        },
        
        ActionItem
        {
            title: qsTr("Review") + Retranslate.onLanguageChanged
            imageSource: "images/ic_review.png"
            
            onTriggered: {
                console.log("UserEvent: ReviewApp");
                persist.reviewApp();
                
                reporter.record("ReviewApp");
            }
        }
    ]
    
    helpAction: HelpActionItem
    {
        id: helpActionItem
        
        function onDestroyed() {
            enabled = true;
        }
        
        onTriggered:
        {
            console.log("UserEvent: HelpPage");
            
            definition.source = "HelpPage.qml"
            var p = definition.createObject();
            persist.registerForDestroyed(p, helpActionItem);
            launchPage(p);
            enabled = false;

            reporter.record("HelpPage");
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
                    
                    reporter.record("DonateTriggered");
                }
            }
        },
        
        OrientationHandler
        {
            onOrientationChanged: {
                reporter.record( "Orientation", orientation.toString() );
            }
        }
    ]
}