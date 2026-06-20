#include "dlgfiscaltest.h"
#include "ui_dlgfiscaltest.h"

#include "printtaxn.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QShowEvent>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QMessageBox>
#include <QPointer>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QTableWidgetItem>
#include <QThread>
#include <QTimer>

namespace {

enum GoodsColumn {
    colName = 0,
    colPrice,
    colQty,
    colTaxDept,
    colAdg,
    colDiscount,
    colCode,
    colCount
};

QString stateFilePath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + QStringLiteral("/state.json");
}

double cellDouble(QTableWidget *tbl, int row, int col, double fallback = 0.0)
{
    auto *item = tbl->item(row, col);
    if (!item) {
        return fallback;
    }
    bool ok = false;
    const double v = item->text().replace(',', '.').toDouble(&ok);
    return ok ? v : fallback;
}

QString cellText(QTableWidget *tbl, int row, int col)
{
    auto *item = tbl->item(row, col);
    return item ? item->text().trimmed() : QString();
}

} // namespace

DlgFiscalTest::DlgFiscalTest(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgFiscalTest)
{
    ui->setupUi(this);
    setWindowTitle(tr("Test fiscal receipt"));

    ui->tblGoods->setColumnCount(colCount);
    ui->tblGoods->setHorizontalHeaderLabels({
        tr("Name"),
        tr("Price"),
        tr("Qty"),
        tr("Tax dept"),
        tr("ADG"),
        tr("Discount %"),
        tr("Code"),
    });
    ui->tblGoods->horizontalHeader()->setStretchLastSection(true);
    ui->tblGoods->horizontalHeader()->setSectionResizeMode(colName, QHeaderView::Stretch);
    ui->tblGoods->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblGoods->setSelectionMode(QAbstractItemView::SingleSelection);

    auto *modeGroup = new QButtonGroup(this);
    modeGroup->addButton(ui->rbModeDetailed, static_cast<int>(PrintMode::Detailed));
    modeGroup->addButton(ui->rbModeSimple, static_cast<int>(PrintMode::Simple));

    m_saveTimer = new QTimer(this);
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(400);
    connect(m_saveTimer, &QTimer::timeout, this, &DlgFiscalTest::saveState);

    connect(modeGroup, &QButtonGroup::idClicked, this, [this](int) {
        updatePrintModeUi();
        scheduleSaveState();
    });

    setupAutoSave();
}

DlgFiscalTest::~DlgFiscalTest()
{
    saveState();
    delete ui;
}

void DlgFiscalTest::closeEvent(QCloseEvent *event)
{
    saveState();
    QDialog::closeEvent(event);
}

void DlgFiscalTest::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (m_stateLoaded) {
        return;
    }
    m_stateLoaded = true;

    loadState();
    updatePrintModeUi();

    if (ui->tblGoods->rowCount() == 0) {
        addGoodsRow(tr("Test item"), 1000.0, 1.0, 1, QStringLiteral("0101"), 0.0, QStringLiteral("1"));
    }
}

DlgFiscalTest::PrintMode DlgFiscalTest::currentPrintMode() const
{
    return ui->rbModeSimple->isChecked() ? PrintMode::Simple : PrintMode::Detailed;
}

void DlgFiscalTest::updatePrintModeUi()
{
    const bool simple = currentPrintMode() == PrintMode::Simple;
    ui->labelSimpleTaxDept->setEnabled(simple);
    ui->leSimpleTaxDept->setEnabled(simple);
    // In simple mode we don't need goods rows, but the print button must remain enabled.
    ui->tblGoods->setEnabled(!simple);
    ui->btnAddRow->setEnabled(!simple);
    ui->btnRemoveRow->setEnabled(!simple);
}

