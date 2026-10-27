#include "dlgsemireadyinout.h"
#include "ui_dlgsemireadyinout.h"
#include "c5selector.h"
#include "c5database.h"
#include "c5utils.h"
#include <QMap>
#include <QSet>
#include <QVector>

DlgSemireadyInOut::DlgSemireadyInOut(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgSemireadyInOut)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 0);
    ui->tbl->setColumnWidth(1, 300);
    ui->tbl->setColumnWidth(2, 80);
    // textChanged: covers typing, paste, setDouble; textEdited alone can be missed with C5LineEdit.
    connect(ui->leQty, &QLineEdit::textChanged, this, &DlgSemireadyInOut::rescaleIngredients);
}

DlgSemireadyInOut::~DlgSemireadyInOut()
{
    delete ui;
}

int DlgSemireadyInOut::dishId()
{
    return ui->leGoods->property("f_base").toInt();
}

QString DlgSemireadyInOut::dishName()
{
    return ui->leGoods->text();
}

double DlgSemireadyInOut::dishQty()
{
    return ui->leQty->getDouble();
}

int DlgSemireadyInOut::rowCount()
{
    return ui->tbl->rowCount();
}

int DlgSemireadyInOut::goodsId(int r)
{
    return ui->tbl->getInteger(r, 0);
}

QString DlgSemireadyInOut::goodsName(int r)
{
    return ui->tbl->getString(r, 1);
}

double DlgSemireadyInOut::goodsQty(int r)
{
    return ui->tbl->getDouble(r, 2);
}

void DlgSemireadyInOut::on_btnSelectGoods_clicked()
{
    QJsonArray vals;
    QHash<QString, QString> trans;

    if(!C5Selector::getValues(mUser,
                              "select distinct(g.f_id), g.f_name, g.f_complectout, u.f_name as f_unitname "
                              "from c_goods_complectation gc "
                              "left join c_goods g on g.f_id=gc.f_base "
                              "left join c_units u on u.f_id=g.f_unit ", vals, trans)) {
        return;
    }

    ui->tbl->setRowCount(0);
    mRefDishQty = vals.at(3).toDouble();
    if(mRefDishQty < 0.0000001) {
        mRefDishQty = 1.0;
    }
    ui->leQty->setProperty("qty", mRefDishQty);
    ui->lbMeas->setText(vals.at(4).toString());
    ui->leGoods->setText(vals.at(2).toString());
    const int baseId = vals.at(1).toInt();
    ui->leGoods->setProperty("f_base", baseId);

    // Flatten nested complects into unique leaves (sum qty if same goods via several paths).
    // Root recipe uses raw f_qty; nested levels use f_qty / f_complectout * parentQty.
    struct Frame {
        int id = 0;
        double mult = 1.0;
        bool nested = false;
        QSet<int> path;
    };
    struct Leaf {
        QString name;
        double qty = 0;
    };

    C5Database db;
    QMap<int, Leaf> leaves;
    QVector<Frame> stack;
    stack.append(Frame{baseId, 1.0, false, {}});

    while(!stack.isEmpty()) {
        const Frame frame = stack.takeLast();
        if(frame.id <= 0 || frame.mult <= 0.0000001) {
            continue;
        }
        if(frame.path.contains(frame.id)) {
            continue;
        }

        db[":f_base"] = frame.id;
        db.exec("select g.f_name, gc.f_goods, gc.f_qty, "
                "coalesce(nullif(base.f_complectout, 0), 1) as f_complectout "
                "from c_goods_complectation gc "
                "left join c_goods g on g.f_id=gc.f_goods "
                "left join c_goods base on base.f_id=gc.f_base "
                "where gc.f_base=:f_base ");

        struct Comp {
            int id;
            QString name;
            double qty;
            double complectOut;
        };
        QVector<Comp> comps;
        while(db.nextRow()) {
            comps.append(Comp{
                db.getInt("f_goods"),
                db.getString("f_name"),
                db.getDouble("f_qty"),
                db.getDouble("f_complectout")
            });
        }

        if(comps.isEmpty()) {
            // Should not happen for the selected root; for a stuck nested node keep as leaf.
            if(frame.nested) {
                Leaf &leaf = leaves[frame.id];
                if(leaf.name.isEmpty()) {
                    db[":f_id"] = frame.id;
                    db.exec("select f_name from c_goods where f_id=:f_id");
                    if(db.nextRow()) {
                        leaf.name = db.getString("f_name");
                    }
                }
                leaf.qty += frame.mult;
            }
            continue;
        }

        QSet<int> childPath = frame.path;
        childPath.insert(frame.id);

        for(const Comp &c : comps) {
            if(c.id <= 0 || childPath.contains(c.id)) {
                continue;
            }
            const double lineQty = frame.nested
                    ? (c.qty / c.complectOut) * frame.mult
                    : c.qty * frame.mult;

            db[":f_base"] = c.id;
            db.exec("select 1 from c_goods_complectation where f_base=:f_base limit 1");
            if(db.nextRow()) {
                stack.append(Frame{c.id, lineQty, true, childPath});
            } else {
                Leaf &leaf = leaves[c.id];
                if(leaf.name.isEmpty()) {
                    leaf.name = c.name;
                }
                leaf.qty += lineQty;
            }
        }
    }

    for(auto it = leaves.constBegin(); it != leaves.constEnd(); ++it) {
        if(it.value().qty <= 0.0000001) {
            continue;
        }
        const int r = ui->tbl->addEmptyRow();
        ui->tbl->setInteger(r, 0, it.key());
        ui->tbl->setString(r, 1, it.value().name);
        ui->tbl->setDouble(r, 2, it.value().qty);
        if(QTableWidgetItem *item = ui->tbl->item(r, 2)) {
            // Amount for mRefDishQty of the selected complect.
            item->setData(Qt::UserRole, it.value().qty);
        }
    }

    QSignalBlocker blocker(ui->leQty);
    ui->leQty->setDouble(mRefDishQty);
}

void DlgSemireadyInOut::on_btnCancel_clicked()
{
    reject();
}

void DlgSemireadyInOut::on_leQty_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    rescaleIngredients();
}

void DlgSemireadyInOut::rescaleIngredients()
{
    double baseQty = ui->leQty->property("qty").toDouble();
    if(baseQty < 0.0000001) {
        baseQty = mRefDishQty;
    }
    if(baseQty < 0.0000001) {
        return;
    }

    const double newQty = ui->leQty->getDouble();
    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        QTableWidgetItem *it = ui->tbl->item(i, 2);
        if(!it) {
            continue;
        }
        const double refComp = it->data(Qt::UserRole).toDouble();
        ui->tbl->setDouble(i, 2, (refComp / baseQty) * newQty);
    }
}

void DlgSemireadyInOut::on_btnOk_clicked()
{
    accept();
}
