#ifndef DIALOGUTILS_H_
#define DIALOGUTILS_H_

#include <QObject>

#include <bb/system/SystemUiPosition>
#include <bb/system/SystemUiResult>

namespace bb {
    namespace system {
        class InvokeManager;
        class SystemDialog;
        class SystemPrompt;
        class SystemToast;
    }
}

#define INVOKE_TARGET_PROPERTY "target"
#define INVOKE_CALLBACK_FUNC "onTargetLookupFinished"

namespace canadainc {

class DialogUtils : public QObject
{
    Q_OBJECT

    bb::system::SystemDialog* m_dialog;
    bb::system::SystemToast* m_toast;
    bb::system::SystemPrompt* m_prompt;

    friend class Persistance;

private slots:
    void dialogFinished(bb::system::SystemUiResult::Type value);
    void finished(bb::system::SystemUiResult::Type value);
    void onLookupFinished();
    void promptFinished(bb::system::SystemUiResult::Type value);

public:
    DialogUtils();
    virtual ~DialogUtils() {}

    bool isBlocked() const;
    void findTarget(QString const& uri, QString const& target, QObject* caller, bb::system::InvokeManager* im);
    static QString showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode);
    static bool showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled);
    void showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled, QString const& rememberMeText, bool rememberMeValue, QString const& funcName);
    void showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode, QString const& funcName, QVariant const& data);
    void showToast(QString const& text, QString const& icon, int pos);
};

} /* namespace canadainc */

#endif /* DIALOGUTILS_H_ */
