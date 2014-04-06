import bb.cascades 1.0

Page
{
    id: root
    property string projectName
    property bool showLogCollector: false
    property bool showSubmitLogs: false
    
    onShowLogCollectorChanged: {
        if (showLogCollector) {
            root.addAction(startLogging);
            root.addAction(stopLogging);
        } else {
            root.removeAction(startLogging);
            root.removeAction(stopLogging);
        }
    }
    
    onShowSubmitLogsChanged: {
        if (showSubmitLogs) {
            root.addAction(submitLogs);
        } else {
            root.removeAction(submitLogs);
        }
    }
    
    titleBar: TitleBar {
        title: qsTr("Bug Report") + Retranslate.onLanguageChanged
    }
    
    onProjectNameChanged: {
        webView.url = "http://code.google.com/p/%1/issues/list".arg(projectName);
        browserAction.query.uri = "http://code.google.com/p/%1/issues/list".arg(projectName);
        browserAction.query.updateQuery();
    }
    
    actions: [
        InvokeActionItem
        {
            id: browserAction
            ActionBar.placement: ActionBarPlacement.OnBar
            
            query {
                mimeType: "text/html"
                uri: "http://www.canadainc.org"
                invokeActionId: "bb.action.OPEN"
            }
            
            title: qsTr("Open in Browser") + Retranslate.onLanguageChanged
        }
    ]
    
	Container
	{
	    horizontalAlignment: HorizontalAlignment.Fill
	    verticalAlignment: VerticalAlignment.Fill
	    background: Color.White
	    layout: DockLayout {}
	    
	    ScrollView
	    {
	        id: scrollView
	        horizontalAlignment: HorizontalAlignment.Fill
	        verticalAlignment: VerticalAlignment.Fill
	        scrollViewProperties.scrollMode: ScrollMode.Both
	        scrollViewProperties.pinchToZoomEnabled: true
	        scrollViewProperties.initialScalingMethod: ScalingMethod.AspectFill
	        
	        WebView
	        {
	            id: webView
	            settings.zoomToFitEnabled: true
	            settings.activeTextEnabled: true
	            horizontalAlignment: HorizontalAlignment.Fill
	            verticalAlignment: VerticalAlignment.Fill
	            
	            onLoadProgressChanged: {
	                progressIndicator.value = loadProgress;
	            }
	            
	            onLoadingChanged: {
	                if (loadRequest.status == WebLoadStatus.Started) {
	                    progressIndicator.visible = true;
	                    progressIndicator.state = ProgressIndicatorState.Progress;
	                } else if (loadRequest.status == WebLoadStatus.Succeeded) {
	                    progressIndicator.visible = false;
	                    progressIndicator.state = ProgressIndicatorState.Complete;
	                } else if (loadRequest.status == WebLoadStatus.Failed) {
	                    html = "<html><head><title>Load Fail</title><style>* { margin: 0px; padding 0px; }body { font-size: 48px; font-family: monospace; border: 1px solid #444; padding: 4px; }</style> </head> <body>Loading failed! Please check your internet connection.</body></html>"
	                    progressIndicator.visible = false;
	                    progressIndicator.state = ProgressIndicatorState.Error;
	                }
	            }
	        }
	    }
	    
	    ProgressIndicator {
	        id: progressIndicator
	        horizontalAlignment: HorizontalAlignment.Center
	        verticalAlignment: VerticalAlignment.Top
	        visible: true
	        value: 0
	        fromValue: 0
	        toValue: 100
	        state: ProgressIndicatorState.Pause
	        topMargin: 0; bottomMargin: 0; leftMargin: 0; rightMargin: 0;
	    }
	}
	
	attachedObjects: [
        ActionItem
        {
            id: submitLogs
            title: qsTr("Submit Logs") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            
            onTriggered: {
                console.log("UserEvent: SubmitLogs");
                reporter.submitLogs();
            }
        },
        
        ActionItem
        {
            id: startLogging
            title: qsTr("Start Logging") + Retranslate.onLanguageChanged
            imageSource: "file:///usr/share/icons/bb_action_install.png"
            
            onTriggered: {
                console.log("UserEvent: StartLogging");
                persist.saveValueFor("startLogging", 1);
                persist.showToast( qsTr("Diagnostic Logging Started"), "", "asset:///images/ic_review.png" );
            }
        },
        
        DeleteActionItem
        {
            id: stopLogging
            title: qsTr("Stop Logging") + Retranslate.onLanguageChanged
            
            onTriggered: {
                console.log("UserEvent: StopLogging");
                persist.saveValueFor("stopLogging", 1);
                persist.showToast( qsTr("Diagnostic Logging Stopped"), "", "file:///usr/share/icons/bb_action_delete.png" );
            }
        }
	]
}