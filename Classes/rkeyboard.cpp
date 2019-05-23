#include "rkeyboard.h"
#include "ui_rkeyboard.h"

RKeyboard::RKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RKeyboard)
{
    ui->setupUi(this);
    fLine1b << ui->btn1_1
            << ui->btn1_2
            << ui->btn1_3
            << ui->btn1_4
            << ui->btn1_5
            << ui->btn1_6
            << ui->btn1_7
            << ui->btn1_8
            << ui->btn1_9
            << ui->btn1_10
            << ui->btn1_11
            << ui->btn1_12
            << ui->btn1_13
            << ui->btn1_14;
    fLine2b << ui->btn2_1
            << ui->btn2_2
            << ui->btn2_3
            << ui->btn2_4
            << ui->btn2_5
            << ui->btn2_6
            << ui->btn2_7
            << ui->btn2_8
            << ui->btn2_9
            << ui->btn2_10
            << ui->btn2_11
            << ui->btn2_12;
    fLine3b << ui->btn3_1
            << ui->btn3_2
            << ui->btn3_3
            << ui->btn3_4
            << ui->btn3_5
            << ui->btn3_6
            << ui->btn3_7
            << ui->btn3_8
            << ui->btn3_9
            << ui->btn3_10
            << ui->btn3_11;
    fLine4b << ui->btn4_1
            << ui->btn4_2
            << ui->btn4_3
            << ui->btn4_4
            << ui->btn4_5
            << ui->btn4_6
            << ui->btn4_7
            << ui->btn4_8
            << ui->btn4_9
            << ui->btn4_10;
    connectButtons(fLine1b);
    connectButtons(fLine2b);
    connectButtons(fLine3b);
    connectButtons(fLine4b);
    fShiftOn = false;
    fCapsOn = false;
    setupEnglish();
    setStyleSheet("");
}

RKeyboard::~RKeyboard()
{
    delete ui;
}

void RKeyboard::setText(const QString &text)
{
    fText = text;
}

QString RKeyboard::text() const
{
    return ui->leResult->text();
}

void RKeyboard::btnTextClicked()
{
    QPushButton *b = static_cast<QPushButton*>(sender());
    fText.append(b->text());
    if (fShiftOn) {
        fShiftOn = false;
        setupEnglish();
    }
    ui->leResult->setText(fText);
    ui->leResult->setCursorPosition(ui->leResult->text().length());
    emit textChanged(fText);
}

void RKeyboard::on_btnSpace_clicked()
{
    fText.append(" ");
    emit textChanged(fText);
}

void RKeyboard::on_btnBackspace_clicked()
{
    fText.remove(fText.length() - 1, 1);
    emit textChanged(fText);
}

void RKeyboard::connectButtons(QList<QPushButton *> &buttons)
{
    for (QList<QPushButton*>::const_iterator it = buttons.begin(); it != buttons.end(); it++) {
        connect(*it, SIGNAL(clicked()), this, SLOT(btnTextClicked()));
        (*it)->setStyleSheet(styleSheet());
    }
}

void RKeyboard::setButtonsText(QList<QPushButton *> &buttons, const QString &text)
{
    for (int i = 0; i < buttons.count(); i++) {
        buttons[i]->setText(text.at(i));
    }
}

void RKeyboard::setupEnglish()
{
    setButtonsText(fLine1b, "`1234567890-=\\");
    setButtonsText(fLine2b, "qwertyuiop[]");
    setButtonsText(fLine3b, "asdfghjkl;'");
    setButtonsText(fLine4b, "zxcvbnm,./");
}

void RKeyboard::setupEnglishCaps()
{
    setButtonsText(fLine1b, "~!@#$%^&*()_+|");
    setButtonsText(fLine2b, "QWERTYUIOP{}");
    setButtonsText(fLine3b, "ASDFGHJKL:\"");
    setButtonsText(fLine4b, "ZXCVBNM<>?");
}

void RKeyboard::setupRussia()
{
    setButtonsText(fLine1b, "ё1234567890-=\\");
    setButtonsText(fLine2b, "йцукенгшщзхъ");
    setButtonsText(fLine3b, "фывапролджэ");
    setButtonsText(fLine4b, "ячсмитьбю.");
}

void RKeyboard::setupRussiaCaps()
{
    setButtonsText(fLine1b, "Ё!\"№;%:?*()_+/");
    setButtonsText(fLine2b, "ЙЦУКЕНГШЩЗХЪ");
    setButtonsText(fLine3b, "ФЫВАПРОЛДЖЭ");
    setButtonsText(fLine4b, "ЯЧСМИТЬБЮ,");
}

void RKeyboard::setupArmenian()
{
    setButtonsText(fLine1b, "՝էթփձջւևրչճ-ժշ");
    setButtonsText(fLine2b, "քոեռտըւիօպխծ");
    setButtonsText(fLine3b, "ասդֆգհյկլ;՛");
    setButtonsText(fLine4b, "զղցվբնմ,․/");
}

void RKeyboard::setupArmenianCaps()
{
    setButtonsText(fLine1b, "՜ԷԹՓՁՋՒևՐՉՃ—ԺՇ");
    setButtonsText(fLine2b, "ՔՈԵՌՏԸՒԻՕՊԽԾ");
    setButtonsText(fLine3b, "ԱՍԴՖԳՀՅԿԼ։\"");
    setButtonsText(fLine4b, "ԶՂՑՎԲՆՄ<>՞");
}

void RKeyboard::setupKbd()
{
    if ((fShiftOn && !fCapsOn) || (!fShiftOn && fCapsOn)) {
        if (fCurrentLanguage == "en") {
            setupEnglishCaps();
        } else if (fCurrentLanguage == "ru" ) {
            setupRussiaCaps();
        } else {
            setupArmenianCaps();
        }
    } else {
        if (fCurrentLanguage == "en") {
            setupEnglish();
        } else if (fCurrentLanguage == "ru" ) {
            setupRussia();
        } else {
            setupArmenian();
        }
    }
}


void RKeyboard::on_btnCaps_clicked()
{
    fCapsOn = !fCapsOn;
    setupKbd();
}

void RKeyboard::on_btnLShift_clicked()
{
    fShiftOn = !fShiftOn;
    setupKbd();
}

void RKeyboard::on_btnCancel_clicked()
{
    emit reject();
}

void RKeyboard::on_btnOk_clicked()
{
    emit accept();
}

void RKeyboard::on_btnRShift_clicked()
{
    on_btnLShift_clicked();
}

void RKeyboard::on_btnEn_clicked()
{
    fCurrentLanguage = "en";
    setupKbd();
}

void RKeyboard::on_btnAm_clicked()
{
    fCurrentLanguage = "am";
    setupKbd();
}

void RKeyboard::on_btnRu_clicked()
{
    fCurrentLanguage = "ru";
    setupKbd();
}

void RKeyboard::on_btnClear_clicked()
{
    ui->leResult->clear();
    fText.clear();
    emit textChanged(fText);
}

void RKeyboard::on_leResult_textEdited(const QString &arg1)
{
    emit textChanged(arg1);
}
