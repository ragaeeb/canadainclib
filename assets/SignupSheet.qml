import bb.cascades 1.0

Sheet
{
    id: sheet
    
    Page
    {
        property variant appScene: Application.scene
        
        onAppSceneChanged: {
            passwordField.requestFocus();
        }
        
        titleBar: TitleBar {
            title: {
                if ( security.accountCreated() ) {
                    return qsTr("Change Password") + Retranslate.onLanguageChanged
                } else {
                    return qsTr("Create Password") + Retranslate.onLanguageChanged
                }
            }
        }
        
        Container
        {
            TextField {
                id: passwordField
                
                inputMode: TextFieldInputMode.Password
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                
                input.submitKey: SubmitKey.Submit
                
                input.onSubmitted: {
                    confirmField.requestFocus();
                }
            }
            
            TextField {
                id: confirmField
                
                inputMode: TextFieldInputMode.Password
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                
                input.submitKey: SubmitKey.Submit
                
                input.onSubmitted: {
                    setPasswordButton.clicked();
                }
            }
            
            Button {
                id: setPasswordButton
                text: qsTr("Set Password") + Retranslate.onLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                
                onClicked: {
                    if (passwordField.text != confirmField.text) {
                        persist.showToast( qsTr("Passwords do not match!") );
                        confirmField.requestFocus();
                    } else if ( passwordField.text.length < 3 ) {
                        persist.showToast( qsTr("Password must be at least 3 characters minimum!") );
                        passwordField.requestFocus();
                    } else {
                        security.savePassword(passwordField.text);
                        persist.showToast( qsTr("Successfully set password!\n\nMake sure you remember it!") );
                        
                        sheet.close();
                    }
                }
            }
        }
        
        onCreationCompleted: {
            if ( security.accountCreated() ) {
                titleBar.dismissAction = cancelDefinition.createObject();
            }
        }
        
        attachedObjects: [
            ComponentDefinition
            {
                id: cancelDefinition
                
                ActionItem {
                    id: cancelAction
                    title: qsTr("Cancel") + Retranslate.onLanguageChanged
                    
                    onTriggered: {
                        sheet.close();
                    }
                }
            }
        ]
    }
    
    onClosed: {
        sheet.destroy();
    }
}