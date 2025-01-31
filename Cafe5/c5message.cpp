#include "c5message.h"
#include "ui_c5message.h"
#include "c5config.h"
#include <QClipboard>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

C5Message::C5Message(QWidget *parent) :
#ifdef WAITER
    QDialog(parent, Qt::FramelessWindowHint),
#else
    QDialog(parent),
#endif
    ui(new Ui::C5Message)
{
    ui->setupUi(this);
#ifndef WAITER
    ui->frame->setFrameShape(QFrame::NoFrame);
#endif
    ui->btnCopy->setVisible(false);
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &C5Message::timeout);
    timer->start(3000);
}

C5Message::~C5Message()
{
    delete ui;
}

int C5Message::error(const QString &errorStr, const QString &yes, const QString &no)
{
    return showMessage(errorStr, 2, yes, no, "");
}

int C5Message::info(const QString &infoStr, const QString &yes, const QString &no, bool playSound)
{
    return showMessage(infoStr, 1, yes, no, "", playSound);
}

int C5Message::question(const QString &questionStr, const QString &yes, const QString &no, const QString &a3)
{
    return showMessage(questionStr, 3, yes, no, a3);
}

void C5Message::timeout()
{
    if (fPlaySound) {
        QMediaPlayer *mp = new QMediaPlayer();
        auto *ao = new QAudioOutput();
        mp->setAudioOutput(ao);
        mp->setSource(QUrl(":/icq.wav"));
        ao->setVolume(0.5);
        mp->play();
    }
}

int C5Message::showMessage(const QString &text, int tp, const QString &yes, const QString &no, const QString &a3,
                           bool playsound)
{
    C5Message *c5 = new C5Message(__c5config.fParentWidget);
    c5->fPlaySound = playsound;
    c5->ui->btnYes->setText(yes);
    c5->ui->btnCancel->setText(no);
    if (no.isEmpty()) {
        c5->ui->btnCancel->setVisible(false);
    }
    c5->ui->btnA3->setVisible(!a3.isEmpty());
    c5->ui->btnA3->setText(a3);
    QString img;
    switch (tp) {
        case 1:
            img = "info";
            break;
        case 2:
            img = "error";
            c5->ui->btnCopy->setVisible(true);
            break;
        case 3:
            img = "help";
            break;
    }
#ifdef WAITER
    c5->ui->btnYes->setMinimumHeight(50);
    c5->ui->btnCancel->setMinimumHeight(50);
    c5->ui->btnYes->setMinimumWidth(100);
    c5->ui->btnCancel->setMinimumWidth(100);
#endif
    c5->ui->img->setPixmap(QPixmap(QString(":/%1.png").arg(img)));
    c5->ui->label->setText(text);
    c5->adjustSize();
    int result = c5->exec();
    delete c5;
    return result;
}

void C5Message::on_btnYes_clicked()
{
    accept();
}

void C5Message::on_btnCancel_clicked()
{
    reject();
}

void C5Message::on_btnA3_clicked()
{
    done(2);
}

void C5Message::on_btnCopy_clicked()
{
    qApp->clipboard()->setText(ui->label->text());
}
