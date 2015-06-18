import bb.cascades 1.2
import bb.cascades.pickers 1.0

Page
{
    id: root
    property string projectName
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    
    onProjectNameChanged: {
        webView.url = "https://github.com/canadainc/%1/issues".arg(projectName);
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
        }
    }
    
    actions: [
        ActionItem
        {
            id: browserAction
            imageSource: "images/bugs/ic_open_browser.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            title: qsTr("Open in Browser") + Retranslate.onLanguageChanged
            
            onTriggered: {
                console.log("UserEvent: OpenInBrowser");
                persist.openUri( webView.url.toString() );
                reporter.record("OpenInBrowser");
            }
        },
        
        ActionItem
        {
            id: submitLogs
            ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
            title: qsTr("Submit Logs") + Retranslate.onLanguageChanged
            imageSource: "images/ic_bugs.png"
            
            onTriggered: {
                console.log("UserEvent: SubmitLogs");
                enabled = false;
                sheetDelegate.active = true;
                reporter.record("SubmitLogs");
            }
            
            function onSubmitted(message)
            {
                progressIndicator.visible = false;
                persist.showDialog( qsTr("Submission Status"), message );
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
                                reporter.simulationFilesAvailable.disconnect(simulate.onSimulationComplete);
                                sheetDelegate.active = false;
                            }
                            
                            onOpened: {
                                nameField.requestFocus();
                            }
                            
                            Page
                            {
                                id: notesPage
                                
                                onCreationCompleted: {
                                    if (reporter.isAdmin) {
                                        addAction(simulate);
                                    }
                                }
                                
                                actions: [
                                    ActionItem
                                    {
                                        id: attach
                                        imageSource: "images/menu/ic_copy.png"
                                        title: qsTr("Attach") + Retranslate.onLanguageChanged
                                        ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
                                        onTriggered: {
                                            console.log("UserEvent: Attach");
                                            reporter.record("AttachFiles");
                                            filePicker.open();
                                        }
                                        
                                        attachedObjects: [
                                            FilePicker {
                                                id: filePicker
                                                directories: ["/accounts/1000/shared/camera"]
                                                type : FileType.Picture
                                                title : qsTr("Select Files") + Retranslate.onLanguageChanged
                                                mode: FilePickerMode.PickerMultiple
                                                property variant attachments
                                                
                                                onFileSelected : {
                                                    var n = selectedFiles.length;
                                                    reporter.record("AttachmentCount", n);
                                                    
                                                    if (n > 5) {
                                                        persist.showToast( qsTr("Only a maximum of 5 screenshots may be attached!"), "asset:///images/bugs/ic_bugs_cancel.png" );
                                                        attachments = undefined;
                                                    } else {
                                                        attachments = selectedFiles;
                                                    }
                                                }
                                                
                                                onCanceled: {
                                                    attachments = undefined;
                                                    persist.showToast( qsTr("Attachments cleared!"), "asset:///images/bugs/ic_bugs_cancel.png" );
                                                    reporter.record("AttachFilesCancel");
                                                }
                                            }
                                        ]
                                    }
                                ]
                                
                                attachedObjects: [
                                    ActionItem
                                    {
                                        id: simulate
                                        imageSource: "images/bugs/ic_bugs_submit.png"
                                        title: qsTr("Simulate") + Retranslate.onLanguageChanged
                                        ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar

                                        onTriggered: {
                                            console.log("UserEvent: Simulate");
                                            reporter.submitLogs(body.text, true, true, filePicker.attachments);
                                        }
                                        
                                        function onSimulationComplete(result)
                                        {
                                            body.visible = false;
                                            
                                            adm.clear();
                                            adm.append(result);
                                        }
                                        
                                        onCreationCompleted: {
                                            reporter.simulationFilesAvailable.connect(onSimulationComplete);
                                        }
                                    }
                                ]
                                
                                titleBar: TitleBar
                                {
                                    title: filePicker.attachments ? qsTr("%n attachments", "", filePicker.attachments.length) + Retranslate.onLanguageChanged : qsTr("Add Notes") + Retranslate.onLanguageChanged
                                    
                                    acceptAction: ActionItem
                                    {
                                        id: submit
                                        imageSource: "images/bugs/ic_bugs_submit.png"
                                        title: qsTr("Submit") + Retranslate.onLanguageChanged
                                        
                                        onTriggered: {
                                            console.log("UserEvent: SubmitNotes");
                                            nameField.validator.validate();
                                            emailField.validator.validate();
                                            
                                            if ( body.text.trim() == body.template ) {
                                                persist.showToast( qsTr("Please enter detailed notes about the bug you observed!"), "images/bugs/ic_bugs_info.png" );
                                                return;
                                            }
                                            
                                            if (nameField.validator.valid && emailField.validator.valid)
                                            {
                                                reporter.submitLogs( nameField.text.trim()+"\n"+emailField.text.trim()+"\n\n"+body.text.trim(), true, filePicker.attachments);
                                                progressIndicator.value = 0;
                                                progressIndicator.state = ProgressIndicatorState.Progress;
                                                progressIndicator.visible = true;
                                                sheet.close();
                                            }
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
                                    
                                    Divider {
                                        id: divider
                                        topMargin: 0; bottomMargin: body.visible ? 10 : 0
                                    }
                                    
                                    ListView
                                    {
                                        id: simulationList
                                        scrollRole: ScrollRole.Main
                                        visible: !body.visible

                                        dataModel: ArrayDataModel {
                                            id: adm
                                        }
                                        
                                        listItemComponents: [
                                            ListItemComponent
                                            {
                                                StandardListItem
                                                {
                                                    title: ListItemData.substring( ListItemData.lastIndexOf("/")+1 )
                                                    
                                                    function endsWith(str, suffix) {
                                                        return str.indexOf(suffix, str.length - suffix.length) !== -1;
                                                    }
                                                    
                                                    imageSource: {
                                                        if ( endsWith(ListItemData, "txt") ) {
                                                            return "images/ic_bugs.png";
                                                        } else if ( endsWith(ListItemData, "log") ) {
                                                            return "images/bugs/ic_bugs_cancel.png";
                                                        } else if ( endsWith(ListItemData, "conf") ) {
                                                            return "images/bugs/ic_bugs_submit.png";
                                                        } else {
                                                            return "images/bugs/ic_open_browser.png";
                                                        }
                                                    }
                                                }
                                            }
                                        ]
                                        
                                        onTriggered: {
                                            persist.openUri("file://"+dataModel.data(indexPath));
                                        }
                                    }
                                    
                                    TextField
                                    {
                                        id: nameField
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        hintText: qsTr("Name:") + Retranslate.onLanguageChanged
                                        topMargin: 0; bottomMargin: 0
                                        content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff
                                        backgroundVisible: false
                                        inputMode: TextFieldInputMode.Text
                                        visible: body.visible
                                        maximumLength: 25
                                        input.submitKey: SubmitKey.Next
                                        input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                                        
                                        validator: Validator
                                        {
                                            errorMessage: qsTr("Name cannot be empty!") + Retranslate.onLanguageChanged
                                            
                                            onValidate: {
                                                valid = nameField.text.trim().length > 0;
                                            }
                                        }
                                    }
                                    
                                    TextField
                                    {
                                        id: emailField
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        hintText: qsTr("Email Address:") + Retranslate.onLanguageChanged
                                        topMargin: 0; bottomMargin: 0
                                        content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff
                                        inputMode: TextFieldInputMode.EmailAddress
                                        backgroundVisible: false
                                        maximumLength: 40
                                        visible: body.visible
                                        input.submitKey: SubmitKey.Next
                                        input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                                        
                                        input.onSubmitted: {
                                            if (body.text == body.template) {
                                                body.editor.cursorPosition = body.text.indexOf(":")+2;
                                            }
                                        }
                                        
                                        validator: Validator
                                        {
                                            errorMessage: qsTr("Invalid email address!") + Retranslate.onLanguageChanged
                                            
                                            onValidate: {
                                                var t = emailField.text.trim();
                                                var firstAtSign = t.indexOf("@");
                                                var lastAtSign = t.lastIndexOf("@");
                                                var dotExists = t.indexOf(".") > 0;
                                                
                                                valid = t.length > 8 && firstAtSign >= 0 && lastAtSign >= 0 && firstAtSign == lastAtSign && dotExists;
                                            }
                                        }
                                    }
                                    
                                    Divider {
                                        topMargin: 0; bottomMargin: 0
                                        visible: emailField.visible
                                    }
                                    
                                    TextArea
                                    {
                                        id: body
                                        property string template: qsTr("Summary of Bug:\n\n\nSteps To Reproduce:\n\n\nHow often can you reproduce this?") + Retranslate.onLanguageChanged
                                        topMargin: 0; topPadding: 0
                                        backgroundVisible: false
                                        hintText: qsTr("Enter the notes you wish to add...") + Retranslate.onLanguageChanged
                                        text: template
                                        content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff
                                        verticalAlignment: VerticalAlignment.Fill
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
	        scrollRole: ScrollRole.Main
	        
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
	
	function cleanUp()
	{
        reporter.progress.disconnect(onNetworkProgressChanged);
        reporter.submitted.connect(submitLogs.onSubmitted);
	}
	
	onCreationCompleted: {
        tutorial.execActionBar( "submitLogs", qsTr("If you were instructed by our staff to submit a bug report, please use the '%1' action at the bottom. Then fill out the form, and send the representative the Bug Report ID generated.").arg(submitLogs.title) );
        tutorial.execActionBar( "openBugsInBrowser", qsTr("To open this page in the web browser, please use the '%1' action at the bottom.").arg(browserAction.title), "r" );
	}
}