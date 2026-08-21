#include "ce5goodsgroup.h"
#include "ui_ce5goodsgroup.h"
#include "c5editor.h"
#include "c5cache.h"
#include "c5database.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5user.h"
#include <QBuffer>
#include <QColorDialog>
#include <QFileDialog>
#include <QMenu>

CE5GoodsGroup::CE5GoodsGroup(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5GoodsGroup)
{
    ui->setupUi(this);
    ui->leParentGroup->setSelector(ui->leParentGroupName, cache_goods_group);
    connect(ui->leColor, &C5LineEditWithSelector::doubleClicked, this, &CE5GoodsGroup::setColor);
    connect(ui->lbImg, &QLabel::customContextMenuRequested, this, &CE5GoodsGroup::on_lbImg_customContextMenuRequested);
}

CE5GoodsGroup::~CE5GoodsGroup()
{
    delete ui;
}

void CE5GoodsGroup::setId(int id)
{
    ui->leCode->setEnabled(false);
    if (id == 0) {
        return;
    }

    ui->leCode->setInteger(id);
    fHttp->createHttpQueryLambda(
        QStringLiteral("/engine/v2/officen/goodsgroup/get"),
        QJsonObject{{QStringLiteral("f_id"), id}},
        [this](const QJsonObject &jdoc) { openResponse(jdoc); },
        [](const QJsonObject &) { return false; });
}

bool CE5GoodsGroup::checkData(QString &err)
{
    CE5Editor::checkData(err);
    if (ui->lineEdit_2->text().trimmed().isEmpty()) {
        err += tr("Name") + " " + tr("cannot be empty") + "\r\n";
    }
    return err.isEmpty();
}

bool CE5GoodsGroup::save(QString &err, QList<QMap<QString, QVariant>> &data)
{
    Q_UNUSED(data);
    if (!checkData(err)) {
        return false;
    }

    fHttp->createHttpQueryLambda(
        QStringLiteral("/engine/v2/officen/goodsgroup/save"),
        makeSaveJson(),
        [this](const QJsonObject &jdoc) { saveResponse(jdoc); },
        [](const QJsonObject &) { return false; });
    err = QStringLiteral("json");
    return true;
}

bool CE5GoodsGroup::isOnline()
{
    return true;
}

void CE5GoodsGroup::clear()
{
    CE5Editor::clear();
    ui->leCode->clear();
    ui->leCode->setEnabled(false);
    fPendingImage.clear();
    fImageChanged = false;
    fRemoveImage = false;
    ui->chOnlineSale->setChecked(false);
    ui->lbImg->setPixmap(QPixmap());
    ui->lbImg->setText(tr("Right click to select image"));
    ui->leImageUUID->setText(C5Database::uuid());
}

void CE5GoodsGroup::setColor()
{
    QColor initColor = QColor::fromRgb(ui->leColor->color());
    int color = QColorDialog::getColor(initColor, this, tr("Background color")).rgb();
    ui->leColor->setColor(color);
    ui->leColor->setInteger(color);
}

void CE5GoodsGroup::applyGroup(const QJsonObject &group, const QString &imageBase64)
{
    ui->leCode->setInteger(group.value(QStringLiteral("f_id")).toInt());
    ui->leClass->setInteger(group.value(QStringLiteral("f_class")).toInt(1));
    ui->leParentGroup->setValue(group.value(QStringLiteral("f_parent")).toInt());
    ui->leParentGroupName->setText(group.value(QStringLiteral("f_parent_name")).toString());
    ui->leColor->setInteger(group.value(QStringLiteral("f_color")).toInt(-1));
    if (ui->leColor->getInteger() >= 0) {
        ui->leColor->setColor(ui->leColor->getInteger());
    }
    ui->lineEdit_2->setText(group.value(QStringLiteral("f_name")).toString());
    ui->lineEdit_3->setInteger(group.value(QStringLiteral("f_taxdept")).toInt(1));
    ui->lineEdit_4->setText(group.value(QStringLiteral("f_adgcode")).toString());
    ui->lineEdit_5->setDouble(group.value(QStringLiteral("f_chargevalue")).toDouble());
    ui->lineEdit_6->setInteger(group.value(QStringLiteral("f_order")).toInt());
    {
        const QJsonValue online = group.value(QStringLiteral("f_online_sale"));
        const bool checked = online.isBool() ? online.toBool()
                                             : (online.toVariant().toInt() > 0);
        ui->chOnlineSale->setChecked(checked);
    }

    const QString imageId = group.value(QStringLiteral("f_image")).toString();
    if (!imageId.isEmpty()) {
        ui->leImageUUID->setText(imageId);
    } else if (ui->leImageUUID->text().isEmpty()) {
        ui->leImageUUID->setText(C5Database::uuid());
    }

    fPendingImage.clear();
    fImageChanged = false;
    fRemoveImage = false;
    ui->lbImg->setPixmap(QPixmap());
    if (!imageBase64.isEmpty()) {
        QPixmap pix;
        pix.loadFromData(QByteArray::fromBase64(imageBase64.toLatin1()));
        ui->lbImg->setPixmap(pix.scaled(ui->lbImg->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        ui->lbImg->setText(tr("Right click to select image"));
    }
}

void CE5GoodsGroup::openResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
        C5Message::error(jdoc.value(QStringLiteral("message")).toString());
        return;
    }
    applyGroup(jdoc.value(QStringLiteral("group")).toObject(), jdoc.value(QStringLiteral("image")).toString());
}

