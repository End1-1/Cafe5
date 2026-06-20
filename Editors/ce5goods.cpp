#include "ce5goods.h"
#include <QBuffer>
#include <QClipboard>
#include <QCompleter>
#include <QDateTime>
#include <QFileDialog>
#include <QFontDatabase>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QPaintEngine>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QCheckBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStringListModel>
#include "barcode.h"
#include "c5cache.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5editor.h"
#include "c5database.h"
#include "c5htmlprint.h"
#include "c5message.h"
#include "c5replacecharacter.h"
#include "c5selector.h"
#include "c5storebarcode.h"
#include "c5user.h"
#include "c5utils.h"
#include "ce5goodsgroup.h"
#include "ce5goodsmodel.h"
#include "ce5goodsunit.h"
#include "ce5partner.h"
#include "dict_goods_types.h"
#include "ui_ce5goods.h"
#include <stdexcept>

static int fLastGroup = 0;
static int fLastUnit = 0;

static QByteArray decodeBase64ImagePayload(const QString &payload)
{
    QString text = payload.trimmed();
    if(text.isEmpty()) {
        return {};
    }

    // Supports "data:image/png;base64,...." style payload.
    if(text.startsWith(QStringLiteral("data:"), Qt::CaseInsensitive)) {
        const int comma = text.indexOf(QLatin1Char(','));
        if(comma >= 0) {
            text = text.mid(comma + 1);
        }
    }

    // DB / transport may insert line breaks; strip common whitespace before decode.
    text.remove(QLatin1Char(' '));
    text.remove(QLatin1Char('\n'));
    text.remove(QLatin1Char('\r'));
    text.remove(QLatin1Char('\t'));

    return QByteArray::fromBase64(text.toLatin1());
}

