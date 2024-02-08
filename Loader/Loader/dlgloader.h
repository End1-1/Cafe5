#ifndef DLGLOADER_H
#define DLGLOADER_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgLoader; }
QT_END_NAMESPACE

class DlgLoader : public QDialog
{
    Q_OBJECT

public:
    DlgLoader(QWidget *parent = nullptr);
    ~DlgLoader();
    void checkForUpdate();

private:
    Ui::DlgLoader *ui;
};
#endif // DLGLOADER_H
