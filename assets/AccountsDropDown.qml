import bb.cascades 1.0

DropDown
{
    id: accountChoice
    title: qsTr("Account") + Retranslate.onLanguageChanged
    property variant selectedAccountId
    signal accountsLoaded(int numAccounts);
    horizontalAlignment: HorizontalAlignment.Fill
    
    function onAccountsImported(results)
    {
        for (var i = results.length-1; i >= 0; i--)
        {
            var current = results[i];
            var option = optionDefinition.createObject();
            option.text = current.name;
            option.description = current.address;
            option.value = current.accountId;
            
            if (current.accountId == selectedAccountId) {
                option.selected = true;
            }
            
            add(option);
        }
        
        accountsLoaded(results.length);
    }
    
    onCreationCompleted: {
        app.accountsImported.connect(onAccountsImported);
        app.loadAccounts();
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: optionDefinition
            
            Option {
                imageSource: value == 23 ? "images/dropdown/ic_sms.png" : "images/dropdown/ic_account.png"
            }
        }
    ]
}