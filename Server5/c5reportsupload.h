#ifndef C5REPORTSUPLOAD_H
#define C5REPORTSUPLOAD_H

#include <QDialog>

namespace Ui {
class C5ReportsUpload;
}

class C5ReportsUpload : public QDialog
{
    Q_OBJECT

public:
    explicit C5ReportsUpload(QWidget *parent = nullptr);
    ~C5ReportsUpload();

private:
    Ui::C5ReportsUpload *ui;
};

#endif // C5REPORTSUPLOAD_H
