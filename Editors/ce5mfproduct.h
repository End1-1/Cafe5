#ifndef CE5MFPRODUCT_H
#define CE5MFPRODUCT_H

#include "ce5editor.h"

namespace Ui {
class CE5MFProduct;
}

class CE5MFProduct : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MFProduct(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5MFProduct();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id);

    virtual void clear();

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

private slots:
    void durationChanged(const QString &arg1);

    void goalPriceChanged(const QString &arg1);

    void on_btnAdd_clicked();

    void on_btnMinus_clicked();

    void on_btnNew_clicked();

    void on_btnPrint_clicked();

private:
    Ui::CE5MFProduct *ui;

    QString durationStr(int sec);
};

#endif // CE5MFPRODUCT_H
