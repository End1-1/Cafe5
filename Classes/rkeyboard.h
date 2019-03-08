#ifndef RKEYBOARD_H
#define RKEYBOARD_H

#include <QWidget>
#include <QList>
#include <QPushButton>

namespace Ui {
class RKeyboard;
}

class RKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit RKeyboard(QWidget *parent = 0);

    ~RKeyboard();

    void setText(const QString &text);

private slots:
    void btnTextClicked();

    void on_btnSpace_clicked();

    void on_btnBackspace_clicked();

    void on_btnCaps_clicked();

    void on_btnLShift_clicked();

    void on_btnCancel_clicked();

    void on_btnOk_clicked();

    void on_btnRShift_clicked();

    void on_btnEn_clicked();

    void on_btnAm_clicked();

    void on_btnRu_clicked();

    void on_btnClear_clicked();

    void on_leResult_textEdited(const QString &arg1);

signals:
    void textChanged(const QString &text);

    void accept();

    void reject();

private:
    Ui::RKeyboard *ui;

    QString fText;

    bool fShiftOn;

    bool fCapsOn;

    QString fCurrentLanguage;

    QList<QPushButton*> fLine1b;

    QList<QPushButton*> fLine2b;

    QList<QPushButton*> fLine3b;

    QList<QPushButton*> fLine4b;

    void connectButtons(QList<QPushButton*> &buttons);

    void setButtonsText(QList<QPushButton*> &buttons, const QString &text);

    void setupEnglish();

    void setupEnglishCaps();

    void setupRussia();

    void setupRussiaCaps();

    void setupArmenian();

    void setupArmenianCaps();

    void setupKbd();
};

#endif // RKEYBOARD_H
