import bb.cascades 1.0

ComponentDefinition
{
    property string messageBody
    
    onCreationCompleted: {
        if ( persist.getValueFor("hideAgreement") != 1 )
        {
            var sheet = createObject();
            sheet.open();
        }
    }
    
    Sheet
    {
        id: root
        
        onClosed: {
            destroy();
        }
        
        Page
        {
            titleBar: TitleBar {
                title: qsTr("Agreement") + Retranslate.onLanguageChanged
                
                acceptAction: ActionItem {
                    title: qsTr("Accept") + Retranslate.onLanguageChanged
                    
                    onTriggered: {
                        if (hideNextTime.checked) {
                            persist.saveValueFor("hideAgreement", 1);
                        }
                        
                        root.close();
                    }
                }
            }
            
            Container
            {
                leftPadding: 10; rightPadding: 10; topPadding: 10; bottomPadding: 10
                
                ScrollView
                {
                    scrollViewProperties.pinchToZoomEnabled: true
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    Label {
                        multiline: true
                        textStyle.textAlign: TextAlign.Center
                        verticalAlignment: VerticalAlignment.Center
                        content.flags: TextContentFlag.ActiveText
                        text: messageBody
                    }   
                }
                
                CheckBox {
                    id: hideNextTime
                    text: qsTr("Don't show again") + Retranslate.onLanguageChanged
                    verticalAlignment: VerticalAlignment.Bottom
                }
            }
        }
    }
}