static void setLabelPixmapScaled(QLabel *label, const QPixmap &pm)
{
    if(!label || pm.isNull()) {
        return;
    }

    const QSize target = label->size();
    if(target.isEmpty() || target.width() <= 1 || target.height() <= 1) {
        label->setPixmap(pm);
        return;
    }

    label->setPixmap(pm.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

static bool loadGoodsImageToLabel(int goodsId, QLabel *label, QString *outBase64 = nullptr)
{
    if(goodsId <= 0 || !label) {
        return false;
    }

    C5Database db;
    db[":f_id"] = goodsId;
    db.exec("select f_image from c_goods_images where f_id=:f_id");
    if(!db.nextRow()) {
        return false;
    }

    const QString imagePayload = db.getValue("f_image").toString();
    const QByteArray rawFromFImage = decodeBase64ImagePayload(imagePayload);
    QPixmap p;
    if(!rawFromFImage.isEmpty() && p.loadFromData(rawFromFImage)) {
        setLabelPixmapScaled(label, p);
        if(outBase64) {
            *outBase64 = imagePayload;
        }
        return true;
    }

    return false;
}

static QJsonValue dynamicAttributeToJson(const QString &value, const QString &measurement, const QString &priceText)
{
    const QString trimmedValue = value.trimmed();
    const QString trimmedMeas = measurement.trimmed();
    bool priceOk = false;
    const double price = priceText.trimmed().replace(QLatin1Char(','), QLatin1Char('.')).toDouble(&priceOk);
    const bool hasMeas = !trimmedMeas.isEmpty();

    if (!hasMeas && !priceOk) {
        return trimmedValue;
    }

    QJsonObject obj;
    obj.insert(QStringLiteral("value"), trimmedValue);
    if (hasMeas) {
        obj.insert(QStringLiteral("measurement"), trimmedMeas);
    }
    if (priceOk) {
        obj.insert(QStringLiteral("price"), price);
    }
    return obj;
}

static void readDynamicAttributeFromJson(
    const QJsonValue &json,
    QString *value,
    QString *measurement,
    QString *priceText)
{
    if (json.isObject()) {
        const QJsonObject obj = json.toObject();
        *value = obj.value(QStringLiteral("value")).toString();
        *measurement = obj.value(QStringLiteral("measurement")).toString();
        if (obj.contains(QStringLiteral("price"))) {
            *priceText = QString::number(obj.value(QStringLiteral("price")).toDouble());
        } else {
            priceText->clear();
        }
        return;
    }
    *value = json.toString();
    measurement->clear();
    priceText->clear();
}

static QString modificatorSelectorQuery()
{
    return QString(R"(
        select g.f_id as `%1`, gg.f_name as `%2`, g.f_name as `%3`, u.f_name as `%4`,
               g.f_scancode as `%5`, gpr.f_price1 as `%6`
        from c_goods g
        left join c_groups gg on gg.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1
        where g.f_type=%7 and coalesce(g.f_enabled, 1)=1
        order by g.f_name
    )")
        .arg(QObject::tr("Code").toLower(),
             QObject::tr("Group").toLower(),
             QObject::tr("Name").toLower(),
             QObject::tr("Unit").toLower(),
             QObject::tr("Scancode").toLower(),
             QObject::tr("Price").toLower())
        .arg(GOODS_TYPE_MODIFICATOR);
}

static QString menuDishSelectorQuery()
{
    return QString(R"(
        select mm.f_dish as `%1`, gr.f_name as `%2`, g.f_name as `%3`, u.f_name as `%4`,
               g.f_scancode as `%5`, gpr.f_price1 as `%6`
        from c_menu mm
        inner join c_goods g on g.f_id = mm.f_dish
        left join c_groups gr on gr.f_id = g.f_group
        left join c_units u on u.f_id = g.f_unit
        left join c_goods_prices gpr on gpr.f_goods = g.f_id and gpr.f_currency = 1
        where mm.f_menu = 1 and mm.f_state = 1 and coalesce(g.f_enabled, 1) = 1
        order by g.f_name
    )")
        .arg(QObject::tr("Code").toLower(),
             QObject::tr("Group").toLower(),
             QObject::tr("Name").toLower(),
             QObject::tr("Unit").toLower(),
             QObject::tr("Scancode").toLower(),
             QObject::tr("Price").toLower());
}

static QJsonArray relatedItemsFromTable(C5TableWidget *tbl)
{
    QJsonArray items;
    if(!tbl) {
        return items;
    }

    for(int i = 0; i < tbl->rowCount(); ++i) {
        const int dishId = tbl->getInteger(i, 0);
        if(dishId <= 0) {
            continue;
        }
        QJsonObject item;
        item.insert(QStringLiteral("f_id"), dishId);
        item.insert(QStringLiteral("f_name"), tbl->getString(i, 1).trimmed());
        items.append(item);
    }
    return items;
}

static QJsonObject parseGoodsFData(const QJsonValue &rawFData)
{
    if(rawFData.isObject()) {
        return rawFData.toObject();
    }

    const QString text = rawFData.toString().trimmed();
    if(text.isEmpty()) {
        return {};
    }

    const QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
    return doc.isObject() ? doc.object() : QJsonObject();
}

static QJsonArray modificatorsFromTable(C5TableWidget *tbl)
{
    QJsonArray modificators;
    if(!tbl) {
        return modificators;
    }

    for(int i = 0; i < tbl->rowCount(); ++i) {
        C5TableWidgetItem *idItem = tbl->item(i, 0);
        C5TableWidgetItem *nameItem = tbl->item(i, 1);
        C5LineEdit *priceEdit = tbl->lineEdit(i, 2);
        C5CheckBox *requiredBox = tbl->checkBox(i, 3);
        if(!idItem || !nameItem || !priceEdit || !requiredBox) {
            continue;
        }

        int modificatorId = idItem->data(Qt::EditRole).toInt();
        if(modificatorId <= 0) {
            modificatorId = idItem->text().trimmed().toInt();
        }
        if(modificatorId <= 0) {
            continue;
        }

        QJsonObject item;
        item.insert(QStringLiteral("f_id"), modificatorId);
        item.insert(QStringLiteral("f_name"), nameItem->text().trimmed());
        item.insert(QStringLiteral("f_price"), priceEdit->getDouble());
        item.insert(QStringLiteral("f_required"), requiredBox->isChecked());
        modificators.append(item);
    }

    return modificators;
}

static void initDynamicAttributesTable(QTableWidget *tbl)
{
    if(!tbl) {
        return;
    }

    constexpr int kRows = 2;
    constexpr int kColAttribute = 0;
    constexpr int kColValue = 1;
    constexpr int kColMeasurement = 2;
    constexpr int kColPrice = 3;
    constexpr int kRequiredCols = 4; // Attribute / Value / Measurement / Price

    if(tbl->columnCount() < kRequiredCols) {
        tbl->setColumnCount(kRequiredCols);
    }
    tbl->setRowCount(kRows);

    const QString kType = QStringLiteral("Type");
    const QString kSize = QStringLiteral("Size");

    for(int r = 0; r < kRows; ++r) {
        const QString attrName = (r == 0) ? kType : kSize;

        QTableWidgetItem *attrItem = tbl->item(r, kColAttribute);
        if(!attrItem) {
            attrItem = new QTableWidgetItem();
            tbl->setItem(r, kColAttribute, attrItem);
        }
        attrItem->setText(attrName);
        attrItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        QTableWidgetItem *valItem = tbl->item(r, kColValue);
        if(!valItem) {
            valItem = new QTableWidgetItem();
            tbl->setItem(r, kColValue, valItem);
        }
        valItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

        QTableWidgetItem *measItem = tbl->item(r, kColMeasurement);
        if(!measItem) {
            measItem = new QTableWidgetItem();
            tbl->setItem(r, kColMeasurement, measItem);
        }
        measItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        if (r == 0) {
            measItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            measItem->setText(QString());
        }

        QTableWidgetItem *priceItem = tbl->item(r, kColPrice);
        if(!priceItem) {
            priceItem = new QTableWidgetItem();
            tbl->setItem(r, kColPrice, priceItem);
        }
        priceItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }
}

CE5Goods::CE5Goods(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Goods)
{
    ui->setupUi(this);

    // Hardcoded required dynamic attributes.
    initDynamicAttributesTable(ui->tblDynamicAttributes);
    ui->tblModificators->setColumnWidths(4, 60, 200, 80, 70);
    ui->tblModificators->horizontalHeader()->setStretchLastSection(false);
    ui->tblModificators->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tblRelatedDrinks->setColumnWidths(2, 60, 300);
    ui->tblRelatedDrinks->horizontalHeader()->setStretchLastSection(false);
    ui->tblRelatedDrinks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tblRelatedOther->setColumnWidths(2, 60, 300);
    ui->tblRelatedOther->horizontalHeader()->setStretchLastSection(false);
    ui->tblRelatedOther->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    {
        constexpr QSize dietaryIconSize(24, 24);
        const auto limitDietaryIcon = [&](QCheckBox *cb) {
            if (!cb) {
                return;
            }
            cb->setIconSize(dietaryIconSize);
        };
        limitDietaryIcon(ui->chGlutenFree);
        limitDietaryIcon(ui->chVegetarian);
        limitDietaryIcon(ui->chVegan);
        limitDietaryIcon(ui->chNoGmo);
        limitDietaryIcon(ui->chNoLactose);
        limitDietaryIcon(ui->chNoSugar);
        limitDietaryIcon(ui->chContainsNuts);

        const QPixmap halal(QStringLiteral(":/dietary/halal.png"));
        const QPixmap kosher(QStringLiteral(":/dietary/kosher.png"));
        if (!halal.isNull() && !kosher.isNull()) {
            const QPixmap halalScaled =
                halal.scaled(dietaryIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            const QPixmap kosherScaled =
                kosher.scaled(dietaryIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            constexpr int gap = 4;
            QPixmap combined(halalScaled.width() + gap + kosherScaled.width(),
                             qMax(halalScaled.height(), kosherScaled.height()));
            combined.fill(Qt::transparent);
            QPainter painter(&combined);
            painter.drawPixmap(0, (combined.height() - halalScaled.height()) / 2, halalScaled);
            painter.drawPixmap(halalScaled.width() + gap,
                               (combined.height() - kosherScaled.height()) / 2,
                               kosherScaled);
            ui->chHalalKosher->setIcon(QIcon(combined));
            ui->chHalalKosher->setIconSize(combined.size());
        } else {
            limitDietaryIcon(ui->chHalalKosher);
        }
    }

    ui->leSupplier->setSelector(ui->leSupplierName, cache_goods_partners);
    ui->leGroup->setSelector(ui->leGroupName, cache_goods_group);
    ui->leUnit->setSelector(ui->leUnitName, cache_goods_unit);
    ui->leLowLevel->setValidator(new QDoubleValidator(0, 100000, 4));
    ui->tblGoods->setColumnWidths(7, 0, 0, 400, 80, 80, 80, 80);
    ui->leStoreId->setSelector(ui->leStoreIdName, cache_goods, 1, 3);
    ui->wGoodsType->selectorCallback = goodsTypeItemSelector;
#ifndef QT_DEBUG
    ui->leIsComplect->setVisible(false);
#endif
    C5Database db;
    db.exec("select f_name from c_goods");

    while(db.nextRow()) {
        fStrings.insert(db.getString(0));
    }

    QStringListModel *m = new QStringListModel(fStrings.values());
    QCompleter *c = new QCompleter(m);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    ui->leName->setCompleter(c);
    fBarcode = new Barcode();
    ui->lbScancodeType->setVisible(false);
    ui->btnPinLast->setChecked(__c5config.getRegValue("last_goods_editor").toBool());
    ui->leCostPrice->fDecimalPlaces = 4;
    QStringList l;
    l.append(tr("Sale price"));
    l.append(tr("Whosale"));
    l.append(tr("Retail disc"));
    l.append(tr("Whosale disc"));
    ui->tblPricing->setVerticalHeaderLabels(l);
    ui->tblPricing->setRowHidden(1, __c5config.getValue(211).toInt() == 1);
    db.exec("select * from e_currency order by f_id");
    QStringList colLabels;

    while(db.nextRow()) {
        int c = ui->tblPricing->columnCount();
        ui->tblPricing->setColumnCount(c + 1);
        colLabels.append(db.getString("f_name"));
        ui->tblPricing->createLineEdit(0, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(1, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(2, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(3, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->lineEdit(0, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(1, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(2, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(3, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(0, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(1, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(2, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(3, c)->setProperty("c", db.getInt("f_id"));
        connect(ui->tblPricing->lineEdit(0, c), &C5LineEdit::textEdited, this, &CE5Goods::priceEdited);
        connect(ui->tblPricing->lineEdit(1, c), &C5LineEdit::textEdited, this, &CE5Goods::priceEdited);
    }

    ui->tblPricing->setHorizontalHeaderLabels(colLabels);
    ui->tblPricing->fitColumnsToWidth();
    ui->cbCurrency->setDBValues("select f_id, f_name from e_currency");
    int basecurrecny = __c5config.getValue(param_default_currency).toInt();
    ui->cbCurrency->setIndexForValue(basecurrecny);
    connect(ui->leScanCode, &C5LineEditWithSelector::doubleClicked, this, &CE5Goods::genScancode);
    fScancodeGenerated = false;
    db.exec("select * from e_currency_cross_rate");

    while(db.nextRow()) {
        fCrossRate[QString("%1-%2").arg(db.getInt("f_currency1")).arg(db.getInt("f_currency2"))] = db.getDouble("f_rate");
    }

    db.exec("select f_id, f_name from as_list");

    while(db.nextRow()) {
        int r = ui->tblAs->rowCount();
        ui->tblAs->setRowCount(r + 1);
        ui->tblAs->setInteger(r, 0, db.getInt("f_id"));
        ui->tblAs->setString(r, 1, db.getString("f_name"));
        ui->tblAs->createLineEdit(r, 2);
    }

    ui->rbGenEAN8->setChecked(__c5config.getRegValue("gen_ean8").toBool());
    ui->tblMenu->setVisible(mUser->fConfig["officen_mode"].toInt() != 2);
    ui->tblMenu->setColumnWidths(ui->tblMenu->columnCount(), 0, 0, 200, 100, 150, 100, 100, 50, 50);
    if (mUser->fConfig["officen_mode"].toInt() != 2) {
        if (ui->tblMenu->rowCount() == 0) {
            NInterface::query1("/engine/v2/officen/menu/list", mUser->mSessionKey, this, {}, [this](const QJsonObject &jdoc) {
                QJsonArray jmenu = jdoc["menu"].toArray();
                QJsonArray jstorages = jdoc["storages"].toArray();
                QJsonArray jprinters = jdoc["printers"].toArray();
                QList<int> storageId;
                QStringList storageName;
                QStringList printers;

                for (int i = 0; i < jstorages.size(); i++) {
                    const QJsonObject &js = jstorages.at(i).toObject();
                    storageId << js["f_id"].toInt();
                    storageName << js["f_name"].toString();
                }

                for (int i = 0; i < jprinters.size(); i++) {
                    const QJsonObject &js = jprinters.at(i).toObject();
                    printers << js["f_name"].toString();
                }

                ui->tblMenu->setUpdatesEnabled(false);
                ui->tblMenu->setRowCount(jmenu.count());

                for (int i = 0; i < jmenu.count(); i++) {
                    for (int c = 0; c < ui->tblMenu->columnCount(); c++) {
                        ui->tblMenu->setItem(i, c, new QTableWidgetItem());
                    }

                    const QJsonObject &jm = jmenu.at(i).toObject();
                    ui->tblMenu->setInteger(i, 1, jm["f_id"].toInt());
                    ui->tblMenu->setString(i, 2, jm["f_menu_name"].toString());
                    C5LineEdit *le = ui->tblMenu->createLineEdit(i, 3);
                    le->setValidator(new QDoubleValidator(0, 999999999, 2));

                    C5ComboBox *cb = ui->tblMenu->createComboBox(i, 4);
                    cb->setValues(storageId, storageName);
                    cb = ui->tblMenu->createComboBox(i, 5);
                    cb->insertItems(0, printers);
                    cb = ui->tblMenu->createComboBox(i, 6);
                    cb->insertItems(0, printers);
                    C5CheckBox *ch = ui->tblMenu->createCheckbox(i, 7);
                    ch = ui->tblMenu->createCheckbox(i, 8);
                }
                ui->tblMenu->setUpdatesEnabled(true);
            });
        }
    }
}

CE5Goods::~CE5Goods()
{
    delete ui;
}

QString CE5Goods::title()
{
    return tr("Goods");
}

QString CE5Goods::table()
{
    return "c_goods";
}

QString CE5Goods::dbError(QString err)
{
    if(err.contains("f_scancode_UNIQUE")) {
        return tr("Duplicate scancode");
    }

    return CE5Editor::dbError(err);
}

void CE5Goods::setId(int id)
{
    if(id > 0) {
        fHttp->createHttpQuery("/engine/goods/goods-open.php", QJsonObject{{"id", id}}, SLOT(openResponse(QJsonObject)));
    }
}

bool CE5Goods::save(QString &err, QList<QMap<QString, QVariant> >& data)
{
    fLastGroup = ui->leGroup->getInteger();
    fLastUnit = ui->leUnit->getInteger();

    if(!ui->chSameStoreId->isChecked()) {
        if(ui->leStoreId->getInteger() == 0) {
            err += tr("Goods code for store output cannot be undefined") + "<br>";
        }
    }

    if(ui->wGoodsType->value() == 0) {
        err += tr("Goods type must be selected") + "<br>";
    }

    if(ui->leQtyBox->getDouble() < 1) {
        ui->leQtyBox->setDouble(1);
    }

    if(!err.isEmpty()) {
        return false;
    }

    /* Additional Options */
    fStrings.insert(ui->leName->text());
    static_cast<QStringListModel*>(ui->leName->completer()->model())->setStringList(fStrings.values());
    fHttp->createHttpQuery("/engine/goods/goods-save.php", makeJsonObject(), SLOT(saveResponse(QJsonObject)));
    err = "json";
    return true;
}

void CE5Goods::clear()
{
    int scancode = ui->leScanCode->getInteger();
    fImage.clear();
    fBigImage.clear();
    ui->tblBarcodes->setRowCount(0);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    for (int i = 0; i < ui->tblMenu->rowCount(); i++) {
        ui->tblMenu->lineEdit(i, 3)->clear();
    }
    CE5Editor::clear();
    ui->leLowLevel->setText("0");

    if(ui->tabWidget->currentIndex() > 1) {
        ui->tabWidget->setCurrentIndex(0);
    }

    ui->lbImage->setText(tr("Image"));
    ui->leTotal->clear();
    ui->chOnlyWholeNumber->setChecked(false);
    ui->chSameStoreId->setChecked(true);
    emit ui->chSameStoreId->clicked(true);

    if(__c5config.getRegValue("last_goods_editor").toBool()) {
        ui->leGroup->setValue(fLastGroup);
        ui->leUnit->setValue(fLastUnit);

        if(scancode) {
            ui->leScanCode->setInteger(scancode + 1);
        }

        ui->leName->setFocus();
    }

    for(int i = 0; i < ui->tblPricing->rowCount(); i++) {
        for(int c = 0; c < ui->tblPricing->columnCount(); c++) {
            ui->tblPricing->lineEdit(i, c)->clear();
        }
    }

    fScancodeGenerated = false;

    if(ui->cbCurrency->currentData().toInt() == 0) {
        ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    }

    for(int i = 0; i < ui->tblAs->rowCount(); i++) {
        ui->tblAs->lineEdit(i, 2)->clear();
    }

    ui->chCountDiscount->setChecked(true);
    ui->chCountService->setChecked(true);

    // Dietary / allergen badges
    ui->chGlutenFree->setChecked(false);
    ui->chVegetarian->setChecked(false);
    ui->chVegan->setChecked(false);
    ui->chNoGmo->setChecked(false);
    ui->chNoLactose->setChecked(false);
    ui->chNoSugar->setChecked(false);
    ui->chContainsNuts->setChecked(false);
    ui->chHalalKosher->setChecked(false);

    ui->leBjuKcal->clear();
    ui->leBjuProtein->clear();
    ui->leBjuFat->clear();
    ui->leBjuCarbs->clear();

    // Reset dynamic attributes (keep two required rows).
    initDynamicAttributesTable(ui->tblDynamicAttributes);
    if(ui->tblDynamicAttributes->item(0, 1)) {
        ui->tblDynamicAttributes->item(0, 1)->setText(QString());
    }
    if(ui->tblDynamicAttributes->item(1, 1)) {
        ui->tblDynamicAttributes->item(1, 1)->setText(QString());
    }
    if(ui->tblDynamicAttributes->item(0, 2)) {
        ui->tblDynamicAttributes->item(0, 2)->setText(QString());
    }
    if(ui->tblDynamicAttributes->item(1, 2)) {
        ui->tblDynamicAttributes->item(1, 2)->setText(QString());
    }
    if(ui->tblDynamicAttributes->item(0, 3)) {
        ui->tblDynamicAttributes->item(0, 3)->setText(QString());
    }
    if(ui->tblDynamicAttributes->item(1, 3)) {
        ui->tblDynamicAttributes->item(1, 3)->setText(QString());
    }

    ui->tblModificators->setRowCount(0);
    ui->tblRelatedDrinks->setRowCount(0);
    ui->tblRelatedOther->setRowCount(0);
}

QPushButton* CE5Goods::b1()
{
    QPushButton *btn = new QPushButton(tr("Print card"));
    connect(btn, SIGNAL(clicked()), this, SLOT(printCard()));
    return btn;
}

QJsonObject CE5Goods::makeJsonObject()
{
    fJsonData = QJsonObject();
    //Miscelanouse data
    QJsonObject jdata;
    jdata["f_count_service"] = ui->chCountService->isChecked();
    jdata["f_count_discount"] = ui->chCountDiscount->isChecked();
    jdata["f_hourly_payment"] = ui->chHourlyPayment->isChecked();
    jdata["f_hourly_rule"] = ui->leHourlyRole->text();
    jdata["f_cooking_time"] = ui->leCookingTime->getInteger();
    jdata["f_salary_department"] = ui->leSalaryDepartment->getInteger();
    jdata["f_salary_fixed_value"] = ui->leSalaryFixedValue->getDouble();
    jdata["f_salary_percent_value"] = ui->leSalaryPercentValue->getDouble();

    // Saved to c_goods.f_data.f_dietary_badge
    QJsonObject dietaryBadge;
    dietaryBadge["gluten_free"] = ui->chGlutenFree->isChecked();
    dietaryBadge["vegetarian"] = ui->chVegetarian->isChecked();
    dietaryBadge["vegan"] = ui->chVegan->isChecked();
    dietaryBadge["no_gmo"] = ui->chNoGmo->isChecked();
    dietaryBadge["no_lactose"] = ui->chNoLactose->isChecked();
    dietaryBadge["no_sugar"] = ui->chNoSugar->isChecked();
    dietaryBadge["contains_nuts"] = ui->chContainsNuts->isChecked();
    dietaryBadge["halal_kosher"] = ui->chHalalKosher->isChecked();
    jdata["f_dietary_badge"] = dietaryBadge;

    QJsonObject bju;
    bju.insert(QStringLiteral("kcal"), ui->leBjuKcal->getDouble());
    bju.insert(QStringLiteral("protein"), ui->leBjuProtein->getDouble());
    bju.insert(QStringLiteral("fat"), ui->leBjuFat->getDouble());
    bju.insert(QStringLiteral("carbs"), ui->leBjuCarbs->getDouble());
    jdata[QStringLiteral("f_bju")] = bju;

    // Stored to c_goods.f_data.f_dynamic_attributes (Type/Size + Measurement + Price).
    QJsonObject dynAttrs;
    initDynamicAttributesTable(ui->tblDynamicAttributes);
    const auto typeValue = ui->tblDynamicAttributes->item(0, 1)
                               ? ui->tblDynamicAttributes->item(0, 1)->text()
                               : QString();
    const auto typePrice = ui->tblDynamicAttributes->item(0, 3)
                               ? ui->tblDynamicAttributes->item(0, 3)->text()
                               : QString();
    const auto sizeValue = ui->tblDynamicAttributes->item(1, 1)
                               ? ui->tblDynamicAttributes->item(1, 1)->text()
                               : QString();
    const auto sizeMeas = ui->tblDynamicAttributes->item(1, 2)
                              ? ui->tblDynamicAttributes->item(1, 2)->text()
                              : QString();
    const auto sizePrice = ui->tblDynamicAttributes->item(1, 3)
                               ? ui->tblDynamicAttributes->item(1, 3)->text()
                               : QString();
    dynAttrs.insert(QStringLiteral("Type"), dynamicAttributeToJson(typeValue, QString(), typePrice));
    dynAttrs.insert(QStringLiteral("Size"), dynamicAttributeToJson(sizeValue, sizeMeas, sizePrice));
    jdata[QStringLiteral("f_dynamic_attributes")] = dynAttrs;
    jdata[QStringLiteral("f_modificators")] = modificatorsFromTable(ui->tblModificators);
    jdata[QStringLiteral("f_related_drink")] = relatedItemsFromTable(ui->tblRelatedDrinks);
    jdata[QStringLiteral("f_related_other")] = relatedItemsFromTable(ui->tblRelatedOther);

    QJsonObject j;
    j["f_id"] = ui->leCode->getInteger();
    j["f_name"] = ui->leName->text();
    j["f_supplier"] = ui->leSupplier->getInteger();
    j["f_group"] =  ui->leGroup->getInteger();
    j["f_unit"] = ui->leUnit->getInteger();
    j["f_lowlevel"] = ui->leLowLevel->getDouble();
    j["f_qtybox"] = ui->leQtyBox->getDouble();
    j["f_scancode"] = ui->leScanCode->text().isEmpty() ? QJsonValue() : ui->leScanCode->text();
    j["f_fiscalname"] = ui->leFiscalName->text();
    j["f_data"] = jdata;
    //TODO: option
    //j["f_lastinputprice"] = ui->leTotal->getDouble() > 0 ? ui->leTotal->getDouble() : ui->leCostPrice->getDouble();
    j["f_lastinputprice"] = ui->leCostPrice->getDouble();
    j["f_price_margin"] = ui->leMargin->getDouble();
    j["f_price_margin2"] = ui->leMargin2->getDouble();
    j["f_base_currency"] = ui->cbCurrency->currentData().toInt();
    j["f_storeid"] = ui->leStoreId->getInteger();
    j["f_adg"] = ui->leAdg->text();
    j["f_nospecial_price"] = ui->chNoSpecialPrice->isChecked() ? 1 : 0;
    j["f_service"] = ui->wGoodsType->value() == 3 ? 1 : 0;
    j["f_type"] = ui->wGoodsType->value();
    j["f_enabled"] = ui->chEnabled->isChecked() ? 1 : 0;
    j["f_production"] = ui->chProduction->isChecked() ? 1  : 0;
    j["f_wholenumber"] = ui->chOnlyWholeNumber->isChecked() ? 1 : 0;
    j["f_complectout"] = ui->leComplectOutputQty->getDouble();
    j["f_component_exit"] = ui->chComponentExit->isChecked() ? 1 : 0;
    j["f_weblink"] = ui->leWebLink->text();
    j["f_queue"] = ui->leQueue->getInteger();
    j["f_description"] = ui->plainTextEdit->toPlainText();
    j["f_acc"] = ui->leAcc->text();
    j["f_autodiscount"] = ui->leAutodiscount->text();
    fJsonData["goods"] = j;
    fJsonData["samestore"] = ui->chSameStoreId->isChecked();
    fJsonData["scangenerated"] = fScancodeGenerated;
    fJsonData["scancounter"] = ui->leScanCode->text().left(7).toInt();
    fJsonData["image"] = fImage.isEmpty() ? QJsonValue() : fImage;
    fJsonData["bigimage"] = fBigImage.isEmpty() ? QJsonValue() : fBigImage;
    QJsonArray ja;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        j = QJsonObject();
        j["f_base"] = ui->leCode->text();
        j["f_goods"] = ui->tblGoods->getInteger(i, 1);
        j["f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        j["f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        ja.append(j);
    }

    fJsonData["c_goods_complectation"] = ja;
    ja = QJsonArray();

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        j = QJsonObject();
        j["f_goods"] = ui->leCode->getInteger();
        j["f_price1"] = ui->tblPricing->lineEdit(0, i)->getDouble();
        j["f_price2"] = ui->tblPricing->lineEdit(1, i)->getDouble();
        j["f_price1disc"] = ui->tblPricing->lineEdit(2, i)->getDouble();
        j["f_price2disc"] = ui->tblPricing->lineEdit(3, i)->getDouble();
        j["f_currency"] = ui->tblPricing->lineEdit(0, i)->property("c").toInt();
        ja.append(j);
    }

    fJsonData["c_goods_prices"] = ja;
    fJsonData["astable"] = "c_goods";
    fJsonData["astableid"] = ui->leCode->getInteger();
    ja = QJsonArray();

    for(int i = 0; i < ui->tblAs->rowCount(); i++) {
        j = QJsonObject();
        j["f_asdbid"] = ui->tblAs->getInteger(i, 0);
        j["f_table"] = "c_goods";
        j["f_tableid"] = ui->leCode->getInteger();
        j["f_ascode"] = ui->tblAs->lineEdit(i, 2)->text();
        ja.append(j);
    }

    fJsonData["asconver"] = ja;

    if(ui->leCode->getInteger() > 0) {
        if(ui->chSameStoreId->isChecked() && ui->leStoreId->getInteger() == 0) {
            ui->leStoreId->setInteger(ui->leCode->getInteger());
        }
    }

    fJsonData["f_unitname"] = ui->leUnitName->text();
    QJsonArray barcodes;
    QSet<QString> uniqueBarcodes;

    // 1. Пытаемся добавить код из поля ввода
    QString scanCode = ui->leScanCode->text().trimmed();
    if (!scanCode.isEmpty()) {
        uniqueBarcodes.insert(scanCode);
    }

    // 2. Добавляем коды из таблицы
    for (int i = 0; i < ui->tblBarcodes->rowCount(); i++) {
        QString tableCode = ui->tblBarcodes->getString(i, 0).trimmed();
        if (!tableCode.isEmpty()) {
            uniqueBarcodes.insert(tableCode);
        }
    }

    // 3. Переносим уникальные значения в QJsonArray
    for (const QString &code : uniqueBarcodes) {
        barcodes.append(code);
    }

    fJsonData["f_barcodes"] = barcodes;
    QJsonArray jmenu;

    for(int i = 0; i < ui->tblMenu->rowCount(); i++) {
        QJsonObject jm;
        jm["f_menu"] = ui->tblMenu->getInteger(i, 1);
        jm["f_dish"] = ui->leCode->getInteger();
        jm["f_price"] = ui->tblMenu->lineEdit(i, 3)->getDouble();
        jm["f_store"] = (ui->tblMenu->comboBox(i, 4)->currentData().toInt() == 0) ? QJsonValue::Null : QJsonValue(ui->tblMenu->comboBox(i, 4)->currentData().toInt());
        jm["f_print1"] = ui->tblMenu->comboBox(i, 5)->currentText();
        jm["f_print2"] = ui->tblMenu->comboBox(i, 6)->currentText();
        jm["f_state"] = ui->tblMenu->checkBox(i, 7)->isChecked() ? 1 : 0;
        jm["f_recent"] = ui->tblMenu->checkBox(i, 8)->isChecked() ? 1 : 0;
        jmenu.append(jm);
    }

    fJsonData["menu"] = jmenu;
    return fJsonData;
}

bool CE5Goods::acceptOnSave() const
{
    return true;
}

bool CE5Goods::isOnline()
{
    return true;
}

void CE5Goods::saveResponse(const QJsonObject &jdoc)
{
    // data[0]["f_saleprice1"] = ui->tblPricing->lineEdit(0, 0)->getDouble();
    // data[0]["f_saleprice2"] = ui->tblPricing->lineEdit(1, 0)->getDouble();
    QJsonObject j = fJsonData["goods"].toObject();
    j["f_id"] = jdoc["f_id"].toInt();
    j["f_unitname"] = ui->leUnitName->text();
    fJsonData["goods"] = j;
    fHttp->httpQueryFinished(sender());

    if(jdoc["isnew"].toBool()) {
        C5Cache::cache(cache_goods)->refreshId("g.f_id", j["f_id"].toInt());
    }

    if(C5Editor *editor = qobject_cast<C5Editor *>(fEditor)) {
        QMap<QString, QVariant> row;
        row.insert(QStringLiteral("f_id"), jdoc.value(QStringLiteral("f_id")).toInt());
        editor->appendResultRow(row);
    }

    if(acceptOnSave()) {
        emit Accept();
    }
}

void CE5Goods::openResponse(const QJsonObject &jdoc)
{
    const QString imagePayload = jdoc.value(QStringLiteral("image")).toString();
    fImage = imagePayload;

    const QByteArray raw = decodeBase64ImagePayload(imagePayload);
    QPixmap p;
    bool imageShown = !raw.isEmpty() && p.loadFromData(raw);
    if(imageShown) {
        setLabelPixmapScaled(ui->lbImage, p);
    } else {
        ui->lbImage->setPixmap(QPixmap());
        ui->lbImage->setText(tr("Image"));
    }
    QJsonObject j = jdoc["goods"].toObject();
    ui->leCode->setInteger(j["f_id"].toInt());
    if(!imageShown) {
        loadGoodsImageToLabel(ui->leCode->getInteger(), ui->lbImage, &fImage);
    }
    ui->leName->setText(j["f_name"].toString());
    ui->leGroup->setValue(j["f_group"].toInt());
    ui->leSupplier->setValue(j["f_supplier"].toInt());
    ui->leUnit->setValue(j["f_unit"].toInt());
    ui->leScanCode->setText(j["f_scancode"].toString());
    ui->leLowLevel->setDouble(j["f_lowlevel"].toDouble());
    ui->leQtyBox->setDouble(j["f_qtybox"].toDouble());
    ui->leFiscalName->setText(j["f_fiscalname"].toString());
    ui->leCostPrice->setDouble(j["f_lastinputprice"].toDouble());
    ui->leAdg->setText(j["f_adg"].toString());
    ui->leMargin->setDouble(j["f_price_margin"].toDouble());
    ui->leMargin2->setDouble(j["f_price_margin2"].toDouble());
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(j["f_base_currency"].toInt()));
    ui->leStoreId->setValue(j["f_storeid"].toInt());
    ui->chSameStoreId->setChecked(ui->leStoreId->getInteger() == ui->leCode->getInteger());
    ui->chEnabled->setChecked(j["f_enabled"].toInt() > 0);
    ui->chOnlyWholeNumber->setChecked(j["f_wholenumber"].toInt() > 0);
    ui->wGoodsType->setCodeAndName(j.value("f_type").toInt(), j.value("f_type_name").toString());
    ui->chNoSpecialPrice->setChecked(j["f_nospecial_price"].toInt() > 0);
    ui->leComplectOutputQty->setDouble(j["f_complectout"].toDouble());
    ui->chComponentExit->setChecked(j["f_component_exit"].toInt() > 0);
    ui->leWebLink->setText(j["f_weblink"].toString());
    ui->leQueue->setInteger(j["f_queue"].toInt());
    ui->leAutodiscount->setText(j["f_autodiscount"].toString());
    QJsonArray ja = jdoc["complect"].toArray();

    for(int i = 0; i < ja.size(); i++) {
        int row = addGoodsRow();
        const QJsonObject &j = ja.at(i).toObject();
        ui->tblGoods->setInteger(row, 0, j["f_id"].toInt());
        ui->tblGoods->setInteger(row, 1, j["f_goods"].toInt());
        ui->tblGoods->setString(row, 2, j["f_goodsname"].toString());
        ui->tblGoods->lineEdit(row, 3)->setDouble(j["f_qty"].toDouble());
        ui->tblGoods->setString(row, 4, j["f_unitname"].toString());
        ui->tblGoods->lineEdit(row, 5)->setDouble(j["f_lastinputprice"].toDouble());
        ui->tblGoods->lineEdit(row, 6)->setDouble(j["f_total"].toDouble());
    }

    countTotal();
    ja = jdoc["goods_prices"].toArray();

    for(int i = 0; i < ja.size(); i++) {
        const QJsonObject &o = ja.at(i).toObject();

        for(int j = 0; j < ui->tblPricing->columnCount(); j++) {
            if(ui->tblPricing->lineEdit(0, j)->property("c").toInt() == o["f_currency"].toInt()) {
                ui->tblPricing->lineEdit(0, j)->setDouble(o["f_price1"].toDouble());
                ui->tblPricing->lineEdit(1, j)->setDouble(o["f_price2"].toDouble());
                ui->tblPricing->lineEdit(2, j)->setDouble(o["f_price1disc"].toDouble());
                ui->tblPricing->lineEdit(3, j)->setDouble(o["f_price2disc"].toDouble());
                break;
            }
        }
    }

    if(ui->cbCurrency->currentData().toInt() == 0) {
        ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    }

    ja = jdoc["astable"].toArray();

    for(int i = 0; i < ja.size(); i++) {
        const QJsonObject &o = ja.at(i).toObject();
        int asrow = -1;

        for(int i = 0; i < ui->tblAs->rowCount(); i++) {
            if(ui->tblAs->getInteger(i, 0) == o["f_asdbid"].toInt()) {
                asrow = i;
                break;
            }
        }

        if(asrow < 0) {
            throw std::runtime_error(QString("The database id (%1) not exists. Check database structure.").arg(
                                         asrow).toLocal8Bit().data());
        }

        ui->tblAs->lineEdit(asrow, 2)->setText(o["f_ascode"].toString());
    }

    //TODO
    // if(mUser->check(cp_t6_goods_only_price_edit)) {
    //     bool enabled = ui->leCode->getInteger() == 0;
    //     ui->tab_2->setEnabled(enabled);
    //     ui->tab_3->setEnabled(enabled);
    //     ui->tab_4->setEnabled(enabled);
    //     ui->leGroup->setEnabled(enabled);
    //     ui->leName->setEnabled(enabled);
    //     ui->leScanCode->setEnabled(enabled);
    // }
    QJsonArray jbarcodes = jdoc["barcodes"].toArray();

    for(int i = 0; i < jbarcodes.count(); i++) {
        int r = ui->tblBarcodes->addEmptyRow();
        ui->tblBarcodes->setString(i, 0, jbarcodes.at(i).toString());
        auto *b = static_cast<QPushButton*>(ui->tblBarcodes->createWidget2<QPushButton>(r, 1));
        connect(b, &QPushButton::clicked, this, [this, b]() {
            int r, c;

            if(ui->tblBarcodes->findWidget(b, r, c)) {
                ui->tblBarcodes->removeRow(r);
            }
        });
    }

    QJsonArray jmenu = jdoc["menu"].toArray();
    QJsonArray jstorages = jdoc["storages"].toArray();
    QJsonArray jprinters = jdoc["printers"].toArray();
    QList<int> storageId;
    QStringList storageName;
    QStringList printers;

    for(int i = 0; i < jstorages.size(); i++) {
        const QJsonObject &js = jstorages.at(i).toObject();
        storageId << js["f_id"].toInt();
        storageName << js["f_name"].toString();
    }

    for(int i = 0; i < jprinters.size(); i++) {
        const QJsonObject &js = jprinters.at(i).toObject();
        printers << js["f_name"].toString();
    }

    ui->tblMenu->setUpdatesEnabled(false);
    ui->tblMenu->setRowCount(jmenu.count());

    for(int i = 0; i < jmenu.count(); i++) {
        for(int c = 0; c < ui->tblMenu->columnCount(); c++) {
            ui->tblMenu->setItem(i, c, new QTableWidgetItem());
        }

        const QJsonObject &jm = jmenu.at(i).toObject();
        ui->tblMenu->setInteger(i, 1, jm["f_id"].toInt());
        ui->tblMenu->setString(i, 2, jm["f_menu_name"].toString());
        C5LineEdit *le = ui->tblMenu->createLineEdit(i, 3);
        le->setValidator(new QDoubleValidator(0, 999999999, 2));
        le->setDouble(jm["f_price"].toDouble());
        C5ComboBox *cb = ui->tblMenu->createComboBox(i, 4);
        cb->setValues(storageId, storageName);
        cb->setCurrentIndex(cb->findData(jm["f_store"].toInt()));
        cb = ui->tblMenu->createComboBox(i, 5);
        cb->insertItems(0, printers);
        cb->setCurrentIndex(cb->findText(jm["f_print1"].toString()));
        cb = ui->tblMenu->createComboBox(i, 6);
        cb->insertItems(0, printers);
        cb->setCurrentIndex(cb->findText(jm["f_print2"].toString()));
        C5CheckBox *ch = ui->tblMenu->createCheckbox(i, 7);
        ch->setChecked(jm["f_state"].toInt());
        ch = ui->tblMenu->createCheckbox(i, 8);
        ch->setChecked(jm["f_recent"].toInt());
    }

    QJsonObject jdata = parseGoodsFData(jdoc.value(QStringLiteral("goods")).toObject().value(QStringLiteral("f_data")));
    ui->chCountDiscount->setChecked(jdata["f_count_discount"].toBool());
    ui->chCountService->setChecked(jdata["f_count_service"].toBool());
    ui->chHourlyPayment->setChecked(jdata["f_hourly_payment"].toBool());
    ui->leHourlyRole->setText(jdata["f_hourly_rule"].toString());
    ui->leCookingTime->setInteger(jdata["f_cooking_time"].toInt());
    ui->leSalaryDepartment->setInteger(jdata["f_salary_department"].toInt());
    ui->leSalaryFixedValue->setDouble(jdata["f_salary_fixed_value"].toDouble());
    ui->leSalaryPercentValue->setDouble(jdata.value("f_salary_percent_value").toDouble());

    // Dietary / allergen badges
    const QJsonValue jb = jdata.value("f_dietary_badge");
    const auto dObj = jb.isObject() ? jb.toObject() : QJsonObject();
    ui->chGlutenFree->setChecked(dObj.value("gluten_free").toBool());
    ui->chVegetarian->setChecked(dObj.value("vegetarian").toBool());
    ui->chVegan->setChecked(dObj.value("vegan").toBool());
    ui->chNoGmo->setChecked(dObj.value("no_gmo").toBool());
    ui->chNoLactose->setChecked(dObj.value("no_lactose").toBool());
    ui->chNoSugar->setChecked(dObj.value("no_sugar").toBool());
    ui->chContainsNuts->setChecked(dObj.value("contains_nuts").toBool());
    ui->chHalalKosher->setChecked(dObj.value("halal_kosher").toBool());

    const QJsonObject bjuObj = jdata.value(QStringLiteral("f_bju")).toObject();
    ui->leBjuKcal->setDouble(bjuObj.value(QStringLiteral("kcal")).toDouble());
    ui->leBjuProtein->setDouble(bjuObj.value(QStringLiteral("protein")).toDouble());
    ui->leBjuFat->setDouble(bjuObj.value(QStringLiteral("fat")).toDouble());
    ui->leBjuCarbs->setDouble(bjuObj.value(QStringLiteral("carbs")).toDouble());

    // Dynamic attributes (hardcoded required Type/Size rows + Measurement column).
    initDynamicAttributesTable(ui->tblDynamicAttributes);
    const QJsonObject dynObj = jdata.value(QStringLiteral("f_dynamic_attributes")).toObject();

    QString typeValue;
    QString typeMeas;
    QString typePrice;
    QString sizeValue;
    QString sizeMeas;
    QString sizePrice;
    readDynamicAttributeFromJson(dynObj.value(QStringLiteral("Type")), &typeValue, &typeMeas, &typePrice);
    readDynamicAttributeFromJson(dynObj.value(QStringLiteral("Size")), &sizeValue, &sizeMeas, &sizePrice);

    if (sizeMeas.isEmpty()) {
        sizeMeas = dynObj.value(QStringLiteral("Measurement")).toString();
    }

    if(ui->tblDynamicAttributes->item(0, 1)) {
        ui->tblDynamicAttributes->item(0, 1)->setText(typeValue);
    }
    if(ui->tblDynamicAttributes->item(1, 1)) {
        ui->tblDynamicAttributes->item(1, 1)->setText(sizeValue);
    }
    if(ui->tblDynamicAttributes->item(0, 2)) {
        ui->tblDynamicAttributes->item(0, 2)->setText(QString());
    }
    if(ui->tblDynamicAttributes->item(1, 2)) {
        ui->tblDynamicAttributes->item(1, 2)->setText(sizeMeas);
    }
    if(ui->tblDynamicAttributes->item(0, 3)) {
        ui->tblDynamicAttributes->item(0, 3)->setText(typePrice);
    }
    if(ui->tblDynamicAttributes->item(1, 3)) {
        ui->tblDynamicAttributes->item(1, 3)->setText(sizePrice);
    }

    loadModificatorsFromJson(jdata.value(QStringLiteral("f_modificators")).toArray());
    loadRelatedFromJson(ui->tblRelatedDrinks, jdata.value(QStringLiteral("f_related_drink")).toArray());
    loadRelatedFromJson(ui->tblRelatedOther, jdata.value(QStringLiteral("f_related_other")).toArray());

    ui->tblMenu->setUpdatesEnabled(true);
    fHttp->httpQueryFinished(sender());
}

static QString makePriceHeaders(QTableWidget *tbl)
{
    QString h;
    QTextStream s(&h);

    for(int i = 0; i < tbl->columnCount(); ++i) {
        s << "<th class='right'>" << (i + 1) << "</th>";
    }

    return h;
}

static QString makeComplectationTable(C5TableWidget *tbl)
{
    QString h;
    QTextStream s(&h);
    s << "<div class='section-title'>" << htmlEscape(QObject::tr("Complectation")) << "</div>";
    s << "<table>";
    s << "<thead><tr>"
      << "<th>NN</th>"
      << "<th>Name</th>"
      << "<th class='right'>Qty</th>"
      << "<th>Unit</th>"
      << "<th class='right'>Price</th>"
      << "<th class='right'>Total</th>"
      << "</tr></thead><tbody>";

    for(int i = 0; i < tbl->rowCount(); ++i) {
        s << "<tr>";
        s << "<td class='center'>" << (i + 1) << "</td>";

        for(int c = 2; c < tbl->columnCount(); ++c) {
            const bool isNum = (c >= 4);
            s << "<td" << (isNum ? " class='right'" : "") << ">"
              << htmlEscape(tbl->getString(i, c))
              << "</td>";
        }

        s << "</tr>";
    }

    s << "</tbody></table>";
    return h;
}

static QString makePriceRow(C5TableWidget *tbl, int row)
{
    QString h;
    QTextStream s(&h);

    for(int i = 0; i < tbl->columnCount(); ++i) {
        s << "<td class='right'>"
          << htmlEscape(tbl->lineEdit(row, i)->text())
          << "</td>";
    }

    return h;
}

static QString makeComplectationTable2(C5TableWidget *tbl)
{
    QString h;
    QTextStream s(&h);
    s << "<div class='section-title'>" << htmlEscape(QObject::tr("Complectation")) << "</div>";
    s << "<table>";
    s << "<thead><tr>"
      << "<th>NN</th>"
      << "<th>Name</th>"
      << "<th class='right'>Qty</th>"
      << "<th>Unit</th>"
      << "<th class='right'>Price</th>"
      << "<th class='right'>Total</th>"
      << "</tr></thead><tbody>";

    for(int i = 0; i < tbl->rowCount(); ++i) {
        s << "<tr>";
        s << "<td class='center'>" << (i + 1) << "</td>";

        for(int c = 2; c < tbl->columnCount(); ++c) {
            const bool isNum = (c >= 4);
            s << "<td" << (isNum ? " class='right'" : "") << ">"
              << htmlEscape(tbl->getString(i, c))
              << "</td>";
        }

        s << "</tr>";
    }

    s << "</tbody></table>";
    return h;
}

void CE5Goods::printCard()
{
    QString html = loadTemplate("goods_card_a4.html");

    if(html.isEmpty()) {
        C5Message::error(tr("Template not found"));
        return;
    }

    QMap<QString, QString> v;
    v["printed"] =
        htmlEscape(QString("%1: %2")
                   .arg(tr("Printed"))
                   .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)));
    v["title"] =
        htmlEscape(tr("Goods card") + " " +
                   ui->leName->text() + " " +
                   ui->leScanCode->text());
    v["group"] =
        htmlEscape(tr("Goods group") + ": " + ui->leGroupName->text());
    v["internal_code"] =
        htmlEscape(tr("Internal code") + ": " + ui->leCode->text());
    v["price_type"] = htmlEscape(tr("Price type"));
    v["retail_label"] = htmlEscape(tr("Retail price"));
    v["wholesale_label"] = htmlEscape(tr("Wholesale price"));
    v["price_headers"] = makePriceHeaders(ui->tblPricing);
    v["retail_values"] = makePriceRow(ui->tblPricing, 0);
    v["wholesale_values"] = makePriceRow(ui->tblPricing, 1);

    // complectation
    if(ui->tblGoods->rowCount() > 0) {
        QString block = makeComplectationTable2(ui->tblGoods);
        block += "<div class='total'>" +
                 htmlEscape(tr("Complectation cost") + ": " + ui->leTotal->text()) +
                 "</div>";
        v["complectation_block"] = block;
    } else {
        v["complectation_block"] = "";
    }

    html = applyTemplate(html, v);
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog pp(&printer, this);
    connect(&pp, &QPrintPreviewDialog::paintRequested, [&](QPrinter * p) { doc.print(p); });
    pp.setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    pp.raise();
    pp.activateWindow();
    pp.exec();
}

void CE5Goods::priceEdited(const QString &arg1)
{
    C5LineEdit *e = static_cast<C5LineEdit*>(sender());
    int r, c;

    if(!ui->tblPricing->findWidget(e, r, c)) {
        return;
    }

    C5LineEditWithSelector *l;

    switch(r) {
    case 0:
        l = ui->leMargin;
        break;

    case 1:
        l = ui->leMargin2;
        break;
    }

    int basecurrency = e->property("c").toInt();

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        if(ui->tblPricing->lineEdit(r, i) == e) {
            double costprice = ui->leCostPrice->getDouble();

            if(basecurrency != ui->cbCurrency->currentData().toInt()) {
                QString mcrossrate = QString("%1-%2").arg(ui->cbCurrency->currentData().toString(), QString::number(basecurrency));
                costprice *= fCrossRate[mcrossrate];
            }

            l->setDouble(((str_float(arg1) / costprice) - 1) * 100);
            continue;
        }

        QString crossrate = QString("%1-%2").arg(QString::number(basecurrency), ui->tblPricing->lineEdit(r,
                            i)->property("c").toString());
        double rate = fCrossRate[crossrate];
        ui->tblPricing->lineEdit(r, i)->setDouble(str_float(arg1) *rate);
    }

    if(l->text() == "nan" || l->text() == "inf") {
        l->setDouble(0);
    }
}

void CE5Goods::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    ui->tblGoods->findWidget(l, row, col);
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, 6);
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
    countTotal();
}

void CE5Goods::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    ui->tblGoods->findWidget(l, row, col);
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, 6);
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
    countTotal();
}

void CE5Goods::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    ui->tblGoods->findWidget(l, row, col);
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, 6);

    if(lqty->getDouble() > 0.0001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    } else {
        lprice->setDouble(0);
    }

    countTotal();
}

void CE5Goods::uploadImage()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Image"), "", "*.jpg;*.png;*.bmp");

    if(fn.isEmpty()) {
        return;
    }

    QPixmap pm;

    if(!pm.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }

    QByteArray ba;
    QBuffer bigBuff(&ba);
    bigBuff.open(QIODevice::WriteOnly);
    pm.save(&bigBuff, "JPG");
    fBigImage = ba.toBase64();

    do {
        if(fBigImage.isEmpty()) {
        }

        pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
        ba.clear();
        QBuffer buff(&ba);
        buff.open(QIODevice::WriteOnly);
        pm.save(&buff, "JPG");
    } while(ba.size() > 100000);

    if(ba.isEmpty()) {
        C5Message::error(tr("Could not encode image"));
        return;
    }

    setLabelPixmapScaled(ui->lbImage, pm);
    fImage = QString::fromLatin1(ba.toBase64());
}

void CE5Goods::removeImage()
{
    if(C5Message::question(tr("Remove image")) !=  QDialog::Accepted) {
        return;
    }

    fImage = "";
    fBigImage.clear();
    ui->lbImage->setText(tr("Image"));
}

void CE5Goods::genScancode()
{
    if(!ui->leScanCode->isEmpty()) {
        C5Message::error(tr("Scancode field must be empty"));
        return;
    }

    C5Database db;
    db.exec("select * from c_goods_scancode_counter");
    db.nextRow();
    ui->leScanCode->setText(QString("%1").arg(db.getInt("f_counter") + 1, db.getInt("f_digitsnumber"), 10, QChar('0')));
    //    int checksum = fBarcode->ean8CheckSum(ui->leScanCode->text());
    //    ui->leScanCode->setText(QString("%1%2").arg(ui->leScanCode->text(), QString::number(checksum)));
    fScancodeGenerated = true;
}

void CE5Goods::on_btnNewGroup_clicked()
{
    CE5GoodsGroup *ep = new CE5GoodsGroup();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        ui->leGroup->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void CE5Goods::on_btnNewUnit_clicked()
{
    CE5GoodsUnit *ep = new CE5GoodsUnit();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        ui->leUnit->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void CE5Goods::on_btnAddGoods_clicked()
{
    QJsonArray vals;

    if(!C5Selector::getValue(mUser, cache_goods, vals)) {
        return;
    }

    int row = addGoodsRow();
    ui->tblGoods->setInteger(row, 1, vals.at(1).toInt());
    ui->tblGoods->setString(row, 2, vals.at(3).toString());
    ui->tblGoods->setString(row, 4, vals.at(4).toString());
    ui->tblGoods->lineEdit(row, 5)->setDouble(vals.at(6).toDouble());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
    setComplectFlag();
}

void CE5Goods::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }

    ui->tblGoods->removeRow(row);
    setComplectFlag();
    countTotal();
}

int CE5Goods::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, 0, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 1, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 2, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, 3);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, 5);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 4));
    lprice->fDecimalPlaces = 4;
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 6);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

