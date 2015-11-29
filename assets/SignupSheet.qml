import bb.cascades 1.2

Sheet
{
    id: sheet
    
    Page
    {
        property variant appScene: Application.scene
        
        onAppSceneChanged: {
            passwordField.requestFocus();
        }
        
        titleBar: TitleBar
        {
            id: tb
            
            title: {
                if ( security.accountCreated() ) {
                    return qsTr("Change Password") + Retranslate.onLanguageChanged
                } else {
                    return qsTr("Create Password") + Retranslate.onLanguageChanged
                }
            }
            
            acceptAction: ActionItem
            {
                id: setPasswordAction
                imageSource: "images/common/dropdown/set_password.png"
                title: qsTr("Save") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    console.log("UserEvent: SetPassword");
                    reporter.record("SetPassword");
                    
                    minCharValidator.validate();
                    equalityValidator.validate();
                    
                    if (minCharValidator.valid && equalityValidator.valid)
                    {
                        reporter.record("PasswordLength", passwordField.text.length);
                        security.savePassword(passwordField.text);
                        persist.showToast( qsTr("Successfully set password!\n\nMake sure you remember it!"), "images/common/dropdown/set_password.png" );
                        sheet.close();
                    }
                }
            }
        }
        
        Container
        {
            TextField
            {
                id: passwordField
                inputMode: TextFieldInputMode.Password
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                hintText: qsTr("Enter a password at least 3 characters long...") + Retranslate.onLanguageChanged
                input.submitKey: SubmitKey.Submit
                input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                maximumLength: 20
                
                validator: Validator
                {
                    id: minCharValidator
                    errorMessage: qsTr("The password must be at least 3 characters!") + Retranslate.onLanguageChanged
                    mode: ValidationMode.FocusLost
                    
                    onValidate: {
                        valid = passwordField.text.length > 2;
                    }
                }
            }
            
            TextField
            {
                id: confirmField
                inputMode: TextFieldInputMode.Password
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                hintText: qsTr("Confirm password") + Retranslate.onLanguageChanged
                input.submitKey: SubmitKey.Submit
                input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Lose
                
                validator: Validator
                {
                    id: equalityValidator
                    errorMessage: qsTr("Passwords do not match!") + Retranslate.onLanguageChanged
                    mode: ValidationMode.FocusLost
                    
                    onValidate: {
                        valid = confirmField.text == passwordField.text;
                        
                        if (!valid) {
                            reporter.record("MismatchingPasswords");
                        }
                    }
                }
                
                input.onSubmitted: {
                    setPasswordAction.triggered();
                }
            }
        }
        
        attachedObjects: [
            ComponentDefinition
            {
                id: cancelDefinition
                
                ActionItem
                {
                    id: cancelAction
                    imageSource: "images/common/dropdown/dismiss.png"
                    title: qsTr("Cancel") + Retranslate.onLanguageChanged
                    
                    onTriggered: {
                        console.log("UserEvent: SignupSheetCancel");
                        reporter.record("SignupSheetCancel");
                        sheet.close();
                    }
                }
            }
        ]
    }
    
    onOpened: {
        passwordField.requestFocus();
        
        tutorial.execBelowTitleBar("inputPassword", qsTr("Enter your desired password here. It needs to be at least 3-characters in length.") );
        tutorial.execBelowTitleBar("confirmPassword", qsTr("Confirm the same password you typed above here to ensure you remember it."), deviceUtils.du(8) );
        tutorial.execTitle( "savePassword", qsTr("Once you are satisfied with your password, tap on the '%1' button to save it.").arg(setPasswordAction.title), "r" );
        
        if ( security.accountCreated() ) {
            tb.dismissAction = cancelDefinition.createObject();
            tutorial.execTitle( "cancelSavePassword", qsTr("If you don't want to change the password, you can click the 'Cancel' button to dismiss this dialog."), "l" );
        }
    }
    
    onClosed: {
        sheet.destroy();
    }
}