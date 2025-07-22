#ifndef CONFIGS_H
#define CONFIGS_H

#include <QStringList>
#include <QJsonObject>

class Configs
{
public:
    Configs(int id);
    virtual void readValues() = 0;

    template<class T>
    static T* construct(int id)
    {
        T *t = new T(id);
        t->readValues();
        return t;
    }

protected:
    QJsonObject jo;

private:
    int fId;
};

#endif // CONFIGS_H
