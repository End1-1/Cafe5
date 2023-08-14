#ifndef C5GROUPPERMISSIONSEDITOR_H
#define C5GROUPPERMISSIONSEDITOR_H

#include "c5widget.h"

class C5CheckBox;

namespace Ui {
class C5GroupPermissionsEditor;
}

class C5GroupPermissionsEditor : public C5Widget
{
    Q_OBJECT

public:
    explicit C5GroupPermissionsEditor(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5GroupPermissionsEditor();

    void setPermissionsGroupId(int id);

    virtual QToolBar *toolBar();

private slots:
    void on_lbGroup_clicked();

    void savePemissions();

    void getPermissions();

    void on_chPreferences_clicked(bool checked);

    void on_chMenu_clicked(bool checked);

    void on_chGoods_clicked(bool checked);

    void on_chActions_clicked(bool checked);

    void on_chWaiterAllItems_clicked(bool checked);

    void on_chREportsAllItems_clicked(bool checked);

    void on_chOtherAllItems_clicked(bool checked);

    void on_chCash_clicked(bool checked);

    void on_chSalaryAll_clicked(bool checked);

    void on_chManufactureAvailbility_clicked(bool checked);

private:
    Ui::C5GroupPermissionsEditor *ui;

    int fGroupId;

    QMap<int, C5CheckBox*> fCheckBoxes;

    void getCheckBoxes(QWidget *parent);

    void setChecked(int start, int end, bool v);
};

#endif // C5GROUPPERMISSIONSEDITOR_H
