#ifndef C5SETTINGSWIDGET_H
#define C5SETTINGSWIDGET_H

#include "ce5editor.h"

namespace Ui {
class C5SettingsWidget;
}

class C5SettingsWidget : public CE5Editor
{
    Q_OBJECT

public:
    explicit C5SettingsWidget(const QStringList &dbParams, QWidget *parent = 0);

    ~C5SettingsWidget();

    virtual QString title() {return tr("Settings");}

    virtual QString table() {return "s_settings_names";}

    virtual void setId(int id);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

    void clear(QWidget *parent);

private:
    Ui::C5SettingsWidget *ui;

    int fSettingsId;

    void clearWidgetValue(QWidget *w);

    void setWidgetValue(QWidget *w, const QString &value);

    QWidget *widget(QWidget *parent, int tag);
};

#endif // C5SETTINGSWIDGET_H
