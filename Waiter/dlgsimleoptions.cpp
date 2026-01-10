#include "dlgsimleoptions.h"
#include "ui_dlgsimleoptions.h"
#include <QToolButton>

DlgSimleOptions::DlgSimleOptions(const QStringList &title, const QList<int>& values)
    : C5Dialog(nullptr),
      ui(new Ui::DlgSimleOptions)
{
    ui->setupUi(this);

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
        ui->vl->addWidget(btn);
    }
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
