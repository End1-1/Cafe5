#ifndef SELFBOARDUPDATE_H
#define SELFBOARDUPDATE_H

#include "c5updaterlauncher.h"
#include "dlgupdaterequired.h"
#include "ninterface.h"

#include <QApplication>
#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include <QTimer>
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

    // Top-level window: a translucent child of the fullscreen kiosk crashes on close (c0000005).
    DlgUpdateRequired dlg(nullptr);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setUpdateInfo(msg, oldVersion, newVersion);
    dlg.adjustSize();
    if (parent && parent->window()) {
        dlg.move(parent->window()->geometry().center() - dlg.rect().center());
    }

    const int answer = dlg.exec();
    dlg.prepareForClose();

    const auto leave = [] {
        QTimer::singleShot(0, qApp, [] { QCoreApplication::exit(0); });
    };

    if (answer != QDialog::Accepted) {
        leave();
        return;
    }
    if (!C5UpdaterLauncher::tryStart(appName, newVersion)) {
        QMessageBox::information(
            nullptr,
            QObject::tr("Update required"),
            QObject::tr("Could not start the updater. Reinstall the application or run the setup from picasso.am."));
    }
    leave();
}

} // namespace SelfBoardUpdate

#endif // SELFBOARDUPDATE_H