void CE5Goods::countTotal()
{
    double total = 0;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }

    ui->leTotal->setDouble(total);
    setComplectFlag();

    if(ui->wGoodsType->value() == GOODS_TYPE_DISH
            || ui->wGoodsType->value() == GOODS_TYPE_GOODS) {
        ui->leCostPrice->setDouble(total);
    }
}

void CE5Goods::setComplectFlag()
{
    ui->leIsComplect->setInteger(ui->tblGoods->rowCount() == 0 ? 0 : 1);

    if(ui->leComplectOutputQty->getDouble() < 0.001) {
        ui->leComplectOutputQty->setDouble(1);
    }
}

void CE5Goods::countSalePrice(int r, double margin)
{
    int basecurrency = ui->cbCurrency->currentData().toInt();

    for(int c = 0; c < ui->tblPricing->columnCount(); c++) {
        if(ui->tblPricing->lineEdit(r, c)->property("c").toInt() == basecurrency) {
            ui->tblPricing->lineEdit(r, c)->setDouble(((margin / 100) *ui->leCostPrice->getDouble()) +
                ui->leCostPrice->getDouble());
        } else {
            QString crossrate = QString("%1-%2").arg(QString::number(basecurrency), ui->tblPricing->lineEdit(r,
                                c)->property("c").toString());
            double rate = fCrossRate[crossrate];
            ui->tblPricing->lineEdit(r, c)->setDouble((((margin / 100) *ui->leCostPrice->getDouble()) +
                    ui->leCostPrice->getDouble()) *rate);
        }
    }
}

