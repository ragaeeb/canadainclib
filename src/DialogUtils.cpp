#include "DialogUtils.h"
#include "Logger.h"

#include <bb/system/InvokeManager>
#include <bb/system/InvokeQueryTargetsRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/system/SystemDialog>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemToast>

#define KEY_TOAST_SHOWING "showing"
#define KEY_ARGS "args"
#define KEY_CALLBACK "callback"
#define METHOD_NAME "onFinished"

namespace {

bool isNowBlocked = false;

}

namespace canadainc {

using namespace bb::system;

DialogUtils::DialogUtils(): m_dialog(NULL), m_toast(NULL), m_prompt(NULL)
{
    isNowBlocked = false;
}

void DialogUtils::showToast(QString const& text, QString const& icon, int pos)
{
    if (m_toast == NULL) {
        m_toast = new SystemToast(this);
        connect( m_toast, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( finished(bb::system::SystemUiResult::Type) ) );
    }

    m_toast->setBody(text);
    m_toast->setIcon( icon.startsWith("asset:///") || icon.startsWith("file:///") ? icon : "asset:///"+icon );
    m_toast->setProperty(KEY_TOAST_SHOWING, true);
    m_toast->setPosition( SystemUiPosition::Type(pos) );
    m_toast->show();
}


void DialogUtils::finished(bb::system::SystemUiResult::Type value)
{
    Q_UNUSED(value);
    sender()->setProperty(KEY_TOAST_SHOWING, false);
}


void DialogUtils::findTarget(QString const& uri, QString const& target, QObject* caller, InvokeManager* im)
{
    InvokeQueryTargetsRequest request;
    request.setUri(uri);

    InvokeQueryTargetsReply* itr = im->queryTargets(request);
    connect( itr, SIGNAL( finished() ), this, SLOT( onLookupFinished() ) );
    itr->setParent(caller);
    itr->setProperty(INVOKE_TARGET_PROPERTY, target);
}


void DialogUtils::onLookupFinished()
{
    InvokeQueryTargetsReply* itr = static_cast<InvokeQueryTargetsReply*>( sender() );
    QString target = itr->property(INVOKE_TARGET_PROPERTY).toString();
    QObject* caller = itr->parent();
    bool result = false;

    QList<InvokeAction> actions = itr->actions();

    for (int i = actions.size()-1; i >= 0; i--)
    {
        QList<InvokeTarget> targets = actions[i].targets();

        foreach (InvokeTarget const& it, targets)
        {
            LOGGER( it.name() << target );
            if ( it.name() == target )
            {
                result = true;
                i = -1;
                break;
            }
        }
    }

    QMetaObject::invokeMethod( caller, INVOKE_CALLBACK_FUNC, Qt::QueuedConnection, Q_ARG(QVariant, target), Q_ARG(QVariant, result) );
    itr->deleteLater();
}


void DialogUtils::showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled, QString const& rememberMeText, bool rememberMeValue)
{
    isNowBlocked = true;

    if (m_dialog == NULL)
    {
        m_dialog = new SystemDialog(caller);
        connect( m_dialog, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( dialogFinished(bb::system::SystemUiResult::Type) ) );
    }

    bool showRememberMe = !rememberMeText.isEmpty();
    m_dialog->setIncludeRememberMe(showRememberMe);

    if (showRememberMe)
    {
        m_dialog->setRememberMeChecked(rememberMeValue);
        m_dialog->setRememberMeText(rememberMeText);
    }

    m_dialog->setParent(caller);
    m_dialog->setBody(text);
    m_dialog->setTitle(title);
    m_dialog->cancelButton()->setLabel(cancelButton);
    m_dialog->confirmButton()->setLabel(okButton);
    m_dialog->confirmButton()->setEnabled(okEnabled);
    m_dialog->setProperty(KEY_ARGS, data);
    m_dialog->show();
}


