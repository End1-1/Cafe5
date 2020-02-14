#ifndef C5DATABASESYNC_H
#define C5DATABASESYNC_H

#include "c5threadobject.h"

class C5DatabaseSync : public C5ThreadObject
{
    Q_OBJECT
public:
    C5DatabaseSync(QObject *parent = nullptr);

protected slots:
    virtual void run() override;
};

#endif // C5DATABASESYNC_H
