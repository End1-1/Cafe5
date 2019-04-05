#include "c5toolbarwidget.h"
#include "ui_c5toolbarwidget.h"
#include <QProcess>

C5ToolBarWidget::C5ToolBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5ToolBarWidget)
{
    ui->setupUi(this);
}

C5ToolBarWidget::~C5ToolBarWidget()
{
    delete ui;
}

void C5ToolBarWidget::setUpdateButtonVisible(bool v)
{
    ui->btnUpdate->setVisible(v);
}

void C5ToolBarWidget::on_btnUpdate_clicked()
{
    QProcess p;
    p.startDetached(qApp->applicationDirPath() + "/updater.exe");
    qApp->quit();
}