void DialogUtils::dialogFinished(bb::system::SystemUiResult::Type value)
{
    isNowBlocked = false;

    QObject* caller = m_dialog->parent();

    if (caller != NULL)
    {
        bool result = value == SystemUiResult::ConfirmButtonSelection;
        QVariant data = m_dialog->property(KEY_ARGS);
        m_dialog->setParent(this);

        if ( m_dialog->includeRememberMe() )
        {
            bool rememberMe = m_dialog->rememberMeSelection();

            if ( data.isValid() ) {
                QMetaObject::invokeMethod( caller, METHOD_NAME, Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, rememberMe), Q_ARG(QVariant, data) );
            } else {
                QMetaObject::invokeMethod( caller, METHOD_NAME, Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, rememberMe) );
            }
        } else {
            if ( data.isValid() ) {
                QMetaObject::invokeMethod( caller, METHOD_NAME, Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, data) );
            } else {
                QMetaObject::invokeMethod( caller, METHOD_NAME, Qt::QueuedConnection, Q_ARG(QVariant, result) );
            }
        }

        m_dialog->setProperty(KEY_ARGS, QVariant());
    }
}


void DialogUtils::showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode, QString const& funcName, QVariant const& data)
{
    isNowBlocked = true;

    SystemUiInputMode::Type m = (SystemUiInputMode::Type)inputMode;

    if (m_prompt == NULL)
    {
        m_prompt = new SystemPrompt(caller);
        connect( m_prompt, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( promptFinished(bb::system::SystemUiResult::Type) ) );
    }

    m_prompt->setParent(caller);
    m_prompt->setBody(body);
    m_prompt->setTitle(title);
    m_prompt->cancelButton()->setLabel(cancelButton);
    m_prompt->confirmButton()->setLabel(okButton);
    m_prompt->inputField()->setDefaultText(defaultText);
    m_prompt->inputField()->setEmptyText(hintText);
    m_prompt->inputField()->setMaximumLength(maxLength);
    m_prompt->inputField()->setInputMode(m);
    m_prompt->setInputOptions(autoCapitalize ? SystemUiInputOption::AutoCapitalize : SystemUiInputOption::None);
    m_prompt->setProperty(KEY_ARGS, data);
    m_prompt->setProperty(KEY_CALLBACK, funcName);
    m_prompt->show();
}


void DialogUtils::promptFinished(bb::system::SystemUiResult::Type value)
{
    isNowBlocked = false;

    QObject* caller = m_prompt->parent();

    if (caller != NULL)
    {
        QVariant data = m_prompt->property(KEY_ARGS);
        m_prompt->setParent(this);

        QString result = value == SystemUiResult::ConfirmButtonSelection ? m_prompt->inputFieldTextEntry().trimmed() : "";
        const char* callback = m_prompt->property(KEY_CALLBACK).toString().toUtf8().constData();

        if ( data.isValid() ) {
            QMetaObject::invokeMethod( caller, callback, Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, data) );
        } else {
            QMetaObject::invokeMethod( caller, callback, Qt::QueuedConnection, Q_ARG(QVariant, result) );
        }

        m_prompt->setProperty(KEY_ARGS, QVariant());
    }
}


bool DialogUtils::showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled)
{
    isNowBlocked = true;

    SystemDialog dialog;
    dialog.setBody(text);
    dialog.setTitle(title);
    dialog.confirmButton()->setLabel(okButton);
    dialog.cancelButton()->setLabel(cancelButton);
    dialog.confirmButton()->setEnabled(okEnabled);

    bool showRememberMe = !rememberMeText.isNull();

    if (showRememberMe)
    {
        dialog.setIncludeRememberMe(true);
        dialog.setRememberMeChecked(rememberMeValue);
        dialog.setRememberMeText(rememberMeText);
    }

    bool result = dialog.exec() == SystemUiResult::ConfirmButtonSelection;
    rememberMeValue = dialog.rememberMeSelection();

    isNowBlocked = false;

    return result;
}


bool DialogUtils::isBlocked() const {
    return isNowBlocked;
}


QString DialogUtils::showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode)
{
    isNowBlocked = true;

    SystemUiInputMode::Type m = (SystemUiInputMode::Type)inputMode;

    SystemPrompt dialog;
    dialog.setBody(body);
    dialog.setTitle(title);
    dialog.inputField()->setDefaultText(defaultText);
    dialog.inputField()->setEmptyText(hintText);
    dialog.inputField()->setMaximumLength(maxLength);
    dialog.inputField()->setInputMode(m);
    dialog.setInputOptions(autoCapitalize ? SystemUiInputOption::AutoCapitalize : SystemUiInputOption::None);
    dialog.confirmButton()->setLabel(okButton);
    dialog.cancelButton()->setLabel(cancelButton);

    bool result = dialog.exec() == SystemUiResult::ConfirmButtonSelection;

    isNowBlocked = false;

    return result ? dialog.inputFieldTextEntry() : QString();
}


} /* namespace canadainc */
