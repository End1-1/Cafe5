#include "c5grouppermissionseditor.h"
#include "ui_c5grouppermissionseditor.h"
#include "c5permissions.h"
#include "c5selector.h"
#include "c5cache.h"
#include <QProxyStyle>
#include <QStyleOptionTab>

class CustomTabStyle : public QProxyStyle {
public:
  QSize sizeFromContents(ContentsType type, const QStyleOption* option,
                         const QSize& size, const QWidget* widget) const {
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
    if (type == QStyle::CT_TabBarTab) {
      s.transpose();
    }
    return s;
  }

  void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
    if (element == CE_TabBarTabLabel) {
      if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option)) {
        QStyleOptionTab opt(*tab);
        opt.shape = QTabBar::RoundedNorth;
        QProxyStyle::drawControl(element, &opt, painter, widget);
        return;
      }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
  }


};

C5GroupPermissionsEditor::C5GroupPermissionsEditor(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5GroupPermissionsEditor)
{
    ui->setupUi(this);
    fIcon = ":/permissions.png";
    fLabel = tr("Permissions");
    ui->tw->tabBar()->setStyle(new CustomTabStyle());
    getCheckBoxes(this);
}

C5GroupPermissionsEditor::~C5GroupPermissionsEditor()
{
    delete ui;
}

void C5GroupPermissionsEditor::setPermissionsGroupId(int id)
{
    fGroupId = id;
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_name from s_user_group where f_id=:f_id");
    if (db.nextRow()) {
        ui->lbGroup->setText(QString("%1").arg(db.getString(0)));
    }
    getPermissions();
}

QToolBar *C5GroupPermissionsEditor::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(savePemissions()));
    }
    return fToolBar;
}

void C5GroupPermissionsEditor::on_lbGroup_clicked()
{
    QList<QVariant> values;
    if (C5Selector::getValue(fDBParams, cache_users_groups, values)) {
        setPermissionsGroupId(values.at(0).toInt());
    }
}

void C5GroupPermissionsEditor::savePemissions()
{
    C5Database db(fDBParams);
    if (fGroupId == 1) {
        C5Message::info(tr("Nothig was saved"));
        return;
    }
    db[":f_group"] = fGroupId;
    db.exec("delete from s_user_access where f_group=:f_group");
    for (QMap<int, C5CheckBox*>::const_iterator it = fCheckBoxes.begin(); it != fCheckBoxes.end(); it++) {
        if (it.value()->isChecked()) {
            db[":f_group"] = fGroupId;
            db[":f_key"] = it.key();
            db[":f_value"] = 1;
            db.insert("s_user_access", false);
        }
    }
    C5Message::info(tr("Saved"));
}

void C5GroupPermissionsEditor::getPermissions()
{
    QList<C5CheckBox*> l = fCheckBoxes.values();
    foreach (C5CheckBox *c, l) {
        c->setChecked(false);
    }
    C5Database db(fDBParams);
    db[":f_group"] = fGroupId;
    db.exec("select f_key from s_user_access where f_group=:f_group");
    QSet<int> permissions;
    while (db.nextRow()) {
        permissions << db.getInt(0);
    }
    if (fGroupId == 1) {
        foreach (int id, C5Permissions::fTemplate) {
            permissions << id;
        }
    }
    QString err;
    foreach (int p, permissions) {
        if (!fCheckBoxes.contains(p)) {
            err += QString::number(p) + "<br>";
        } else {
            fCheckBoxes[p]->setChecked(true);
        }
    }
}

void C5GroupPermissionsEditor::getCheckBoxes(QWidget *parent)
{
    QObjectList ol = parent->children();
    foreach (QObject *o, ol) {
        if (o->children().count() > 0) {
            getCheckBoxes(static_cast<QWidget*>(o));
        }
        if (strcmp(o->metaObject()->className(), "C5CheckBox") == 0) {
            C5CheckBox *cb = static_cast<C5CheckBox*>(o);
            fCheckBoxes[cb->getTag()] = cb;
        }
    }
}

void C5GroupPermissionsEditor::setChecked(int start, int end, bool v)
{
    for (QMap<int, C5CheckBox*>::const_iterator it = fCheckBoxes.begin(); it != fCheckBoxes.end(); it++) {
        if (it.key() >= start && it.key() <= end) {
            it.value()->setChecked(v);
        }
    }
}

void C5GroupPermissionsEditor::on_chPreferences_clicked(bool checked)
{
    setChecked(100, 199, checked);
}

void C5GroupPermissionsEditor::on_chMenu_clicked(bool checked)
{
    setChecked(400, 499, checked);
}

void C5GroupPermissionsEditor::on_chGoods_clicked(bool checked)
{
    setChecked(600, 699, checked);
}

void C5GroupPermissionsEditor::on_chActions_clicked(bool checked)
{
    setChecked(200, 299, checked);
}

void C5GroupPermissionsEditor::on_chWaiterAllItems_clicked(bool checked)
{
    setChecked(500, 599, checked);
}

void C5GroupPermissionsEditor::on_chREportsAllItems_clicked(bool checked)
{
    setChecked(300, 399, checked);
}

void C5GroupPermissionsEditor::on_chOtherAllItems_clicked(bool checked)
{
    setChecked(700, 799, checked);
}
