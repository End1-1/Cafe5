#ifndef CR5MENUTRANSLATOR_H
#define CR5MENUTRANSLATOR_H

#include "c5widget.h"

namespace Ui {
class CR5MenuTranslator;
}

class CR5MenuTranslator : public C5Widget
{
    Q_OBJECT

public:
    explicit CR5MenuTranslator(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5MenuTranslator();

    QToolBar *toolBar();

private slots:
    void on_leSearch_textChanged(const QString &arg1);

    void on_btnClearSearch_clicked();

    void saveDataChanges();

private:
    Ui::CR5MenuTranslator *ui;
};

#endif // CR5MENUTRANSLATOR_H