void CE5Goods::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QJsonObject data;

    if(e->getJsonObject(data)) {
        int row = addGoodsRow();
        data = data["goods"].toObject();
        ui->tblGoods->setData(row, 1, data["f_id"].toInt());
        ui->tblGoods->setData(row, 2, data["f_name"].toString());
        ui->tblGoods->setData(row, 4, data["f_unitname"].toString());
        ui->tblGoods->lineEdit(row, 3)->setFocus();
    }

    delete e;
    setComplectFlag();
}

void CE5Goods::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        ui->leSupplier->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void CE5Goods::on_btnCopy_clicked()
{
    QString clipbrd;

    for(int r = 0; r < ui->tblGoods->rowCount(); r++) {
        clipbrd += ui->tblGoods->getString(r, 1) + "\t";
        clipbrd += ui->tblGoods->getString(r, 2) + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 3)->text() + "\t";
        clipbrd += ui->tblGoods->getString(r, 4) + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 5)->text() + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 6)->text() + "\t";
        clipbrd += "\r\n";
    }

    if(!clipbrd.isEmpty()) {
        qApp->clipboard()->setText(clipbrd);
    }
}

void CE5Goods::on_btnPaste_clicked()
{
    QStringList rows = qApp->clipboard()->text().split("\r\n");

    foreach(const QString &s, rows) {
        QStringList cols = s.split("\t");

        if(cols.count() < 6) {
            continue;
        }

        int row = addGoodsRow();
        ui->tblGoods->setData(row, 1, cols.at(0));
        ui->tblGoods->setData(row, 2, cols.at(1));
        ui->tblGoods->lineEdit(row, 3)->setText(cols.at(2));
        ui->tblGoods->setData(row, 4, cols.at(3));
        ui->tblGoods->lineEdit(row, 5)->setText(cols.at(4));
        ui->tblGoods->lineEdit(row, 6)->setText(cols.at(5));
    }

    setComplectFlag();
}

