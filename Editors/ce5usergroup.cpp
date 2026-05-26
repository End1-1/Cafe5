#include "ce5usergroup.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "c5message.h"
#include "ui_ce5usergroup.h"

namespace {

QJsonObject salaryFieldsToJson(const Ui::CE5UserGroup *ui)
{
    QJsonObject jo;
    jo.insert(QStringLiteral("f_fixed"), ui->leFixed->getDouble());
    jo.insert(QStringLiteral("f_min"), ui->leMin->getDouble());
    jo.insert(QStringLiteral("f_max"), ui->leMax->getDouble());
    jo.insert(QStringLiteral("f_dep"), ui->leValDep->getInteger());
    jo.insert(QStringLiteral("f_skip_amount"), ui->leSkipAmount->getDouble());
    jo.insert(QStringLiteral("f_count_working_time"), ui->chCountWorkingTime->isChecked());
    return jo;
}

void applySalaryJsonToFields(const QJsonObject &jo, Ui::CE5UserGroup *ui)
{
    ui->leFixed->setDouble(jo.value(QStringLiteral("f_fixed")).toDouble());
    ui->leMin->setDouble(jo.value(QStringLiteral("f_min")).toDouble());
    ui->leMax->setDouble(jo.value(QStringLiteral("f_max")).toDouble());
    ui->leValDep->setInteger(jo.value(QStringLiteral("f_dep")).toInt());
    ui->leSkipAmount->setDouble(jo.value(QStringLiteral("f_skip_amount")).toDouble());
    ui->chCountWorkingTime->setChecked(jo.value(QStringLiteral("f_count_working_time")).toBool());
}

} // namespace

CE5UserGroup::CE5UserGroup(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5UserGroup)
{
    ui->setupUi(this);
}

CE5UserGroup::~CE5UserGroup()
{
    delete ui;
}

void CE5UserGroup::clear()
{
    CE5Editor::clear();
    ui->leFixed->clear();
    ui->leMin->clear();
    ui->leMax->clear();
    ui->leValDep->clear();
    ui->leSkipAmount->clear();
    ui->chCountWorkingTime->setChecked(false);
}

void CE5UserGroup::setId(int id)
{
    CE5Editor::setId(id);
    QJsonObject jo = QJsonDocument::fromJson(ui->teData->toPlainText().toUtf8()).object();
    applySalaryJsonToFields(jo, ui);
}

bool CE5UserGroup::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    QJsonObject jo = QJsonDocument::fromJson(ui->teData->toPlainText().toUtf8()).object();
    const QJsonObject salary = salaryFieldsToJson(ui);
    for (auto it = salary.begin(); it != salary.end(); ++it) {
        jo.insert(it.key(), it.value());
    }
    ui->teData->setPlainText(QString::fromUtf8(
        QJsonDocument(jo).toJson(QJsonDocument::Compact)));

    if (!CE5Editor::save(err, data)) {
        C5Message::error(err);
        return false;
    }
    return true;
}
