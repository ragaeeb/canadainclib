#ifndef LOCALEUTIL_H_
#define LOCALEUTIL_H_

#include <bb/cascades/LocaleHandler>

class QDateTime;

namespace canadainc {

class LocaleUtil : public QObject
{
	Q_OBJECT

    QTranslator m_pTranslator;
	QTranslator m_libTranslator;
    bb::cascades::LocaleHandler m_pLocaleHandler;
    QString m_appName;

private slots:
    void onSystemLanguageChanged();

public:
	LocaleUtil(QString const& appName=QString(), QObject* parent=NULL);
	~LocaleUtil();
};

} /* namespace canadainc */
#endif /* LOCALEUTIL_H_ */
