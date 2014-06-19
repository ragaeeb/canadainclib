import bb.cascades 1.0

DropDown
{
    id: accountChoice
    property variant selectedAccountId
    signal accountsLoaded(int numAccounts);
    title: qsTr("Account") + Retranslate.onLanguageChanged
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
            option.isCellular = current["isCellular"];
            
            var imageSource = "images/dropdown/ic_account.png";
            
            if (current.accountId == 23) {
                imageSource = "images/dropdown/ic_sms.png";
            } else if (current.isCellular) {
                imageSource = "images/dropdown/ic_phone.png";
            } else if (current.accountId == 199) {
                imageSource = "images/dropdown/ic_pin.png";
            }
            
            option.imageSource = imageSource;
            
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
        ComponentDefinition
        {
            id: optionDefinition

            Option {
                property bool isCellular: false
            }
        }
    ]
}