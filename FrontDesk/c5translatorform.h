#ifndef C5TRANSLATORFORM_H
#define C5TRANSLATORFORM_H

#include "c5widget.h"

namespace Ui {
class C5TranslatorForm;
}

class C5TranslatorForm : public C5Widget
{
    Q_OBJECT

public:
    explicit C5TranslatorForm(const QStringList &dbParams, QWidget *parent = 0);

    ~C5TranslatorForm();

    virtual QToolBar *toolBar();

private:
    Ui::C5TranslatorForm *ui;

private slots:
    void saveDataChanges();
};

#endif // C5TRANSLATORFORM_H