void DlgFiscalTest::setupAutoSave()
{
    const auto hookSave = [this](QWidget *w) {
        if (auto *le = qobject_cast<QLineEdit *>(w)) {
            connect(le, &QLineEdit::editingFinished, this, &DlgFiscalTest::scheduleSaveState);
        }
    };
    hookSave(ui->leIp);
    hookSave(ui->lePort);
    hookSave(ui->lePassword);
    hookSave(ui->leOpPin);
    hookSave(ui->leOpPassword);
    hookSave(ui->leCash);
    hookSave(ui->leCard);
    hookSave(ui->lePrepaid);
    hookSave(ui->leSimpleTaxDept);
    connect(ui->cbExternalPos, &QCheckBox::toggled, this, &DlgFiscalTest::scheduleSaveState);
    connect(ui->tblGoods, &QTableWidget::itemChanged, this, &DlgFiscalTest::scheduleSaveState);
}

void DlgFiscalTest::scheduleSaveState()
{
    if (m_loadingState || !m_saveTimer) {
        return;
    }
    m_saveTimer->start();
}

void DlgFiscalTest::loadState()
{
    m_loadingState = true;

    const QSignalBlocker blockTable(ui->tblGoods);
    const QSignalBlocker blockExternalPos(ui->cbExternalPos);
    const QSignalBlocker blockDetailed(ui->rbModeDetailed);
    const QSignalBlocker blockSimple(ui->rbModeSimple);

    QFile file(stateFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        m_loadingState = false;
        return;
    }

    const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    const QJsonObject fiscal = root.value(QStringLiteral("fiscal")).toObject();
    const QJsonObject payment = root.value(QStringLiteral("payment")).toObject();

    ui->leIp->setText(fiscal.value(QStringLiteral("ip")).toString(ui->leIp->text()));
    ui->lePort->setText(fiscal.value(QStringLiteral("port")).toString(ui->lePort->text()));
    ui->lePassword->setText(fiscal.value(QStringLiteral("password")).toString());
    ui->cbExternalPos->setChecked(fiscal.value(QStringLiteral("externalPos")).toBool());
    ui->leOpPin->setText(fiscal.value(QStringLiteral("opPin")).toString());
    ui->leOpPassword->setText(fiscal.value(QStringLiteral("opPassword")).toString());

    ui->leCash->setText(payment.value(QStringLiteral("cash")).toString(ui->leCash->text()));
    ui->leCard->setText(payment.value(QStringLiteral("card")).toString(ui->leCard->text()));
    ui->lePrepaid->setText(payment.value(QStringLiteral("prepaid")).toString(ui->lePrepaid->text()));

    const int mode = root.value(QStringLiteral("printMode")).toInt(static_cast<int>(PrintMode::Detailed));
    if (mode == static_cast<int>(PrintMode::Simple)) {
        ui->rbModeSimple->setChecked(true);
    } else {
        ui->rbModeDetailed->setChecked(true);
    }
    ui->leSimpleTaxDept->setText(root.value(QStringLiteral("simpleTaxDept")).toString(QStringLiteral("1")));

    ui->tblGoods->setRowCount(0);
    const QJsonArray goods = root.value(QStringLiteral("goods")).toArray();
    for (const QJsonValue &gv : goods) {
        const QJsonObject g = gv.toObject();
        addGoodsRow(g.value(QStringLiteral("name")).toString(),
                    g.value(QStringLiteral("price")).toDouble(),
                    g.value(QStringLiteral("qty")).toDouble(1.0),
                    g.value(QStringLiteral("taxDept")).toInt(1),
                    g.value(QStringLiteral("adg")).toString(),
                    g.value(QStringLiteral("discount")).toDouble(),
                    g.value(QStringLiteral("code")).toString());
    }

    ui->teLog->setPlainText(root.value(QStringLiteral("log")).toString().left(32000));

    m_loadingState = false;
}

