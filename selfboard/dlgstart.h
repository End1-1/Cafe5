#ifndef DLGSTART_H
#define DLGSTART_H

#include <QDialog>
#include <QLabel>

#include "dlgmenu.h"

class QShowEvent;
class QKeyEvent;

class QButtonGroup;

namespace Ui {
class DlgStart;
}

class DlgStart : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStart(QWidget *parent = nullptr);
    ~DlgStart() override;

protected:
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onLanguageClicked(int id);
    void onTakeAwayClicked();
    void onDineInClicked();
    void openMenu(DlgMenu::ServiceMode mode);

    void startMenuPreload();

private:
    void setupAppearance();
    void setupBackground();
    void setupIcons();
    void setupLanguageGroup();
    void applyLanguage(const QString &localeCode);
    void updateLanguageButtonIcons();
    void setMenuLoadingUi(bool loading, const QString &message = QString());
    void updateLoadingOverlayGeometry();
    void openServerSettings(bool required = false);
    void runStartup();

    Ui::DlgStart *ui;
    bool m_startupDone = false;
    QLabel *m_loadingOverlay = nullptr;
    bool m_menuReady = false;
    QLabel *m_background = nullptr;
    QButtonGroup *m_langGroup = nullptr;
    QString m_currentLocale = QStringLiteral("en");
};

#endif // DLGSTART_H