void CE5Goods::on_lbImage_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(QIcon(":/new.png"), tr("Upload image"), this, SLOT(uploadImage()));
    m->addAction(QIcon(":/delete.png"), tr("Remove image"), this, SLOT(removeImage()));
    m->popup(ui->lbImage->mapToGlobal(pos));
}

void CE5Goods::on_tabWidget_currentChanged(int index)
{
    switch(index) {
    case 3: {
        loadGoodsImageToLabel(ui->leCode->getInteger(), ui->lbImage, &fImage);
    }
    }
}

void CE5Goods::on_leScanCode_textChanged(const QString &arg1)
{
    if(fBarcode->isEan13(arg1)) {
        ui->lbScancodeType->setVisible(true);
        ui->lbScancodeType->setText("EAN13");
        ui->btnSetControlSum->setVisible(false);
    } else if(fBarcode->isEan13(arg1)) {
        ui->lbScancodeType->setVisible(true);
        ui->lbScancodeType->setText("EAN8");
        ui->btnSetControlSum->setVisible(false);
    } else {
        ui->lbScancodeType->setVisible(false);
        ui->btnSetControlSum->setVisible(true);
        ui->btnSetControlSum->setEnabled(arg1.length() == 12);
    }
}

void CE5Goods::on_btnSetControlSum_clicked()
{
    int checksum = fBarcode->ean13CheckSum(ui->leScanCode->text());

    if(checksum > -1) {
        ui->leScanCode->setText(ui->leScanCode->text() + QString::number(checksum));
    }
}

