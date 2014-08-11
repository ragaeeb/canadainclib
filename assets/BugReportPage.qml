import bb.cascades 1.0
import bb.system 1.0

Page
{
    id: root
    property string projectName
    property bool showServiceLogging: false
    property bool showSubmitLogs: false
    property variant labelColor: Color.White
    
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
                
                Label {
                    text: qsTr("Bug Reports") + Retranslate.onLanguageChanged
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    textStyle.color: labelColor
                }
            }
            
            expandableArea
            {
                expanded: true
                indicatorVisibility: showServiceLogging ? TitleBarExpandableAreaIndicatorVisibility.Visible : TitleBarExpandableAreaIndicatorVisibility.Hidden
                
                onExpandedChanged: {                    
                    console.log("UserEvent: BugReportTitleExpanded", expanded);
                }
                
                content: Container
                {
                    leftPadding: 10; rightPadding: 10; topPadding: 10

                    PersistCheckBox
                    {
                        id: logUiToggle
                        isBool: true
                        text: qsTr("UI Logging") + Retranslate.onLanguageChanged
                        key: "logUI"
                    }
                    
                    ControlDelegate
                    {
                        delegateActive: showServiceLogging
                        
                        sourceComponent: ComponentDefinition
                        {
                            PersistCheckBox
                            {
                                isBool: true
                                text: qsTr("Service Logging") + Retranslate.onLanguageChanged
                                key: "logService"
                            }
                        }
                    }
                    
                    Divider {}
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
            
            onTriggered: {
                console.log("UserEvent: OpenInBrowser");
            }
        },
        
        ActionItem
        {
            id: submitLogs
            ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
            title: qsTr("Submit Logs") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            enabled: logUiToggle.checked
            
            onTriggered: {
                enabled = false;
                console.log("UserEvent: SubmitLogs");
                notesPrompt.show();
            }
            
            function onSubmitted(message)
            {
                progressIndicator.visible = false;
                persist.showBlockingToast( message, qsTr("OK"), "asset:///images/ic_bugs.png" );
                enabled = true;
            }
            
            onCreationCompleted: {
                reporter.submitted.connect(onSubmitted);
                
                if (!showSubmitLogs) {
                    root.removeAction(submitLogs);
                }
            }
            
            attachedObjects: [
                SystemPrompt {
                    id: notesPrompt
                    title: qsTr("Add Notes") + Retranslate.onLanguageChanged
                    body: qsTr("Enter the notes you wish to add:") + Retranslate.onLanguageChanged
                    inputField.emptyText: qsTr("Please start with your name") + Retranslate.onLanguageChanged
                    inputField.maximumLength: 0
                    confirmButton.label: qsTr("OK") + Retranslate.onLanguageChanged
                    cancelButton.label: qsTr("Cancel") + Retranslate.onLanguageChanged
                    
                    onFinished: {
                        console.log("UserEvent: AddNotesPrompt", result);
                        
                        if (result == SystemUiResult.ConfirmButtonSelection)
                        {
                            var value = inputFieldTextEntry().trim();
                            reporter.submitLogs(value, true);
                            progressIndicator.value = 0;
                            progressIndicator.state = ProgressIndicatorState.Progress;
                            progressIndicator.visible = true;
                        } else {
                            submitLogs.enabled = true;
                        }
                    }
                }
            ]
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
	    
	    ProgressIndicator
	    {
	        id: progressIndicator
	        horizontalAlignment: HorizontalAlignment.Center
	        verticalAlignment: VerticalAlignment.Center
	        value: 0
	        fromValue: 0
	        toValue: 100
	        opacity: value/100
	        state: ProgressIndicatorState.Pause
	        topMargin: 0; bottomMargin: 0; leftMargin: 0; rightMargin: 0;
	        
            function onNetworkProgressChanged(cookie, current, total)
            {
                value = current;
                toValue = total;
            }
	        
	        onCreationCompleted: {
	            reporter.progress.connect(onNetworkProgressChanged);
	        }
	    }
	}
}