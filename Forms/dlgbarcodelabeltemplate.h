#ifndef DLGBARCODELABELTEMPLATE_H
#define DLGBARCODELABELTEMPLATE_H

#include <QDialog>

namespace Ui
{
class DlgBarcodeLabelTemplate;
}

class DlgBarcodeLabelTemplate : public QDialog
{
    Q_OBJECT

public:
    explicit DlgBarcodeLabelTemplate(QWidget *parent = nullptr);
    ~DlgBarcodeLabelTemplate();

    /** Show picker; on accept saves selection to local reg. Returns true if accepted. */
    static bool selectTemplate(QWidget *parent);

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DlgBarcodeLabelTemplate *ui;
};

#endif // DLGBARCODELABELTEMPLATE_H
