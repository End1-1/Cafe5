#include "notificationwidget.h"
#include "ui_notificationwidget.h"
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QDesktopWidget>
#include <QScreen>

QList<NotificationWidget *> NotificationWidget::fWidgetList;

NotificationThread::NotificationThread(const QString &message, const QVariant &callbackData, char *callback,
                                       QObject *callbackObject, int info) :
    QObject()
{
    fMessage = message;
    fInfo = info;
    fCallback  = callback;
    fCallbackData = callbackData;
    fCallbackObject = callbackObject;
    moveToThread(qApp->instance()->thread());
    qApp->processEvents();
    connect(this, SIGNAL(go()), this, SLOT(run()));
    emit go();
}

void NotificationThread::run()
{
    NotificationWidget *nw = new NotificationWidget(fMessage, fInfo);
    nw->fCallback = fCallback;
    nw->fCallbackData = fCallbackData;
    nw->fCallbackObject = fCallbackObject;
    if (fCallback != nullptr) {
        connect(nw, SIGNAL(onClose(QVariant)), fCallbackObject, fCallback);
    }
    nw->adjustSize();
    nw->setWindowOpacity(0.5);
    QRect rect = nw->rect();
    int r = 10;
    QRegion region;
    // middle and borders
    region += rect.adjusted(r, 0, -r, 0);
    region += rect.adjusted(0, r, 0, -r);
    // top left
    QRect corner(rect.topLeft(), QSize(r * 2, r * 2));
    region += QRegion(corner, QRegion::Ellipse);
    // top right
    corner.moveTopRight(rect.topRight());
    region += QRegion(corner, QRegion::Ellipse);
    // bottom left
    corner.moveBottomLeft(rect.bottomLeft());
    region += QRegion(corner, QRegion::Ellipse);
    // bottom right
    corner.moveBottomRight(rect.bottomRight());
    region += QRegion(corner, QRegion::Ellipse);
    nw->setMask(region);
    rect = qApp->screens().at(0)->geometry();
    int y = 0;
    for (NotificationWidget *w : NotificationWidget::fWidgetList) {
        y += w->height() + 5;
    }
    nw->move(rect.width() - 10 - nw->width(), y + 5);
    NotificationWidget::fWidgetList.append(nw);
    nw->fTimer.start(50);
    nw->show();
}

NotificationWidget::NotificationWidget(const QString &message, int info, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool),
    ui(new Ui::NotificationWidget)
{
    ui->setupUi(this);
    QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(this);
    eff->setOffset(0, 0);
    eff->setBlurRadius(5.0);
    eff->setColor(Qt::red);
    ui->lbMessage->setGraphicsEffect(eff);
    ui->lbMessage->setText(message);
    ui->lbDataTime->setText(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    switch (info) {
        case 1:
            ui->lbInfo->setPixmap(QPixmap(":/warning.png"));
            break;
    }
    connect( &fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fClose = false;
}

NotificationWidget::~NotificationWidget()
{
    fWidgetList.removeOne(this);
    delete ui;
    compact();
}

void NotificationWidget::showMessage(const QString &message, const QVariant &callBackData, char *callBack,
                                     QObject *callbackObject, int info)
{
    NotificationThread *nt = new NotificationThread(message, callBackData, callBack, callbackObject, info);
    nt->deleteLater();
}

void NotificationWidget::compact()
{
    QRect rect = qApp->screens().at(0)->geometry();
    int pos = 0;
    for (NotificationWidget *w : fWidgetList) {
        int y = 0;
        for (NotificationWidget *ww : fWidgetList) {
            if (fWidgetList.indexOf(ww) > pos) {
                break;
            }
            if (ww != w) {
                y += ww->height() + 5;
            }
        }
        w->move(rect.width() - 10 - w->width(), y + 5);
        pos++;
    }
}

void NotificationWidget::timeout()
{
    if (fClose) {
        if (windowOpacity() > 0) {
            setWindowOpacity(windowOpacity() - 0.3);
        } else {
            if (fCallback) {
                emit onClose(fCallbackData);
            }
            deleteLater();
        }
        return;
    }
    if (windowOpacity() < 1) {
        setWindowOpacity(windowOpacity() + 0.05);
    } else {
        fTimer.stop();
    }
}

void NotificationWidget::on_btnClose_clicked()
{
    fClose = true;
    fTimer.start(50);
}
