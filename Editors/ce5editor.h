#ifndef CE5EDITOR_H
#define CE5EDITOR_H

#include "c5officewidget.h"
#include "c5editor.h"
#include <QSet>
#include <QJsonObject>

class C5LineEditWithSelector;
class C5CheckBox;
class C5DateEdit;
class C5ComboBox;
class QPlainTextEdit;

class CE5Editor : public C5OfficeWidget
{
    Q_OBJECT

public:
    CE5Editor(QWidget *parent = nullptr);

    ~CE5Editor();

    virtual void setId(int id);

    virtual QString title() = 0;

    virtual QString table() = 0;

    virtual QString dbError(QString err);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> >& data);

    virtual bool checkData(QString &err);

    virtual bool isOnline();

    virtual void clear();

    virtual bool canCopy();

    virtual void copyObject();

    virtual QPushButton* b1();

    void getLineEdit(QObject *parent);

    void focusFirst();

    virtual QJsonObject makeJsonObject();

    virtual bool acceptOnSave() const;

    QWidget* fEditor;

    QJsonObject fJsonData;

    template<typename T>
    bool getId(QString &id)
    {
        T *ep = new T();
        C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
        QList<QMap<QString, QVariant> > data;
        bool result = e->getResult(data);

        if(result) {
            id = data.at(0)["f_id"].toString();
        }

        delete e;
        return result;
    }

protected:
    bool fRememberFields;

private:
    C5LineEditWithSelector* findLineEditWithId();

    QSet<C5LineEditWithSelector*> fLines;

    QSet<C5ComboBox*> fCombos;

    QSet<C5CheckBox*> fChecks;

    QSet<C5DateEdit*> fDates;

    QSet<QPlainTextEdit*> fPlainText;

signals:
    void Accept();
};

#endif // CE5EDITOR_H
