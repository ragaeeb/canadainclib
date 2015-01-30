import bb.cascades 1.2

Page
{
    id: root
    property string projectName
    property bool showServiceLogging: false
    
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
                leftPadding: 10; rightPadding: 10; topPadding: 10
                
                Label {
                    text: qsTr("Bug Reports") + Retranslate.onLanguageChanged
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    textStyle.color: 'Signature' in ActionBarPlacement && Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? Color.Black : Color.White
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
                    
                    ImageView {
                        imageSource: "images/bugs/ic_bugs_divider.png"
                        horizontalAlignment: HorizontalAlignment.Center
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
                sheetDelegate.active = true;
            }
            
            function onSubmitted(message)
            {
                progressIndicator.visible = false;
                persist.showBlockingDialog( qsTr("Submission Status"), message, qsTr("OK"), "" );
                enabled = true;
            }
            
            onCreationCompleted: {
                reporter.submitted.connect(onSubmitted);
            }
            
            attachedObjects: [
                Delegate
                {
                    id: sheetDelegate
                    
                    sourceComponent: ComponentDefinition
                    {
                        Sheet
                        {
                            id: sheet
                            
                            onCreationCompleted: {
                                open();
                            }
                            
                            onClosed: {
                                sheetDelegate.active = false;
                            }
                            
                            onOpened: {
                                persist.showToast( qsTr("Enter the notes you wish to add.\n\nPlease include as much detail as possible about the issue you are having and how to reproduce it."), "", "asset:///images/bugs/ic_bugs_info.png" );
                                body.editor.cursorPosition = 6; // right after the name field
                                body.requestFocus();
                                
                                anim.play();
                            }
                            
                            Page
                            {
                                titleBar: TitleBar
                                {
                                    title: qsTr("Add Notes") + Retranslate.onLanguageChanged
                                    
                                    acceptAction: ActionItem
                                    {
                                        id: submit
                                        imageSource: "images/bugs/ic_bugs_submit.png"
                                        title: qsTr("Submit") + Retranslate.onLanguageChanged
                                        
                                        onTriggered: {
                                            reporter.submitLogs(body.text, true, includeScreenshot.checked);
                                            progressIndicator.value = 0;
                                            progressIndicator.state = ProgressIndicatorState.Progress;
                                            progressIndicator.visible = true;
                                            sheet.close();
                                        }
                                    }
                                    
                                    dismissAction: ActionItem
                                    {
                                        imageSource: "images/bugs/ic_bugs_cancel.png"
                                        title: qsTr("Cancel") + Retranslate.onLanguageChanged
                                        
                                        onTriggered: {
                                            submitLogs.enabled = true;
                                            sheet.close();
                                        }
                                    }
                                }
                                
                                Container
                                {
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                    
                                    animations: [
                                        SequentialAnimation
                                        {
                                            id: anim
                                            
                                            ParallelAnimation
                                            {
                                                RotateTransition
                                                {
                                                    target: includeScreenshot
                                                    delay: 500
                                                    fromAngleZ: 0
                                                    toAngleZ: 360
                                                    easingCurve: StockCurve.ElasticOut
                                                    duration: 1000
                                                }
                                                
                                                TranslateTransition
                                                {
                                                    target: includeContainer
                                                    delay: 250
                                                    fromX: 500
                                                    toX: 0
                                                    easingCurve: StockCurve.BackOut
                                                    duration: 1000
                                                }
                                            }
                                            
                                            TranslateTransition
                                            {
                                                target: body
                                                fromY: 500
                                                toY: 0
                                                easingCurve: StockCurve.SineOut
                                                duration: 500
                                            }
                                        }
                                    ]
                                    
                                    Container
                                    {
                                        id: includeContainer
                                        topPadding: 10; bottomPadding: 10; leftPadding: 20; rightPadding: 20
                                        translationX: 500
                                        
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        
                                        Label {
                                            id: includeLabel
                                            text: qsTr("Include Most Recent Captured Screenshot") + Retranslate.onLanguageChanged
                                            verticalAlignment: VerticalAlignment.Center
                                            
                                            layoutProperties: StackLayoutProperties {
                                                spaceQuota: 1
                                            }
                                            
                                            gestureHandlers: [
                                                TapHandler {
                                                    onTapped: {
                                                        reporter.previewLastCapturedPic();
                                                    }
                                                }
                                            ]
                                        }
                                        
                                        ToggleButton
                                        {
                                            id: includeScreenshot
                                            checked: true
                                        }
                                    }
                                    
                                    Divider {
                                        id: divider
                                        topMargin: 0; bottomMargin: 10
                                    }
                                    
                                    TextArea
                                    {
                                        id: body
                                        property string template: qsTr("Name:\n\n\nEmail Address:\n\n\nSummary of Bug:\n\n\nSteps To Reproduce:\n\n\nHow often can you reproduce this?") + Retranslate.onLanguageChanged
                                        topMargin: 0; topPadding: 0
                                        backgroundVisible: false
                                        hintText: qsTr("Enter the notes you wish to add\n\nPlease start with your name and email address...") + Retranslate.onLanguageChanged
                                        text: template
                                        content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff
                                        translationY: 500
                                        
                                        onTextChanging: {
                                            submit.enabled = text != template;
                                        }
                                    }
                                }
                            }
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