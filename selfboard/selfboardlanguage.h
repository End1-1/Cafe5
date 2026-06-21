#ifndef SELFBOARDLANGUAGE_H
#define SELFBOARDLANGUAGE_H

#include <QObject>
#include <QPointer>

class QPushButton;
class QWidget;

class SelfboardLanguage : public QObject
{
    Q_OBJECT

public:
    static SelfboardLanguage &instance();

    QString currentLocale() const;
    QString displayName(const QString &locale) const;
    QString flagIconPath(const QString &locale) const;

    void loadSavedLocale();
    void applyLocale(const QString &code);
    void bindPickerButton(QPushButton *button);

signals:
    void localeChanged(const QString &code);

private:
    explicit SelfboardLanguage(QObject *parent = nullptr);

    void refreshPickerButtons();
    void showPickerMenu(QWidget *button);

    QString m_currentLocale = QStringLiteral("en");
    class QTranslator *m_translator = nullptr;
    bool m_localeLoaded = false;
    QList<QPointer<QWidget>> m_pickers;
};

#endif // SELFBOARDLANGUAGE_H
