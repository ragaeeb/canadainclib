#ifndef REPORTGENERATOR_H_
#define REPORTGENERATOR_H_

#include "Report.h"

#define KEY_FIRST_INSTALL "firstInstall"
#define KEY_LAST_UPGRADE "lastUpgrade"
#define KEY_USER_ID "user_id"
#define WHATSAPP_PATH "/pps/accounts/1000/appserv/com.whatsapp.WhatsApp.gYABgD934jlePGCrd74r6jbZ7jk/app_status"

namespace canadainc {

struct ReportGenerator
{
    static Report generate(CompressFiles func, Report r);
};

} /* namespace canadainc */

#endif /* REPORTGENERATOR_H_ */
