#pragma once

#include "rabstractspecialwidget.h"
#include <QJsonObject>

class QLabel;

class RWorkstationConfigWidget : public RAbstractSpecialWidget
{
    Q_OBJECT
public:
    explicit RWorkstationConfigWidget(QWidget *parent = nullptr);

    void setup(const QJsonObject &jdoc) override;

    void save() override;

    static RWorkstationConfigWidget *createForType(int type, QWidget *parent, C5User *user);

    void setEditorKey(const QString &key) { mEditorKey = key; }

    void addFormWidget(QWidget *widget);

protected:
    virtual void applyConfig(const QJsonObject &config) = 0;

    virtual QJsonObject collectConfig() const = 0;

    void setHeaderInfo(const QJsonObject &jdoc);

    int workstationId() const { return mId; }

    QString editorKey() const { return mEditorKey; }

private:
    int mId = 0;

    int mType = 0;

    QString mEditorKey = QStringLiteral("form_workstations");

    QLabel *mLbTitle = nullptr;
};