void CE5Goods::on_btnPinLast_clicked(bool checked)
{
    __c5config.setRegValue("last_goods_editor", checked);
}

void CE5Goods::on_chSameStoreId_clicked()
{
    ui->leStoreId->setEnabled(!ui->chSameStoreId->isChecked());
    ui->leStoreIdName->setEnabled(!ui->chSameStoreId->isChecked());

    if(ui->chSameStoreId->isChecked()) {
        ui->leStoreId->setValue(ui->leCode->getInteger());
    }
}

void CE5Goods::on_leUnitName_textChanged(const QString &arg1)
{
    ui->lbOutputUnit->setText(arg1);
}

void CE5Goods::on_btnPrintBarcode_clicked()
{
    // if(ui->rbGenEAN8->isChecked()) {
    //     if(ui->leScanCode->text().length() == 4) {
    //         QString code = QString("%1").arg(ui->leGroup->getInteger(), 3, 10,  QChar('0'));
    //         ui->leScanCode->setText(ui->leScanCode->text() + code);
    //     }
    //     if(ui->leScanCode->text().length() == 7) {
    //         ui->leScanCode->setText(Ean8Generator::last(ui->leScanCode->text()));
    //     }
    // }
    QPrintDialog pd;

    if(pd.exec() == QDialog::Accepted) {
        C5StoreBarcode::printOneBarcode(ui->leScanCode->text(), ui->tblPricing->lineEdit(0, 0)->text(), "", ui->leName->text(),
                                        pd);
    }
}

