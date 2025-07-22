#include "supplier.h"
#include "ui_supplier.h"
#include "c5database.h"

supplier::supplier() :
    C5Dialog(),
    ui(new Ui::supplier)
{
    ui->setupUi(this);
    C5Database db;
    db.exec("select f_id, f_name from h_tables order by f_name");

    while(db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setData(Qt::UserRole, db.getInt(0));
        item->setText(db.getString(1));
        item->setSizeHint(QSize(100, 50));
        ui->lst->addItem(item);
    }
}

supplier::~supplier()
{
    delete ui;
}

bool supplier::getSupplier(int& id, QString &name)
{
    bool result = false;
    supplier s;

    if(s.exec() == QDialog::Accepted) {
        result = true;
        id = s.fId;
        name = s.fName;
    }

    return result;
}

void supplier::on_btnCancel_clicked()
{
    reject();
}

void supplier::on_btnOK_clicked()
{
    if(ui->lst->currentIndex().row() < 0) {
        return;
    }

    QListWidgetItem *item = ui->lst->currentItem();
    fId = item->data(Qt::UserRole).toInt();
    fName = item->text();
    accept();
}
