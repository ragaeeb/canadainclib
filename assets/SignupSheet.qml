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
                imageSource: "images/dropdown/set_password.png"
                title: qsTr("Save") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    console.log("UserEvent: SetPassword");
                    
                    minCharValidator.validate();
                    equalityValidator.validate();
                    
                    if (minCharValidator.valid && equalityValidator.valid)
                    {
                        security.savePassword(passwordField.text);
                        persist.showToast( qsTr("Successfully set password!\n\nMake sure you remember it!"), "", "asset:///images/dropdown/set_password.png" );
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
                input.submitKey: SubmitKey.Submit
                input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Lose
                
                validator: Validator
                {
                    id: equalityValidator
                    errorMessage: qsTr("Passwords do not match!") + Retranslate.onLanguageChanged
                    mode: ValidationMode.FocusLost
                    
                    onValidate: {
                        valid = confirmField.text == passwordField.text;
                    }
                }
                
                input.onSubmitted: {
                    setPasswordAction.triggered();
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
                
                ActionItem
                {
                    id: cancelAction
                    imageSource: "images/dropdown/dismiss.png"
                    title: qsTr("Cancel") + Retranslate.onLanguageChanged
                    
                    onTriggered: {
                        console.log("UserEvent: SignupSheetCancel");
                        sheet.close();
                    }
                }
            }
        ]
    }
    
    onOpened: {
        passwordField.requestFocus();
    }
    
    onClosed: {
        sheet.destroy();
    }
}