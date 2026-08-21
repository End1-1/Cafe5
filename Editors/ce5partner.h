#ifndef CE5PARTNER_H
#define CE5PARTNER_H

#include "ce5editor.h"

namespace Ui
{
class CE5Partner;
}

class CE5Partner : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Partner(QWidget *parent = nullptr);

    ~CE5Partner();

    virtual QString title() override;

    virtual QString table() override;

    virtual void setId(int id) override;

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data) override;

    virtual bool checkData(QString &err) override;

    virtual bool isOnline() override;

    virtual bool canCopy() override;

    virtual void copyObject() override;

private slots:
    void saveResponse(const QJsonObject &jdoc);

    void on_btnClearManager_clicked();

private:
    QJsonObject makeSaveJson() const;

    QMap<QString, QVariant> makeResultRow(int id) const;

    void savePartner(bool closeEditor);

    Ui::CE5Partner *ui;

    bool fCloseOnSaveResponse = true;
};

#endif // CE5PARTNER_H
