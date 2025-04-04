#ifndef C5SETTINGSWIDGET_H
#define C5SETTINGSWIDGET_H

#include "ce5editor.h"

namespace Ui
{
class C5SettingsWidget;
}

class C5SettingsWidget : public CE5Editor
{
    Q_OBJECT

public:
    explicit C5SettingsWidget(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5SettingsWidget();

    virtual QString title() override
    {
        return tr("Settings");
    }

    virtual QString table() override
    {
        return "s_settings_names";
    }

    virtual void setId(int id) override;

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

    void clear(QWidget *parent)  ;

    virtual bool canCopy() override;

    virtual void copyObject() override;

private slots:

    void on_btnScalePath_clicked();

private:
    Ui::C5SettingsWidget *ui;

    int fSettingsId;

    void clearWidgetValue(QWidget *w);

    void setWidgetValue(QWidget *w, const QString &value);

    QWidget *widget(QWidget *parent, int tag);
};

#endif // C5SETTINGSWIDGET_H
