#include "dlgscreen.h"
#include "ui_dlgscreen.h"
#include "c5user.h"
#include "dlgface.h"
#include "ndataprovider.h"
#include "c5permissions.h"
#include "c5message.h"
#include "c5connectiondialog.h"
#include "dict_workstation.h"
#include "ninterface.h"
#include "c5utils.h"
#include "struct_workstationitem.h"
#include "dlgdashboard.h"
#include <QKeyEvent>
#include <QPainter>

DlgScreen::DlgScreen(C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgScreen)
{
    ui->setupUi(this);
    ui->lbVersion->setText(NDataProvider::mFileVersion);
    updatePin();
    installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

DlgScreen::~DlgScreen()
{
    delete ui;
}

bool DlgScreen::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        auto *k = static_cast<QKeyEvent*>(e);

        if(k->key() == Qt::Key_Escape ||
                k->key() == Qt::Key_Control ||
                k->key() == Qt::Key_Shift ||
                k->key() == Qt::Key_Alt ||
                k->key() == Qt::Key_Meta) {
            return true;
        }

        if(k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter) {
            on_btnAccept_clicked();
            return true;
        }

        if(k->key() == Qt::Key_Backspace && !mPin.isEmpty()) {
            mPin.chop(1);
            updatePin();
            return true;
        }

        if(!k->text().isEmpty()) {
            mPin += k->text();
            updatePin();
            return true;
        }
    }

    return C5WaiterDialog::eventFilter(o, e);
}

void DlgScreen::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QPixmap mBg(":/waiterbg.jpg");

    if(!mBg.isNull()) {
        // cover: сохраняем пропорции, обрезаем лишнее
        QPixmap scaled = mBg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPoint topLeft((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
        p.drawPixmap(topLeft, scaled);
    }
}

void DlgScreen::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);
    NInterface::query1("/engine/v2/waiter/workstation/get-config", mUser->mSessionKey, this,
    {{"type", WORKSTATION_WAITER}, {"station_account", hostusername()}, {"workstation", hostinfo}},
    [](const QJsonObject & jdoc) {
        mWorkStation = JsonParser<WorkstationItem>::fromJson(jdoc);
    });
}

void DlgScreen::on_btnCancel_clicked()
{
    mPin.clear();
    updatePin();
}

void DlgScreen::on_btn1_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}
void DlgScreen::on_btn2_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn3_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn4_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn5_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn6_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn7_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn8_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn9_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn0_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btnAccept_clicked()
{
    auto *user = new C5User();
    QString pin = mPin;
    mPin.clear();
    updatePin();
    user->authorize(pin, fHttp, [ = ](const  QJsonObject & jdoc) {
        QJsonObject jo = jdoc["data"].toObject();
        NDataProvider::sessionKey = jo["sessionkey"].toString();

        if(!user->check(cp_t5_waiter_edit_order)) {
            user->deleteLater();
            C5Message::error(tr("You have not permission to enter the working area"));
            return;
        }

        QTimer::singleShot(1, user, [ = ]() {
            int finish = 0;

            do {
                QDialog *dlg;

                switch(finish) {
                case 2:
                    dlg = new DlgDashboard(QJsonObject(), user);
                    break;

                case 3:
                    dlg = new DlgFace(user);
                    break;

                default:
                    dlg = new DlgFace(user);
                }

                finish = dlg->exec();
                delete dlg;
            } while(finish > 1);
        });
    }, [user]() {
        user->deleteLater();
    });
}

void DlgScreen::on_lePassword_returnPressed()
{
    on_btnAccept_clicked();
}

void DlgScreen::tryExit()
{
    if(C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void DlgScreen::updatePin()
{
    ui->lbPin->clear();

    for(int i = 0; i < mPin.length(); i++) {
        ui->lbPin->setText(ui->lbPin->text() + "●");
    }
}

void DlgScreen::on_btnSettings_clicked()
{
    C5ConnectionDialog::showSettings(this);
}

void DlgScreen::on_btnClose_clicked()
{
    if(C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}
