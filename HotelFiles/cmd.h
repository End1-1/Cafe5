#ifndef CMD_H
#define CMD_H

#include <QtGlobal>

namespace Cmd {
    static const quint32 cmdtype_json = 1;
    static const quint32 cmd_auth_cts = 1;
    static const quint32 cmd_auth_stc = 2;
    static const quint32 cmd_auth_user = 3;
    static const quint32 cmd_disconnect = 4;
}

#endif // CMD_H
