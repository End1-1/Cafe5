#include "dlgsimleoptions.h"
#include "ui_dlgsimleoptions.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QToolButton>
#include "c5user.h"

DlgSimleOptions::DlgSimleOptions(const QStringList &title, const QList<int> &values)
    : DlgSimleOptions(title, values, nullptr, nullptr)
{
}

DlgSimleOptions::DlgSimleOptions(const QStringList &title, const QList<int> &values, C5User *user, QWidget *parent)
    : DlgSimleOptions(title, values, 1, user, parent)
{
}

DlgSimleOptions::DlgSimleOptions(const QStringList &title, const QList<int> &values, int columns, C5User *user, QWidget *parent)
    : C5Dialog(user, parent),
      ui(new Ui::DlgSimleOptions)
{
    ui->setupUi(this);
    setWindowFlags((windowFlags() | Qt::Popup) & ~Qt::WindowContextHelpButtonHint);
    const int cols = qMax(1, columns);
    auto *container = new QWidget(this);
    auto *gl = new QGridLayout(container);
    gl->setSpacing(1);
    gl->setContentsMargins(0, 0, 0, 0);

    for(int c = 0; c < cols; ++c) {
        gl->setColumnStretch(c, 1);
    }

    for(int i = 0; i < title.size(); i++) {
        auto *btn = new QToolButton();
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setFixedHeight(50);
        btn->setText(title.at(i));
        int value = values.at(i);
        connect(btn, &QToolButton::clicked, this, [this, value]() {
            mResult = value;
            accept();
        });
        gl->addWidget(btn, i / cols, i % cols);
    }

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidget(container);
    ui->vl->addWidget(scroll);

    auto *btnCancel = new QToolButton(this);
    btnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnCancel->setFixedHeight(50);
    btnCancel->setText(tr("Cancel"));
    connect(btnCancel, &QToolButton::clicked, this, [this]() {
        mResult = 0;
        reject();
    });
    ui->vl->addWidget(btnCancel);
}

DlgSimleOptions::~DlgSimleOptions() { delete ui; }

int DlgSimleOptions::exec()
{
    C5Dialog::exec();
    return mResult;
}

void DlgSimleOptions::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);
    adjustSize();
}
