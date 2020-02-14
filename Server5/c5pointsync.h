#ifndef C5POINTSYNC_H
#define C5POINTSYNC_H

#include "c5threadobject.h"

class C5PointSync : public C5ThreadObject
{
    Q_OBJECT
public:
    C5PointSync(const QStringList &mainDbParams, const QStringList &dbParams, QObject *parent = nullptr);

protected:
    virtual void run() override;

private:
    QStringList fMainDbParams;
    QStringList fDbParams;
    void doSync();

private slots:
    void timeout();
};

#endif // C5POINTSYNC_H
