#pragma once

#include <QFrame>

namespace Ui
{
class C5CodeNameSelector;
}

class C5CodeNameSelector : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString selectorName READ selectorName WRITE setSelectorName NOTIFY selectorNameChanged)
public:
    explicit C5CodeNameSelector(QWidget *parent = nullptr);

    ~C5CodeNameSelector();

    std::function<void(C5CodeNameSelector*)> selectorCallback;

    void setCodeAndName(int code, const QString &name);

    void setName(const QString &name);

    QString name() const;

    QString selectorName() const;

    void setSelectorName(const QString &s);

    QPoint getPosition() const;

    int value();

private slots:
    void on_btnOpenSelector_clicked();

private:
    Ui::C5CodeNameSelector* ui;

    QString mSelectorName;

signals:
    void selectorNameChanged();
};
