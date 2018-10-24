#ifndef C5DIALOG_H
#define C5DIALOG_H

#include <QDialog>
#include <QJsonArray>
#include "c5config.h"
#include "c5message.h"
#include "c5sockethandler.h"
#include "c5permissions.h"
#include "c5utils.h"

class C5Dialog : public QDialog
{
    Q_OBJECT
public:
    C5Dialog(const QStringList &dbParams, QWidget *parent = 0);

    virtual bool preambule();

    template<typename T>
    static bool go(const QStringList &dbParams) {
        T *t = new T(dbParams, __c5config.fParentWidget);
        bool result = false;
        if (t->preambule()) {
            result = t->exec() == QDialog::Accepted;
        }
        delete t;
        return result;
    }

    C5SocketHandler *createSocketHandler(const char *slot);

protected:
    const QStringList &fDBParams;

protected slots:
    void handleError(int err, const QString &msg);

signals:
    void escape();
};

#endif // C5DIALOG_H
