import bb.cascades 1.0

ActionItem
{
    title: qsTr("Donate") + Retranslate.onLanguageChanged
    imageSource: "images/ic_donate.png"

    onTriggered: {
        donator.trigger("bb.action.OPEN");
    }

    attachedObjects: [
        Invocation {
            id: donator

            query {
                mimeType: "text/html"
                uri: "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=admin@canadainc.org&currency_code=CAD&no_shipping=1&tax=0&lc=CA&bn=PP-DonationsBF&item_name=Support NotepadPlus Development"
            }
        }
    ]
}