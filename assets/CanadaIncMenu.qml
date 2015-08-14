import bb.cascades 1.0

MenuDefinition
{
    id: menuDef
    property bool allowDonations: false
    property string bbWorldID
    property string projectName
    property alias help: helpActionItem
    property alias settings: settingsActionItem
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
    
    function promptReview()
    {
        if ( reporter.deferredCheck("alreadyReviewed", 15, true) )
        {
            persist.showDialog( orientationHandler, {'cookie': 'review'}, qsTr("Review"), qsTr("If you enjoy the app, we would really appreciate if you left us a review so we can improve! It should only take a second. Would you like to leave one?"), qsTr("Yes"), qsTr("No") );
            return true;
        }
        
        return false;
    }
    
    function promptDonation()
    {
        if ( reporter.deferredCheck("alreadyDonated", 25, true) )
        {
            persist.showDialog( orientationHandler, {'cookie': 'donate'}, qsTr("Donate"), qsTr("While our apps will always remain free of charge for your benefit, we encourage you to please donate whatever you can in order to support development. This will motivate the developers to continue to update the app, add new features and bug fixes. To donate, simply swipe-down from the top-bezel and tap the 'Donate' button to send money via PayPal. Would you like to donate now?"), qsTr("Yes"), qsTr("No") );
            return true;
        }
        
        return false;
    }
    
    function asyncWork()
    {
        if (allowDonations) {
            var donator = donateDefinition.createObject();
            addAction(donator);
        }
        
        Application.swipeDown.connect(onSwipeDown);
        
        reporter.performCII();
        
        if ( reporter.deferredCheck("promoted", 1) ) {
            persist.openChannel();
            persist.setFlag("promoted", 1);
        } else if ( reporter.deferredCheck("appLastUpdateCheck", 2) ) {
            reporter.latestAppVersionFound.connect(onLatestVersionFound);
            reporter.checkForUpdate(projectName);
        } else if ( promptReview() ) {
        } else if ( allowDonations && promptDonation() ) {}
        
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
            
            var p = definition.init("SettingsPage.qml");
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

                var p = definition.init("BugReportPage.qml");
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
            
            var p = definition.init("HelpPage.qml");
            persist.registerForDestroyed(p, helpActionItem);
            launchPage(p);
            enabled = false;

            reporter.record("HelpPage");
        }
    }
    
    attachedObjects: [
        ComponentDefinition
        {
            id: definition
            
            function init(qml)
            {
                source = qml;
                return createObject();
            }
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
            id: orientationHandler
            
            function onFinished(confirmed, data)
            {
                if (data.cookie == "review") {
                    if (confirmed) {
                        persist.reviewApp();
                    }
                    
                    persist.setFlag("alreadyReviewed", Application.applicationVersion);
                } else if (data.cookie == "donate") {
                    if (confirmed) {
                        persist.donate();
                    }
                    
                    persist.setFlag("alreadyDonated", Application.applicationVersion);
                }
                
                reporter.record( "TutorialPromptResult", data.cookie+":"+confirmed.toString() );
            }
            
            onOrientationChanged: {
                reporter.record( "Orientation", orientation.toString() );
            }
        }
    ]
}