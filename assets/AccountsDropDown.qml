import bb.cascades 1.0

DropDown
{
    id: accountChoice
    property variant selectedAccountId
    property bool immediate: true
    property variant controller: app
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
            
            var imageSource = "images/common/dropdown/ic_account.png";
            
            if (current.accountId == 23) {
                imageSource = "images/common/dropdown/ic_sms.png";
            } else if (current.accountId == 8) {
                imageSource = "images/common/dropdown/ic_phone.png";
            } else if (current.accountId == 199) {
                imageSource = "images/common/dropdown/ic_pin.png";
            }
            
            option.imageSource = imageSource;
            
            if (current.accountId == selectedAccountId) {
                option.selected = true;
            }
            
            add(option);
        }
        
        accountsLoaded(results.length);
    }
    
    function onReady() {
        controller.accountsImported.connect(onAccountsImported);
        controller.loadAccounts();
    }
    
    onCreationCompleted: {
        if (!immediate) {
            app.lazyInitComplete.connect(onReady);
        } else {
            onReady();
        }
    }
    
    attachedObjects: [
        ComponentDefinition
        {
            id: optionDefinition
            Option {}
        }
    ]
}