import bb.cascades 1.0

Page
{
    id: root
    property string projectName
    property bool showServiceLogging: false
    property bool showSubmitLogs: false
    
    onShowServiceLoggingChanged: {
        if (showServiceLogging) {
            root.titleBar = serviceTitle.createObject();
        }
    }
    
    onShowSubmitLogsChanged: {
        if (showSubmitLogs) {
            root.addAction(submitLogs);
        } else {
            root.removeAction(submitLogs);
        }
    }
    
    onProjectNameChanged: {
        webView.url = "http://code.google.com/p/%1/issues/list".arg(projectName);
        browserAction.query.uri = "http://code.google.com/p/%1/issues/list".arg(projectName);
        browserAction.query.updateQuery();
    }
    
    titleBar: TitleBar
    {
        kind: TitleBarKind.FreeForm
        scrollBehavior: TitleBarScrollBehavior.NonSticky
        kindProperties: FreeFormTitleBarKindProperties
        {
            Container
            {
                leftPadding: 10; rightPadding: 10;
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                Label {
                    text: qsTr("UI Logging") + Retranslate.onLanguageChanged
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.color: Color.White
                    
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                }
                
                ToggleButton
                {
                    id: logUiToggle
                    verticalAlignment: VerticalAlignment.Center
                    checked: persist.getValueFor("logUI")
                    
                    onCheckedChanged: {
                        persist.saveValueFor("logUI", checked);
                    }
                }
            }
            
            expandableArea
            {
                expanded: true
                indicatorVisibility: showServiceLogging ? TitleBarExpandableAreaIndicatorVisibility.Visible : TitleBarExpandableAreaIndicatorVisibility.Hidden
                
                content: ControlDelegate
                {
                    delegateActive: showServiceLogging
                    
                    sourceComponent: ComponentDefinition
                    {
                        Container
                        {
                            background: Color.Black
                            leftPadding: 10; rightPadding: 80; bottomPadding: 10
                            
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            
                            Label {
                                text: qsTr("Service Logging") + Retranslate.onLanguageChanged
                                verticalAlignment: VerticalAlignment.Center
                                textStyle.color: Color.White
                                
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                            }
                            
                            ToggleButton
                            {
                                verticalAlignment: VerticalAlignment.Center
                                checked: persist.getValueFor("logService")
                                
                                onCheckedChanged: {
                                    persist.saveValueFor("logService", checked);
                                }
                            }
                        }
                    }
                }
            }
        }
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
        },
        
        ActionItem
        {
            id: submitLogs
            ActionBar.placement: ActionBarPlacement.OnBar
            title: qsTr("Submit Logs") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            enabled: logUiToggle.checked
            
            onTriggered: {
                enabled = false;
                console.log("UserEvent: SubmitLogs");
                reporter.submitLogs();
            }
            
            function onSubmitted(message) {
                persist.showBlockingToast( message, qsTr("OK"), "asset:///images/ic_bugs.png" );
                enabled = true;
            }
            
            onCreationCompleted: {
                reporter.submitted.connect(onSubmitted);
                
                if (!showSubmitLogs) {
                    root.removeAction(submitLogs);
                }
            }
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
}