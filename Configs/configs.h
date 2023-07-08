#ifndef CONFIGS_H
#define CONFIGS_H

#include <QStringList>
#include <QJsonObject>

class Configs
{
public:
    Configs(const QStringList &dbParams, int id);
    virtual void readValues() = 0;

    template<class T>
    static T* construct(const QStringList &dbParams, int id) {
        T *t = new T(dbParams, id);
        t->readValues();
        return t;
    }

protected:
    QJsonObject jo;

private:
    QStringList fDbParams;
    int fId;
};

#endif // CONFIGS_H
