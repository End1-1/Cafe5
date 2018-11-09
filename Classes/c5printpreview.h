#ifndef C5PRINTPREVIEW_H
#define C5PRINTPREVIEW_H

#include <QDialog>

namespace Ui {
class C5PrintPreview;
}

class C5PrintPreview : public QDialog
{
    Q_OBJECT

public:
    explicit C5PrintPreview(QWidget *parent = 0);
    ~C5PrintPreview();

private:
    Ui::C5PrintPreview *ui;
};

#endif // C5PRINTPREVIEW_H
