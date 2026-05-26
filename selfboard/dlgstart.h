#ifndef DLGSTART_H
#define DLGSTART_H

#include <QDialog>
#include <QLabel>
#include <QTranslator>

#include "dlgmenu.h"

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
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onLanguageClicked(int id);
    void onTakeAwayClicked();
    void onDineInClicked();

    void openMenu(DlgMenu::ServiceMode mode);

private:
    void setupAppearance();
    void setupBackground();
    void setupIcons();
    void setupLanguageGroup();
    void applyLanguage(const QString &localeCode);
    void updateLanguageButtonIcons();

    Ui::DlgStart *ui;
    QLabel *m_background = nullptr;
    QButtonGroup *m_langGroup = nullptr;
    QTranslator m_translator;
    QString m_currentLocale = QStringLiteral("en");
};

#endif // DLGSTART_H
