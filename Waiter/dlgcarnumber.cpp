#include "dlgcarnumber.h"
#include "ui_dlgcarnumber.h"
#include "c5database.h"
#include "c5message.h"
#include <QListWidgetItem>
#include <QScreen>

QMap<int, QString> DlgCarNumber::fCarList;
QMap<int, QString> DlgCarNumber::fCostumerNames;
QMap<int, QString> DlgCarNumber::fCostumerPhones;
QMap<int, Car> DlgCarNumber::fCostumerCars;

DlgCarNumber::DlgCarNumber() :
    C5Dialog(),
    ui(new Ui::DlgCarNumber)
{
    ui->setupUi(this);
    ui->wlist->setVisible(false);
    fRecordId = 0;
    C5Database db;

    if(fCarList.count() == 0) {
        db.exec("select f_id, f_name from s_car order by f_name");

        while(db.nextRow()) {
            fCarList.insert(db.getInt(0), db.getString(1));
        }
    }

    if(fCostumerCars.count() == 0) {
        db.exec("select f_id, f_car, f_costumer, f_govnumber from b_car ");

        while(db.nextRow()) {
            Car c;
            c.fCar = db.getInt("f_car");
            c.fCostumer = db.getInt("f_costumer");
            c.fNumber = db.getString("f_govnumber");
            fCostumerCars.insert(db.getInt("f_id"), c);
        }
    }

    if(fCostumerNames.count() == 0) {
        db.exec("select f_id, trim(f_contact), f_info from c_partners order by 1");

        while(db.nextRow()) {
            fCostumerNames.insert(db.getInt(0), db.getString(1));
            fCostumerPhones.insert(db.getInt(0), db.getString(2));
        }
    }

    resetMode();
    connect(ui->wKbd, SIGNAL(textChanged(QString)), this, SLOT(kbdText(QString)));
    connect(ui->wKbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->wKbd, SIGNAL(reject()), this, SLOT(kbdReject()));
    adjustSize();
}

DlgCarNumber::~DlgCarNumber()
{
    delete ui;
}

bool DlgCarNumber::getNumber(int& id)
{
    id = 0;
    DlgCarNumber *d = new DlgCarNumber();
    d->move((qApp->screens().at(0)->geometry().width() / 2) - (d->width() / 2), 50);

    if(d->exec() == QDialog::Accepted) {
        id = d->fRecordId;
    }

    delete d;
    return id > 0;
}

void DlgCarNumber::kbdText(const QString &txt)
{
    if(fModel || fCarNumber || fCostumer) {
        for(int i = 0; i < ui->lst->count(); i++) {
            ui->lst->item(i)->setHidden(!ui->lst->item(i)->text().contains(txt, Qt::CaseInsensitive));
        }
    }
}

void DlgCarNumber::kbdAccept()
{
    if(fCarNumber) {
        ui->leCarNumber->setText(ui->wKbd->text().toUpper());

        if(!ui->wKbd->text().isEmpty()) {
            QList<QListWidgetItem*> l = ui->lst->findItems(ui->wKbd->text(), Qt::MatchStartsWith);

            if(l.count() > 0) {
                if(l.count() == 1) {
                }
            }
        }
    }

    if(fCostumer) {
        ui->leCostumer->setText(ui->wKbd->text().toUpper());
    }

    if(fAdditional) {
        ui->leAdditional->setText(ui->wKbd->text().toUpper());
    }

    hideKbd();
}

void DlgCarNumber::kbdReject()
{
    hideKbd();
}

void DlgCarNumber::on_btnCar_clicked()
{
    fModel = true;
    makeMap(fCarList);
}

void DlgCarNumber::on_lst_itemClicked(QListWidgetItem *item)
{
    if(!item) {
        return;
    }

    if(fModel) {
        ui->leCarModel->setText(item->text());
        ui->leCarModel->setProperty("id", item->data(Qt::UserRole).toInt());
    }

    if(fCarNumber) {
        ui->leCarNumber->setText(item->text());

        if(item->data(Qt::UserRole).toInt() > 0) {
            fRecordId = item->data(Qt::UserRole).toInt();
            Car c = fCostumerCars[fRecordId];
            ui->leCostumer->setText(fCostumerNames[c.fCostumer]);
            ui->leCostumer->setProperty("id", c.fCostumer);
            ui->leCarModel->setText(fCarList[c.fCar]);
            ui->leCarModel->setProperty("id", c.fCar);
            ui->leAdditional->setText(fCostumerPhones[c.fCostumer]);
        }
    }

    if(fCostumer) {
        ui->leCostumer->setText(item->text());
        ui->leCostumer->setProperty("id", item->data(Qt::UserRole).toInt());
        ui->leAdditional->setText(fCostumerPhones[item->data(Qt::UserRole).toInt()]);
    }

    if(fAdditional) {
        ui->leAdditional->setText(item->text());
        ui->leCostumer->setText(fCostumerNames[item->data(Qt::UserRole).toInt()]);
    }

    hideKbd();
}