void CE5Goods::on_leCostPrice_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    countSalePrice(0, str_float(ui->leMargin->text()));
    countSalePrice(1, str_float(ui->leMargin2->text()));
}

void CE5Goods::on_leMargin_textEdited(const QString &arg1)
{
    countSalePrice(0, str_float(arg1));
}

void CE5Goods::on_leMargin2_textEdited(const QString &arg1)
{
    countSalePrice(1, str_float(arg1));
}

void CE5Goods::on_btnNewModel_clicked()
{
    CE5GoodsModel *ep = new CE5GoodsModel();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
    }

    delete e;
}

void CE5Goods::on_leScanCode_returnPressed()
{
    ui->leScanCode->setText(C5ReplaceCharacter::replace(ui->leScanCode->text()));
}

void CE5Goods::on_leBarcode_returnPressed()
{
    int r = ui->tblBarcodes->addEmptyRow();
    ui->tblBarcodes->setString(r, 0, ui->leBarcode->text());
    auto *b = static_cast<QPushButton*>(ui->tblBarcodes->createWidget2<QPushButton>(r, 1));
    connect(b, &QPushButton::clicked, this, [this, b]() {
        int r, c;

        if(ui->tblBarcodes->findWidget(b, r, c)) {
            ui->tblBarcodes->removeRow(r);
        }
    });
    ui->leBarcode->clear();
    ui->tblBarcodes->resizeColumnsToContents();
    ui->leBarcode->setFocus();
}

