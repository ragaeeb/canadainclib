#ifndef PIMUTIL_H_
#define PIMUTIL_H_

#include <bb/system/InvokeManager>

#include <bb/pim/message/Attachment>

#define ACCOUNT_KEY_CELLULAR 8
#define ACCOUNT_KEY_PIN 199
#define ACCOUNT_KEY_SMS 23

namespace bb {
	namespace pim {
		namespace message {
			class Message;
			class MessageService;
		}
	}
}

namespace canadainc {

using namespace bb::pim::message;
using namespace bb::system;

class PimUtil : public QObject
{
	Q_OBJECT

public:
    static qint64 extractIdsFromInvoke(QString const& uri, QByteArray const& data, qint64& accountId);
    static qint64 sendMessage(MessageService* ms, Message const& m, QString text, QList<Attachment> const& attachments=QList<Attachment>(), bool replyPrefix=false);
    static QString extractText(Message const& m);
};

} /* namespace canadainc */
#endif /* PIMUTIL_H_ */
