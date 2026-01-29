#include "c5codenameselector.h"
#include "ui_c5codenameselector.h"

C5CodeNameSelector::C5CodeNameSelector(QWidget *parent)
    : QFrame(parent), ui(new Ui::C5CodeNameSelector)
{
    ui->setupUi(this);
    setFrameStyle(QFrame::NoFrame);
    setFrameShape(QFrame::NoFrame);
    setLineWidth(0);
    setMidLineWidth(0);
    setStyleSheet("QFrame { border: 0px; }");
    ui->lbName->clear();
}

C5CodeNameSelector::~C5CodeNameSelector() { delete ui; }

void C5CodeNameSelector::setCodeAndName(int code, const QString &name)
{
    ui->leCode->setInteger(code);
    ui->lbName->setText(name);
}

void C5CodeNameSelector::setName(const QString &name)
{
    ui->lbName->setText(name);
}

QString C5CodeNameSelector::name() const
{
    return ui->lbName->text();
}

QString C5CodeNameSelector::selectorName() const
{
    return mSelectorName;
}

void C5CodeNameSelector::setSelectorName(const QString &s)
{
    if(mSelectorName == s)
        return;

    mSelectorName = s;
    ui->lbSelectorName->setText(s);
    emit selectorNameChanged();
}

QPoint C5CodeNameSelector::getPosition() const
{
    QPoint p = ui->leCode->mapToGlobal(QPoint(0, ui->leCode->height()));
    return p;
}

int C5CodeNameSelector::value()
{
    return ui->leCode->getInteger();
}

void C5CodeNameSelector::on_btnOpenSelector_clicked()
{
    if(selectorCallback)
        selectorCallback(this);
}
