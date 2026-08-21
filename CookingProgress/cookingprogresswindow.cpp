#include "cookingprogresswindow.h"
#include "ui_cookingprogresswindow.h"

#include <QApplication>
#include <QDateTime>
#include <QFrame>
#include <QJsonObject>
#include <QLabel>
#include <QShowEvent>
#include <QVBoxLayout>
#include "c5user.h"
#include "ninterface.h"

CookingProgressWindow::CookingProgressWindow(C5User *user, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::CookingProgressWindow)
{
    ui->setupUi(this);
    ui->horizontalLayoutBoards->setStretch(0, 1);
    ui->horizontalLayoutBoards->setStretch(1, 1);
    setStyleSheet(QStringLiteral(
        "CookingProgressWindow, QDialog, QGroupBox, QScrollArea, QScrollArea > QWidget > QWidget {"
        "  background-color: #ffffff;"
        "  color: #000000;"
        "}"
        "QGroupBox::title {"
        "  color: #000000;"
        "  subcontrol-origin: margin;"
        "  left: 8px;"
        "}"
        "QLabel {"
        "  background-color: transparent;"
        "  color: #000000;"
        "}"
        "QPushButton {"
        "  background-color: #ffffff;"
        "  color: #000000;"
        "  border: 1px solid #cccccc;"
        "  border-radius: 4px;"
        "  padding: 4px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #f5f5f5;"
        "}"
    ));
#ifndef QT_DEBUG
    setWindowState(windowState() | Qt::WindowFullScreen);
#else
    setWindowState(windowState() | Qt::WindowMaximized);
#endif
    connect(&mPollTimer, &QTimer::timeout, this, &CookingProgressWindow::pollQueue);
    mPollTimer.setInterval(3000);
}

CookingProgressWindow::~CookingProgressWindow()
{
    mPollTimer.stop();
    delete ui;
}

void CookingProgressWindow::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);
    if(!mPollTimer.isActive()) {
        pollQueue();
        mPollTimer.start();
    }
}

void CookingProgressWindow::on_btnExit_clicked()
{
    mPollTimer.stop();
    accept();
}

void CookingProgressWindow::pollQueue()
{
    ui->lbClock->setText(QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy HH:mm:ss")));
    NInterface::query1(QStringLiteral("/engine/v2/waiter/in-progress/get"),
                       mUser->mSessionKey,
                       this,
                       {},
                       [this](const QJsonObject &jdoc) {
                           mOrders = jdoc.value(QStringLiteral("data")).toArray();
                           rebuildBoards();
                       });
}

void CookingProgressWindow::clearLayout(QVBoxLayout *layout)
{
    if(!layout) {
        return;
    }
    while(layout->count() > 0) {
        QLayoutItem *item = layout->takeAt(0);
        if(!item) {
            break;
        }
        if(QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

void CookingProgressWindow::rebuildBoards()
{
    clearLayout(ui->vlPreparingCards);
    clearLayout(ui->vlReadyCards);

    QSet<QString> readyNow;
    bool playReadySound = false;

    for(const auto &ov : mOrders) {
        const QJsonObject order = ov.toObject();
        const QJsonArray lines = order.value(QStringLiteral("lines")).toArray();
        if(lines.isEmpty()) {
            continue;
        }
        bool allReady = true;
        for(const auto &lv : lines) {
            const int st = lv.toObject().value(QStringLiteral("f_status")).toInt();
            if(st != 3) {
                allReady = false;
                break;
            }
        }
        if(allReady) {
            const QString headerId = order.value(QStringLiteral("f_header_id")).toString();
            if(!headerId.isEmpty()) {
                readyNow.insert(headerId);
                if(mReadyTrackingStarted && !mReadyOrderIds.contains(headerId)) {
                    playReadySound = true;
                }
            }
            ui->vlReadyCards->addWidget(makeOrderCard(order, true));
        } else {
            ui->vlPreparingCards->addWidget(makeOrderCard(order, false));
        }
    }

    mReadyOrderIds = readyNow;
    mReadyTrackingStarted = true;
    if(playReadySound) {
        QApplication::beep();
    }

    ui->vlPreparingCards->addStretch(1);
    ui->vlReadyCards->addStretch(1);
}

QWidget *CookingProgressWindow::makeOrderCard(const QJsonObject &order, bool readySide)
{
    auto *frame = new QFrame(this);
    frame->setObjectName(QStringLiteral("orderCard"));
    frame->setFrameShape(QFrame::NoFrame);
    frame->setStyleSheet(readySide
                             ? QStringLiteral("#orderCard { background:#d8f5d0; border:2px solid #2e7d32; border-radius:8px; }")
                             : QStringLiteral("#orderCard { background:#fff3cd; border:2px solid #f0ad4e; border-radius:8px; }"));
    auto *lay = new QVBoxLayout(frame);
    lay->setContentsMargins(16, 16, 16, 16);
    lay->setSpacing(8);

    auto *lbOrder = new QLabel(order.value(QStringLiteral("f_order_prefix")).toString(), frame);
    lbOrder->setFrameShape(QFrame::NoFrame);
    QFont f = lbOrder->font();
    f.setPointSize(28);
    f.setBold(true);
    lbOrder->setFont(f);
    lbOrder->setAlignment(Qt::AlignCenter);
    lay->addWidget(lbOrder);

    const auto formatWhen = [](const QString &raw) -> QString {
        const QString s = raw.trimmed();
        if(s.isEmpty()) {
            return QStringLiteral("—");
        }
        QDateTime dt = QDateTime::fromString(s, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        if(!dt.isValid()) {
            dt = QDateTime::fromString(s, Qt::ISODate);
        }
        if(dt.isValid()) {
            return dt.toString(QStringLiteral("HH:mm"));
        }
        if(s.contains(QLatin1Char(' '))) {
            return s.section(QLatin1Char(' '), -1).left(5);
        }
        return s.left(5);
    };

    const QString started = formatWhen(order.value(QStringLiteral("f_started_at")).toString());
    QString ended = formatWhen(order.value(QStringLiteral("f_estimated_end")).toString());
    if(readySide) {
        QString latestReady;
        for(const auto &lv : order.value(QStringLiteral("lines")).toArray()) {
            const QString ra = lv.toObject().value(QStringLiteral("f_ready_at")).toString().trimmed();
            if(ra.isEmpty()) {
                continue;
            }
            if(latestReady.isEmpty() || ra > latestReady) {
                latestReady = ra;
            }
        }
        if(!latestReady.isEmpty()) {
            ended = formatWhen(latestReady);
        }
    }

    auto *lbStart = new QLabel(tr("Start") + QStringLiteral(": ") + started, frame);
    lbStart->setFrameShape(QFrame::NoFrame);
    lbStart->setAlignment(Qt::AlignCenter);
    lbStart->setStyleSheet(QStringLiteral("font-size: 18px;"));
    lay->addWidget(lbStart);

    auto *lbEnd = new QLabel(tr("Estimated end") + QStringLiteral(": ") + ended, frame);
    lbEnd->setFrameShape(QFrame::NoFrame);
    lbEnd->setAlignment(Qt::AlignCenter);
    lbEnd->setStyleSheet(QStringLiteral("font-size: 18px;"));
    lay->addWidget(lbEnd);

    return frame;
}