void CE5Goods::on_leTotal_textChanged(const QString &arg1)
{
    //ui->leCostPrice->setText(arg1);
}

void CE5Goods::on_rbGenEAN8_clicked(bool checked)
{
    __c5config.setRegValue("gen_ean8", checked);
}

void CE5Goods::on_btnFromProduct_clicked()
{
    if(C5Message::question(tr("Clear current data")) == QDialog::Accepted) {
        ui->tblGoods->setRowCount(0);
    }

    QJsonArray vals;

    if(!C5Selector::getValue(mUser, cache_goal_products, vals)) {
        return;
    }

    C5Database db;
    db[":f_product"]  = vals.at(1).toInt();
    db.exec(R"(
    SELECT pm.f_material as f_goods, g1.f_name as f_goodsname, u.f_name  as f_unitname,
    f_qtyperone, f_totalqty, g1.f_lastinputprice
    FROM m_goal_product_material pm
    LEFT JOIN c_goods  g1 ON g1.f_id=pm.f_material
    left join c_units u on u.f_id=g1.f_unit
    where pm.f_product=:f_product
    )");

    while(db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setInteger(row, 0, 0);
        ui->tblGoods->setInteger(row, 1, db.getInt("f_goods"));
        ui->tblGoods->setString(row, 2, db.getString("f_goodsname"));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble("f_qtyperone"));
        ui->tblGoods->setString(row, 4, db.getString("f_unitname"));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_lastinputprice"));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble("f_qtyperon")*db.getDouble("f_lastinputprice"));
    }

    countTotal();
}

int CE5Goods::addModificatorRow()
{
    const int row = ui->tblModificators->rowCount();
    ui->tblModificators->setRowCount(row + 1);
    ui->tblModificators->setItem(row, 0, new QTableWidgetItem());
    ui->tblModificators->setItem(row, 1, new QTableWidgetItem());
    ui->tblModificators->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tblModificators->item(row, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    C5LineEdit *price = ui->tblModificators->createLineEdit(row, 2);
    price->setValidator(new QDoubleValidator(0, 999999999, 2));
    price->fDecimalPlaces = 2;
    ui->tblModificators->createCheckbox(row, 3);
    return row;
}

void CE5Goods::loadModificatorsFromJson(const QJsonArray &items)
{
    ui->tblModificators->setRowCount(0);

    for(const QJsonValue &value : items) {
        if(!value.isObject()) {
            continue;
        }

        const QJsonObject item = value.toObject();
        const int modificatorId = item.value(QStringLiteral("f_id")).toInt();
        if(modificatorId <= 0) {
            continue;
        }

        const int row = addModificatorRow();
        ui->tblModificators->setInteger(row, 0, modificatorId);
        ui->tblModificators->setString(row, 1, item.value(QStringLiteral("f_name")).toString());
        ui->tblModificators->lineEdit(row, 2)->setDouble(item.value(QStringLiteral("f_price")).toDouble());
        ui->tblModificators->checkBox(row, 3)->setChecked(item.value(QStringLiteral("f_required")).toBool());
    }
}

void CE5Goods::on_btnAddModificator_clicked()
{
    QJsonArray vals;
    if(!C5Selector::getValue(mUser, modificatorSelectorQuery(), vals)) {
        return;
    }

    const int modificatorId = vals.at(1).toInt();
    if(modificatorId <= 0) {
        return;
    }

    for(int i = 0; i < ui->tblModificators->rowCount(); ++i) {
        if(ui->tblModificators->getInteger(i, 0) == modificatorId) {
            C5Message::error(tr("This modificator is already added"));
            return;
        }
    }

    const int row = addModificatorRow();
    ui->tblModificators->setInteger(row, 0, modificatorId);
    ui->tblModificators->setString(row, 1, vals.at(3).toString());
    ui->tblModificators->lineEdit(row, 2)->setDouble(vals.at(6).toDouble());
    ui->tblModificators->checkBox(row, 3)->setChecked(false);
}

void CE5Goods::on_btnRemoveModificator_clicked()
{
    const int row = ui->tblModificators->currentRow();
    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblModificators->getString(row, 1)) != QDialog::Accepted) {
        return;
    }

    ui->tblModificators->removeRow(row);
}

int CE5Goods::addRelatedRow(C5TableWidget *tbl)
{
    const int row = tbl->rowCount();
    tbl->setRowCount(row + 1);
    tbl->setItem(row, 0, new QTableWidgetItem());
    tbl->setItem(row, 1, new QTableWidgetItem());
    tbl->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tbl->item(row, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    return row;
}

void CE5Goods::loadRelatedFromJson(C5TableWidget *tbl, const QJsonArray &items)
{
    if(!tbl) {
        return;
    }
    tbl->setRowCount(0);

    for(const QJsonValue &value : items) {
        if(!value.isObject()) {
            continue;
        }
        const QJsonObject item = value.toObject();
        const int dishId = item.value(QStringLiteral("f_id")).toInt();
        if(dishId <= 0) {
            continue;
        }
        const int row = addRelatedRow(tbl);
        tbl->setInteger(row, 0, dishId);
        tbl->setString(row, 1, item.value(QStringLiteral("f_name")).toString());
    }
}

bool CE5Goods::addRelatedFromSelector(C5TableWidget *tbl)
{
    if(!tbl) {
        return false;
    }

    QJsonArray vals;
    if(!C5Selector::getValue(mUser, menuDishSelectorQuery(), vals)) {
        return false;
    }

    const int dishId = vals.at(1).toInt();
    if(dishId <= 0) {
        return false;
    }

    for(int i = 0; i < tbl->rowCount(); ++i) {
        if(tbl->getInteger(i, 0) == dishId) {
            C5Message::error(tr("This dish is already added"));
            return false;
        }
    }

    const int row = addRelatedRow(tbl);
    tbl->setInteger(row, 0, dishId);
    tbl->setString(row, 1, vals.at(3).toString());
    return true;
}

void CE5Goods::on_btnAddRelatedDrink_clicked()
{
    addRelatedFromSelector(ui->tblRelatedDrinks);
}

void CE5Goods::on_btnRemoveRelatedDrink_clicked()
{
    const int row = ui->tblRelatedDrinks->currentRow();
    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblRelatedDrinks->getString(row, 1)) != QDialog::Accepted) {
        return;
    }

    ui->tblRelatedDrinks->removeRow(row);
}

void CE5Goods::on_btnAddRelatedOther_clicked()
{
    addRelatedFromSelector(ui->tblRelatedOther);
}

void CE5Goods::on_btnRemoveRelatedOther_clicked()
{
    const int row = ui->tblRelatedOther->currentRow();
    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblRelatedOther->getString(row, 1)) != QDialog::Accepted) {
        return;
    }

    ui->tblRelatedOther->removeRow(row);
}
