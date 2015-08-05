#ifndef LOCALEUTIL_H_
#define LOCALEUTIL_H_

#include <bb/cascades/LocaleHandler>

namespace canadainc {

class LocaleUtil : public QObject
{
	Q_OBJECT

	QMap<QString, QTranslator*> m_translators;
    bb::cascades::LocaleHandler m_pLocaleHandler;
    QString m_currentLocale;

private slots:
    void onSystemLanguageChanged();

public:
	LocaleUtil(QString appName=QString(), QStringList libs=QStringList());
	~LocaleUtil();

	QString locale() const;
};

} /* namespace canadainc */
#endif /* LOCALEUTIL_H_ */
