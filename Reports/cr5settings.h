#ifndef CR5SETTINGS_H
#define CR5SETTINGS_H

#include "c5reportwidget.h"

class C5SettingsWidget;

class CR5Settings : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Settings(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private:
    C5SettingsWidget *fSettingsWidget;

private slots:
    void removeSettings();
};

#endif // CR5SETTINGS_H
