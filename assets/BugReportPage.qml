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
            imageSource: "images/common/bugs/ic_open_browser.png"
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
            imageSource: "images/common/bugs/ic_bugs.png"
            
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
                                nameField.requestFocus();
                            }
                            
                            Page
                            {
                                id: notesPage
                                
                                actions: [
                                    ActionItem
                                    {
                                        id: attach
                                        imageSource: "images/common/bugs/ic_attach.png"
                                        title: qsTr("Attach") + Retranslate.onLanguageChanged
                                        ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
                                        onTriggered: {
                                            console.log("UserEvent: Attach");
                                            filePicker.open();
                                            reporter.record("AttachFiles");
                                        }
                                        
                                        attachedObjects: [
                                            FilePicker {
                                                id: filePicker
                                                directories: ["/accounts/1000/shared/camera"]
                                                type : FileType.Picture
                                                title : qsTr("Select Files") + Retranslate.onLanguageChanged
                                                mode: FilePickerMode.PickerMultiple
                                                
                                                onFileSelected : {
                                                    var n = selectedFiles.length;
                                                    reporter.record("AttachmentCount", n);
                                                    
                                                    if (n > 5) {
                                                        persist.showToast( qsTr("Only a maximum of 5 screenshots may be attached!"), "asset:///images/common/bugs/ic_bugs_cancel.png" );
                                                    } else {
                                                        adm.clear();
                                                        adm.append(selectedFiles);
                                                    }
                                                }
                                            }
                                        ]
                                    }
                                ]
                                
                                titleBar: TitleBar
                                {
                                    title: adm.count > 0 ? qsTr("%n attachments", "", adm.count) + Retranslate.onLanguageChanged : qsTr("Add Notes") + Retranslate.onLanguageChanged
                                    
                                    acceptAction: ActionItem
                                    {
                                        id: submit
                                        imageSource: "images/common/bugs/ic_bugs_submit.png"
                                        title: qsTr("Submit") + Retranslate.onLanguageChanged
                                        
                                        onTriggered: {
                                            console.log("UserEvent: SubmitNotes");
                                            nameField.validator.validate();
                                            emailField.validator.validate();
                                            
                                            if ( body.text.trim() == body.template ) {
                                                persist.showToast( qsTr("Please enter detailed notes about the bug you observed!"), "images/common/bugs/ic_bugs_info.png" );
                                                return;
                                            }
                                            
                                            if (nameField.validator.valid && emailField.validator.valid)
                                            {
                                                var attachments = [];
                                                
                                                for (var i = adm.count; i >= 0; i--) {
                                                    attachments.push( adm.value(i) );
                                                }
                                                
                                                reporter.submitReport( nameField.text.trim(), emailField.text.trim(), body.text.trim(), attachments );
                                                progressIndicator.value = 0;
                                                progressIndicator.state = ProgressIndicatorState.Progress;
                                                progressIndicator.visible = true;
                                                sheet.close();
                                            }
                                        }
                                    }
                                    
                                    dismissAction: ActionItem
                                    {
                                        imageSource: "images/common/bugs/ic_bugs_cancel.png"
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
                                    
                                    ListView
                                    {
                                        id: attachList
                                        scrollRole: ScrollRole.Main
                                        visible: adm.count > 0
                                        maxHeight: 200

                                        dataModel: ArrayDataModel
                                        {
                                            id: adm
                                            property int count
                                            
                                            function refresh() {
                                                count = size();
                                            }
                                            
                                            onItemsChanged: {
                                                refresh();
                                            }
                                            
                                            onItemRemoved: {
                                                refresh();
                                            }
                                            
                                            onItemAdded: {
                                                refresh();
                                            }
                                        }
                                        
                                        listItemComponents: [
                                            ListItemComponent
                                            {
                                                StandardListItem
                                                {
                                                    id: sli
                                                    title: ListItemData.substring( ListItemData.lastIndexOf("/")+1 )
                                                    imageSource: "images/common/bugs/ic_attach.png"
                                                    
                                                    contextActions: [
                                                        ActionSet
                                                        {
                                                            title: sli.title
                                                            subtitle: ListItemData.substring( 0, ListItemData.lastIndexOf("/") )
                                                            
                                                            DeleteActionItem
                                                            {
                                                                imageSource: "images/common/bugs/ic_remove_attachment.png"
                                                                
                                                                onTriggered: {
                                                                    console.log("UserEvent: RemoveAttachment");
                                                                    
                                                                    sli.ListItem.view.dataModel.removeAt(sli.ListItem.indexPath[0]);
                                                                    reporter.record("RemoveAttachment");
                                                                }
                                                            }
                                                        }
                                                    ]
                                                }
                                            }
                                        ]
                                        
                                        onTriggered: {
                                            console.log("UserEvent: AttachPreview");
                                            reporter.record("AttachPreview");
                                            persist.invoke( "sys.pictures.card.previewer", "bb.action.VIEW", "", "file://"+dataModel.data(indexPath) );
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
                                        maximumLength: 25
                                        input.submitKey: SubmitKey.Next
                                        input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                                        text: persist.getFlag("cached_user_name")
                                        
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
                                        input.submitKey: SubmitKey.Next
                                        input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                                        text: persist.getFlag("cached_user_email")
                                        
                                        input.onSubmitted: {
                                            if (body.text == body.template) {
                                                body.editor.cursorPosition = body.text.indexOf(":")+2;
                                            }
                                        }
                                        
                                        validator: Validator
                                        {
                                            errorMessage: qsTr("Invalid email address!") + Retranslate.onLanguageChanged
                                            
                                            onValidate: {
                                                valid = deviceUtils.isValidEmail( emailField.text.trim() );
                                            }
                                        }
                                    }
                                    
                                    Divider {
                                        topMargin: 0; bottomMargin: 0
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
	    }
	}
	
	function cleanUp()
	{
        reporter.progress.disconnect(progressIndicator.onNetworkProgressChanged);
        reporter.submitted.disconnect(submitLogs.onSubmitted);
	}
	
	onCreationCompleted: {
        reporter.progress.connect(progressIndicator.onNetworkProgressChanged);
        reporter.submitted.connect(submitLogs.onSubmitted);
	    
        tutorial.execActionBar( "submitLogs", qsTr("If you were instructed by our staff to submit a bug report, please use the '%1' action at the bottom. Then fill out the form, and send the representative the Bug Report ID generated.").arg(submitLogs.title) );
        tutorial.execActionBar( "openBugsInBrowser", qsTr("To open this page in the web browser, please use the '%1' action at the bottom.").arg(browserAction.title), "r" );
	}
}