#ifndef CR5MENUTRANSLATOR_H
#define CR5MENUTRANSLATOR_H

#include "c5widget.h"
#include <QJsonObject>

namespace Ui
{
class CR5MenuTranslator;
}

class CR5MenuTranslator : public C5Widget
{
    Q_OBJECT

public:
    explicit CR5MenuTranslator(QWidget *parent = nullptr);

    ~CR5MenuTranslator();

    QToolBar *toolBar();

    void setMode(int mode);

private slots:
    void on_leSearch_textChanged(const QString &arg1);

    void on_btnClearSearch_clicked();

    void saveDataChanges();

    void saveResponse(const QJsonObject &jo);

private:
    Ui::CR5MenuTranslator *ui;

    int fMode;
};

#endif // CR5MENUTRANSLATOR_H
