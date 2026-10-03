#ifndef SELFBOARDUPDATE_H
#define SELFBOARDUPDATE_H

#include "c5updaterlauncher.h"
#include "dlgupdaterequired.h"
#include "ninterface.h"

#include <QApplication>
#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include <QWidget>

namespace SelfBoardUpdate {

inline void offerAndRun(QWidget *parent, const QString &msg, const QString &appName, const QString &newVersion)
{
    NInterface::forceCloseCurrentLoading();

    QString oldVersion;
    const QRegularExpression re(QStringLiteral("(\\d+(?:\\.\\d+){1,3})\\s*(?:→|->|=>)\\s*(\\d+(?:\\.\\d+){1,3})"));
    const auto m = re.match(msg);
    if (m.hasMatch()) {
        oldVersion = m.captured(1);
    }

    DlgUpdateRequired dlg(parent);
    dlg.setUpdateInfo(msg, oldVersion, newVersion);
    dlg.adjustSize();
    if (parent) {
        dlg.move(parent->window()->geometry().center() - dlg.rect().center());
    }

    if (dlg.exec() != QDialog::Accepted) {
        qApp->exit(0);
        return;
    }
    if (!C5UpdaterLauncher::tryStart(appName, newVersion)) {
        QMessageBox::information(
            parent,
            QObject::tr("Update required"),
            QObject::tr("Could not start the updater. Reinstall the application or run the setup from picasso.am."));
    }
    qApp->exit(0);
}

} // namespace SelfBoardUpdate

#endif // SELFBOARDUPDATE_H