void CE5GoodsGroup::saveResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
        C5Message::error(jdoc.value(QStringLiteral("message")).toString());
        return;
    }

    const QJsonObject group = jdoc.value(QStringLiteral("group")).toObject();
    applyGroup(group, jdoc.value(QStringLiteral("image")).toString());
    C5Cache::cache(cache_goods_group)->refresh();

    if(C5Editor *editor = qobject_cast<C5Editor *>(fEditor)) {
        QMap<QString, QVariant> row;
        row.insert(QStringLiteral("f_id"), group.value(QStringLiteral("f_id")).toInt());
        editor->appendResultRow(row);
    }

    emit Accept();
}

QJsonObject CE5GoodsGroup::makeSaveJson() const
{
    auto intOrNull = [](int v) -> QJsonValue {
        return v == 0 ? QJsonValue(QJsonValue::Null) : QJsonValue(v);
    };

    QJsonObject jo;
    const int id = ui->leCode->getInteger();
    if (id > 0) {
        jo.insert(QStringLiteral("f_id"), id);
    } else {
        jo.insert(QStringLiteral("f_id"), QJsonValue(QJsonValue::Null));
    }
    jo.insert(QStringLiteral("f_parent"), intOrNull(ui->leParentGroup->getInteger()));
    jo.insert(QStringLiteral("f_class"), ui->leClass->getInteger());
    const int color = ui->leColor->getInteger();
    jo.insert(QStringLiteral("f_color"), color < 0 ? QJsonValue(QJsonValue::Null) : QJsonValue(color));
    jo.insert(QStringLiteral("f_name"), ui->lineEdit_2->text().trimmed());
    jo.insert(QStringLiteral("f_taxdept"), ui->lineEdit_3->getInteger());
    jo.insert(QStringLiteral("f_adgcode"), ui->lineEdit_4->text());
    jo.insert(QStringLiteral("f_chargevalue"), ui->lineEdit_5->getDouble());
    jo.insert(QStringLiteral("f_order"), intOrNull(ui->lineEdit_6->getInteger()));
    jo.insert(QStringLiteral("f_image"), ui->leImageUUID->text());
    jo.insert(QStringLiteral("f_online_sale"), ui->chOnlineSale->isChecked() ? 1 : 0);

    if (fRemoveImage) {
        jo.insert(QStringLiteral("f_remove_image"), 1);
    } else if (fImageChanged && !fPendingImage.isEmpty()) {
        jo.insert(QStringLiteral("f_image_data"), QString::fromLatin1(fPendingImage.toBase64()));
    }

    return jo;
}

void CE5GoodsGroup::uploadImage()
{
    if (ui->leCode->getInteger() == 0) {
        if (C5Message::question(tr("You should to save before upload an image")) != QDialog::Accepted) {
            return;
        }
        QString err;
        QList<QMap<QString, QVariant>> data;
        if (!save(err, data)) {
            if (err != QStringLiteral("json")) {
                C5Message::error(err);
            }
            return;
        }
    }

    const QString fn = QFileDialog::getOpenFileName(this, tr("Image"), QString(), QStringLiteral("*.jpg;*.png;*.bmp"));
    if (fn.isEmpty()) {
        return;
    }

    QImage img;
    if (!img.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }

    const bool hasAlpha = img.hasAlphaChannel();
    const QImage::Format targetFormat = hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32;
    QImage previewImg = img.convertToFormat(targetFormat);
    ui->lbImg->setPixmap(QPixmap::fromImage(previewImg).scaled(ui->lbImg->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QByteArray ba;
    do {
        previewImg = previewImg.scaled(
            previewImg.width() * 0.8,
            previewImg.height() * 0.8,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
        ba.clear();
        QBuffer buff(&ba);
        buff.open(QIODevice::WriteOnly);
        if (hasAlpha) {
            previewImg.save(&buff, "PNG");
        } else {
            // For smaller payloads: JPEG is enough when there's no alpha channel.
            previewImg.save(&buff, "JPG", 85);
        }
    } while (ba.size() > 100000 && previewImg.width() > 10 && previewImg.height() > 10);

    fPendingImage = ba;
    fImageChanged = true;
    fRemoveImage = false;
}

void CE5GoodsGroup::removeImage()
{
    if (C5Message::question(tr("Remove image")) != QDialog::Accepted) {
        return;
    }
    fPendingImage.clear();
    fImageChanged = false;
    fRemoveImage = true;
    ui->lbImg->setPixmap(QPixmap());
    ui->lbImg->setText(tr("Right click to select image"));
}

void CE5GoodsGroup::on_lbImg_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(QIcon(QStringLiteral(":/new.png")), tr("Upload image"), this, &CE5GoodsGroup::uploadImage);
    m->addAction(QIcon(QStringLiteral(":/delete.png")), tr("Remove image"), this, &CE5GoodsGroup::removeImage);
    m->popup(ui->lbImg->mapToGlobal(pos));
}