void DlgCarNumber::hideKbd()
{
    resetMode();
    ui->wlist->setVisible(false);
    ui->wok->setVisible(true);
    adjustSize();
}

void DlgCarNumber::showKbd()
{
    ui->wKbd->setText("");
    ui->wlist->setVisible(true);
    ui->wok->setVisible(false);
    adjustSize();
}

void DlgCarNumber::resetMode()
{
    fModel = false;
    fCarNumber = false;
    fCostumer = false;
    fAdditional = false;
    ui->wKbd->setText("");
}

void DlgCarNumber::on_btnClose_clicked()
{
    reject();
}

void DlgCarNumber::on_btnAccept_clicked()
{
    C5Database db;

    if(fRecordId == 0) {
        if(ui->leCarModel->property("id").toInt() == 0) {
            C5Message::error(tr("Car model is not selected"));
            return;
        }

        if(ui->leCarNumber->text().isEmpty()) {
            C5Message::error(tr("Car goverment number is empty"));
            return;
        }

        if(ui->leCostumer->text().isEmpty()) {
            C5Message::error(tr("Costumer name is empty"));
            return;
        }

        db[":f_govnumber"] = ui->leCarNumber->text();
        db.exec("select * from b_car where f_govnumber=:f_govnumber");

        if(db.nextRow()) {
            fRecordId = db.getInt("f_id");
            ui->leCostumer->setText(fCostumerNames[db.getInt("f_costumer")]);
            ui->leCostumer->setProperty("id", db.getInt("f_costumer"));
            ui->leAdditional->setText(fCostumerPhones[db.getInt("f_costumer")]);
            ui->leCarModel->setText(fCarList[db.getInt("f_car")]);
            ui->leCarModel->setProperty("id", db.getInt("f_car"));
        }

        if(ui->leCostumer->property("id").toInt() == 0) {
            db[":f_contact"] = ui->leCostumer->text();
            db[":f_info"] = ui->leAdditional->text();
            ui->leCostumer->setProperty("id", db.insert("c_partners"));
        }

        if(fRecordId == 0) {
            db[":f_govnumber"] = ui->leCarNumber->text();
            db[":f_car"] = ui->leCarModel->property("id").toInt();
            db[":f_costumer"] = ui->leCostumer->property("id").toInt();
            fRecordId = db.insert("b_car");
        }
    } else {
        db[":f_contact"] = ui->leCostumer->text();
        db[":f_info"] = ui->leAdditional->text();
        db[":f_id"] = ui->leCostumer->property("id").toInt();
        db.exec("update c_partners set f_contact=:f_contact, f_info=:f_info where f_id=:f_id");
    }

    fCostumerCars.clear();
    fCostumerNames.clear();
    fCostumerPhones.clear();

    if(fRecordId > 0) {
        accept();
    }
}

void DlgCarNumber::on_btnSetNumber_clicked()
{
    fCarNumber = true;
    QMap<int, QString> cars;

    for(QMap<int, Car>::const_iterator it = fCostumerCars.begin(); it != fCostumerCars.end(); it++) {
        cars.insert(it.key(), it.value().fNumber);
    }

    makeMap(cars);
    showKbd();
}

void DlgCarNumber::on_btnCostumer_clicked()
{
    fCostumer = true;
    makeMap(fCostumerNames);
}

void DlgCarNumber::makeMap(const QMap<int, QString>& map)
{
    ui->lst->clear();

    for(QMap<int, QString>::const_iterator it = map.begin(); it != map.end(); it++) {
        if(it.value().isEmpty()) {
            continue;
        }

        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(100, 40));
        item->setText(it.value());
        item->setData(Qt::UserRole, it.key());
        ui->lst->addItem(item);
    }

    showKbd();
}

void DlgCarNumber::on_btnAdditional_clicked()
{
    fAdditional = true;
    makeMap(fCostumerPhones);
}
