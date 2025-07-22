#ifndef C5DIALOG_H
#define C5DIALOG_H

#include <QDialog>
#include <QJsonArray>
#include "ninterface.h"

class C5Dialog : public QDialog
{
    Q_OBJECT

public:
    C5Dialog();

    C5Dialog(bool noparent);

    ~C5Dialog();

    static void setMainWindow(QWidget *widget);

    virtual bool preambule();

    template<typename T>
    static bool go()
    {
        T *t = new T();
        bool result = false;

        if(t->preambule()) {
            result = t->exec() == QDialog::Accepted;
        }

        delete t;
        return result;
    }

    void showFullScreen();

    virtual void selectorCallback(int row, const QJsonArray &values);

    void setFieldValue(const QString &name, const QString &value);

    void setFieldValue(const QString &name, double value);

    void setFieldValue(const QString &name, int value);

    int getFieldIntValue(const QString &name);

    double getFieldDoubleValue(const QString &name);

    QString getFieldStringValue(const QString &name);

protected:

    virtual void keyPressEvent(QKeyEvent *e);

    virtual void keyEnter();

    virtual void keyControlPlusEnter();

    virtual void keyAlterPlusEnter();

    NInterface* fHttp;

protected slots:
    void handleError(int err, const QString &msg);

signals:
    void escape();
};

#endif // C5DIALOG_H
