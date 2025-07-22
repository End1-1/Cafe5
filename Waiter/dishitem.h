#ifndef DISHITEM_H
#define DISHITEM_H

#include <QWidget>
#include <QProxyStyle>
#include <QJsonObject>

namespace Ui
{
class DishItem;
}

class DishItem : public QWidget
{
    Q_OBJECT

public:
    explicit DishItem(const QMap<QString, QVariant>& d, int index, QWidget *parent = nullptr);

    ~DishItem();

    QMap<QString, QVariant> fData;

    void setChanges();

    bool isFocused();

    void setCheckMode(bool v);

    void setChecked(bool v);

    bool isChecked();

    void setReadyOnly(bool v);

public slots:
    void checkFocus(const QString &id);

protected:
    virtual bool event(QEvent *event);

private slots:
    void on_btnDish_clicked();

private:
    Ui::DishItem* ui;

    bool fFocus;

    bool fReadOnly;

signals:
    void focused(QString);
};

#endif // DISHITEM_H
