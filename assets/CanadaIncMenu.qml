import bb.cascades 1.0

MenuDefinition
{
    id: menuDef
    property bool allowDonations: false
    property string bbWorldID
    property string projectName
    property alias help: helpActionItem
    property alias settings: settingsActionItem
    property string helpPageQml: "HelpPage.qml"
    property int analyticDiffDays: 30
    signal finished(bool clean, int analyticResult)
    
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

        if (isOlder) {// if it's an older client
            persist.downloadApp(bbWorldID);
        }
    }
    
    function onSwipeDown() {
        reporter.record("SwipeDown");
    }
    
    function asyncWork()
    {
        Application.swipeDown.connect(onSwipeDown);
        reporter.latestAppVersionFound.connect(onLatestVersionFound);
        
        if (allowDonations) {
            addAction( donateDefinition.createObject() );
        }
        
        var analyticResult = reporter.performCII(analyticDiffDays);
        var clean = false;
        
        var numLaunches = persist.getFlag("launchCount");
        
        if (!numLaunches || analyticResult == 0 || numLaunches == 600) {
            numLaunches = 0;
        }
        
        ++numLaunches;
        
        if ( numLaunches == 5 && persist.getFlag("alreadyReviewed") != Application.applicationVersion ) {
            persist.showDialog( menuDef, {'cookie': 'review'}, qsTr("Review"), qsTr("If you enjoy the app, we would really appreciate if you left us a review so we can improve! It should only take a second. Would you like to leave one?"), qsTr("Yes"), qsTr("No") );
        } else if ( (allowDonations && numLaunches == 10) || (allowDonations && numLaunches == 400) ) {
            persist.showDialog( menuDef, {'cookie': 'donate'}, qsTr("Donate"), qsTr("While our apps will always remain free of charge for your benefit, we encourage you to please donate whatever you can in order to support development. This will motivate the developers to continue to update the app, add new features and bug fixes. To donate, simply swipe-down from the top-bezel and tap the 'Donate' button to send money via PayPal. Would you like to donate now?"), qsTr("Yes"), qsTr("No") );
        } else if ( reporter.deferredCheck("channelPromoted", 1) ) {
            persist.openChannel();
            persist.setFlag("channelPromoted", 1);
        } else {
            clean = true;
        }
        
        persist.setFlag("launchCount", numLaunches);
        
        finished(clean, analyticResult);
    }
    
    function onFinished(confirmed, data)
    {
        if (data.cookie == "review") {
            if (confirmed) {
                persist.reviewApp();
            }
            
            persist.setFlag("alreadyReviewed", Application.applicationVersion);
        } else if (data.cookie == "donate" && confirmed) {
            persist.donate();
        }
        
        reporter.record( "TutorialPromptResult", data.cookie+":"+confirmed.toString() );
    }
    
    function launch(qml)
    {
        var page = initQml(qml);
        launchPage(page);
        
        return page;
    }
    
    function initQml(qml, params)
    {
        definition.source = qml;
        var x = definition.createObject();
        
        return x;
    }
    
    onCreationCompleted: {
        app.lazyInitComplete.connect(asyncWork);
        Qt.launch = launch;
        Qt.initQml = initQml;
    }
    
    settingsAction: SettingsActionItem
    {
        id: settingsActionItem
        imageSource: "images/common/ic_settings.png"
        title: qsTr("Settings") + Retranslate.onLanguageChanged
        
        function onDestroyed() {
            enabled = true;
        }
        
        onTriggered:
        {
            console.log("UserEvent: SettingsPage");
            
            var p = launch("SettingsPage.qml");
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
            imageSource: "images/common/bugs/ic_bugs.png"
            
            function onDestroyed() {
                enabled = true;
            }
            
            onTriggered: {
                console.log("UserEvent: BugReportPage");

                var p = launch("BugReportPage.qml");
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
            imageSource: "images/common/ic_review.png"
            
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
        imageSource: "images/common/ic_help.png"
        title: qsTr("Help") + Retranslate.onLanguageChanged
        
        function onDestroyed() {
            enabled = true;
        }
        
        onTriggered:
        {
            console.log("UserEvent: HelpPage");
            
            var p = launch(helpPageQml);
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
                imageSource: "images/common/ic_donate.png"
                
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
            
            onOrientationChanged: {
                reporter.record( "Orientation", orientation.toString() );
            }
        }
    ]
}