#ifndef CONFIG_H
#define CONFIG_H

#include <QStringList>
#include <QJsonObject>

class Config
{
public:
    Config(const QStringList &dbParams, int id);
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

#endif // CONFIG_H