void DlgFiscalTest::saveState()
{
    if (m_loadingState) {
        return;
    }
    QJsonArray goods;
    for (int r = 0; r < ui->tblGoods->rowCount(); ++r) {
        QJsonObject g;
        g.insert(QStringLiteral("name"), cellText(ui->tblGoods, r, colName));
        g.insert(QStringLiteral("price"), cellDouble(ui->tblGoods, r, colPrice));
        g.insert(QStringLiteral("qty"), cellDouble(ui->tblGoods, r, colQty, 1.0));
        g.insert(QStringLiteral("taxDept"), static_cast<int>(cellDouble(ui->tblGoods, r, colTaxDept, 1.0)));
        g.insert(QStringLiteral("adg"), cellText(ui->tblGoods, r, colAdg));
        g.insert(QStringLiteral("discount"), cellDouble(ui->tblGoods, r, colDiscount));
        g.insert(QStringLiteral("code"), cellText(ui->tblGoods, r, colCode));
        goods.append(g);
    }

    QJsonObject root;
    root.insert(QStringLiteral("fiscal"), QJsonObject{
        {QStringLiteral("ip"), ui->leIp->text().trimmed()},
        {QStringLiteral("port"), ui->lePort->text().trimmed()},
        {QStringLiteral("password"), ui->lePassword->text()},
        {QStringLiteral("externalPos"), ui->cbExternalPos->isChecked()},
        {QStringLiteral("opPin"), ui->leOpPin->text()},
        {QStringLiteral("opPassword"), ui->leOpPassword->text()},
    });
    root.insert(QStringLiteral("payment"), QJsonObject{
        {QStringLiteral("cash"), ui->leCash->text()},
        {QStringLiteral("card"), ui->leCard->text()},
        {QStringLiteral("prepaid"), ui->lePrepaid->text()},
    });
    root.insert(QStringLiteral("printMode"), static_cast<int>(currentPrintMode()));
    root.insert(QStringLiteral("simpleTaxDept"), ui->leSimpleTaxDept->text());
    root.insert(QStringLiteral("goods"), goods);
    root.insert(QStringLiteral("log"), ui->teLog->toPlainText().left(32000));

    QFile file(stateFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

void DlgFiscalTest::addGoodsRow(const QString &name,
                                double price,
                                double qty,
                                int taxDept,
                                const QString &adg,
                                double discount,
                                const QString &code)
{
    const int row = ui->tblGoods->rowCount();
    ui->tblGoods->insertRow(row);
    ui->tblGoods->setItem(row, colName, new QTableWidgetItem(name));
    ui->tblGoods->setItem(row, colPrice, new QTableWidgetItem(QString::number(price, 'f', 2)));
    ui->tblGoods->setItem(row, colQty, new QTableWidgetItem(QString::number(qty, 'f', 3)));
    ui->tblGoods->setItem(row, colTaxDept, new QTableWidgetItem(QString::number(taxDept)));
    ui->tblGoods->setItem(row, colAdg, new QTableWidgetItem(adg));
    ui->tblGoods->setItem(row, colDiscount, new QTableWidgetItem(QString::number(discount, 'f', 2)));
    ui->tblGoods->setItem(row, colCode, new QTableWidgetItem(code));
}

void DlgFiscalTest::on_btnAddRow_clicked()
{
    addGoodsRow();
    scheduleSaveState();
}

void DlgFiscalTest::on_btnRemoveRow_clicked()
{
    const int row = ui->tblGoods->currentRow();
    if (row >= 0) {
        ui->tblGoods->removeRow(row);
        scheduleSaveState();
    }
}

void DlgFiscalTest::on_btnPrint_clicked()
{
    saveState();

    const QString ip = ui->leIp->text().trimmed();
    if (ip.isEmpty()) {
        QMessageBox::warning(this, tr("Test fiscal"), tr("Fiscal IP is required"));
        return;
    }

    const int port = ui->lePort->text().toInt();
    const QString password = ui->lePassword->text();
    const QString extPos = ui->cbExternalPos->isChecked() ? QStringLiteral("true") : QStringLiteral("false");
    const QString opPin = ui->leOpPin->text();
    const QString opPassword = ui->leOpPassword->text();
    const double cash = ui->leCash->text().replace(',', '.').toDouble();
    const double card = ui->leCard->text().replace(',', '.').toDouble();
    const double prepaid = ui->lePrepaid->text().replace(',', '.').toDouble();
    const bool simpleMode = currentPrintMode() == PrintMode::Simple;
    const int simpleTaxDept = ui->leSimpleTaxDept->text().toInt();

    struct GoodsRow {
        int taxDept;
        QString adg;
        QString code;
        QString name;
        double price;
        double qty;
        double discount;
    };
    QVector<GoodsRow> rows;

    if (!simpleMode) {
        if (ui->tblGoods->rowCount() == 0) {
            QMessageBox::warning(this, tr("Test fiscal"), tr("Add at least one goods row"));
            return;
        }

        rows.reserve(ui->tblGoods->rowCount());
        for (int r = 0; r < ui->tblGoods->rowCount(); ++r) {
            const QString name = cellText(ui->tblGoods, r, colName);
            if (name.isEmpty()) {
                continue;
            }
            GoodsRow g;
            g.name = name;
            g.price = cellDouble(ui->tblGoods, r, colPrice, 0.0);
            g.qty = cellDouble(ui->tblGoods, r, colQty, 1.0);
            g.taxDept = static_cast<int>(cellDouble(ui->tblGoods, r, colTaxDept, 1.0));
            g.adg = cellText(ui->tblGoods, r, colAdg);
            g.discount = cellDouble(ui->tblGoods, r, colDiscount, 0.0);
            g.code = cellText(ui->tblGoods, r, colCode);
            if (g.code.isEmpty()) {
                g.code = QString::number(r + 1);
            }
            rows.append(g);
        }

        if (rows.isEmpty()) {
            QMessageBox::warning(this, tr("Test fiscal"), tr("Fill goods name in at least one row"));
            return;
        }
    } else if (cash + card + prepaid <= 0.0) {
        QMessageBox::warning(this, tr("Test fiscal"), tr("Enter payment amount for simple mode"));
        return;
    }

    ui->btnPrint->setEnabled(false);
    ui->lblStatus->setText(tr("Printing..."));

    QPointer<DlgFiscalTest> self(this);
    auto *thread = new QThread(this);
    auto *pt = new PrintTaxN(ip, port, password, extPos, opPin, opPassword, nullptr);
    pt->moveToThread(thread);

    connect(thread, &QThread::started, pt, [pt, rows, cash, card, prepaid, simpleMode, simpleTaxDept, extPos]() {
        if (simpleMode) {
            pt->fJsonHeader[QStringLiteral("paidAmount")] = cash + card + prepaid;
            pt->makeJsonAndPrintSimple(simpleTaxDept, card, prepaid, extPos);
            return;
        }

        for (const GoodsRow &g : rows) {
            pt->addGoods(g.taxDept,
                         g.adg,
                         g.code,
                         g.name,
                         g.price,
                         g.qty,
                         g.discount);
        }
        pt->makeJsonAndPrint(cash, card, prepaid);
    });

    connect(pt, &PrintTaxN::finished, self, [self](const QString &inJson, const QString &outJson, const QString &err, int result) {
        if (!self) {
            return;
        }
        self->ui->btnPrint->setEnabled(true);
        if (result == pt_err_ok) {
            self->ui->lblStatus->setText(tr("Printed OK"));
            self->ui->teLog->setPlainText(
                QStringLiteral("IN:\n%1\n\nOUT:\n%2").arg(inJson, outJson));
            self->saveState();
            QMessageBox::information(self, tr("Test fiscal"), tr("Fiscal receipt printed"));
        } else {
            self->ui->lblStatus->setText(tr("Print failed"));
            self->ui->teLog->setPlainText(
                QStringLiteral("IN:\n%1\n\nOUT:\n%2\n\nERROR:\n%3\nRESULT: %4")
                    .arg(inJson, outJson, err)
                    .arg(result));
            self->saveState();
            QMessageBox::critical(self, tr("Test fiscal"), err.isEmpty() ? tr("Print failed") : err);
        }
    }, Qt::QueuedConnection);

    connect(pt, &PrintTaxN::finished, pt, &QObject::deleteLater);
    connect(pt, &PrintTaxN::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}
