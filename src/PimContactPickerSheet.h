#ifndef PIMCONTACTPICKERSHEET_H_
#define PIMCONTACTPICKERSHEET_H_

#include <QObject>
#include <QVariant>

namespace bb {
	namespace cascades {
		namespace pickers {
			class ContactPicker;
		}
	}
}

namespace canadainc {

using namespace bb::cascades::pickers;

class PimContactPickerSheet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool filterMobile READ filterMobile WRITE setFilterMobile FINAL)

    ContactPicker* m_picker;
    bool m_filterMobile;

public:
    PimContactPickerSheet(QObject* parent=NULL);
    virtual ~PimContactPickerSheet();

    Q_INVOKABLE void open();
    bool filterMobile() const;
    void setFilterMobile(bool filter);

signals:
	void contactSelected(QVariantMap const& result);
	void canceled();

private slots:
	void onContactSelected(int);
};

} /* namespace canadainc */

#endif /* PIMCONTACTPICKERSHEET_H_ */
