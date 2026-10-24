#include "dlgordererror.h"
#include "ui_dlgordererror.h"

#include <QPaintEvent>
#include <QPainter>

DlgOrderError::DlgOrderError(const QString &message, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DlgOrderError)
{
    ui->setupUi(this);
    ui->lblMessage->setText(message);
    connect(ui->btnOk, &QPushButton::clicked, this, &DlgOrderError::acknowledged);
}

DlgOrderError::~DlgOrderError()
{
    delete ui;
}

void DlgOrderError::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(26, 26, 26, 230));
}
