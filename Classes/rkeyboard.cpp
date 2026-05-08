#include "rkeyboard.h"
#include "ui_rkeyboard.h"
#include "amkbd.h"
#include <QKeyEvent>

int RKeyboard::mKbdLang = 2;

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
    fLineNumpad << ui->btnNP_7
                << ui->btnNP_8
                << ui->btnNP_9
                << ui->btnNP_slash
                << ui->btnNP_4
                << ui->btnNP_5
                << ui->btnNP_6
                << ui->btnNP_star
                << ui->btnNP_1
                << ui->btnNP_2
                << ui->btnNP_3
                << ui->btnNP_minus
                << ui->btnNP_0
                << ui->btnNP_dot
                << ui->btnNP_plus;
    connectButtons(fLineNumpad);
    ui->btnNP_enter->setStyleSheet(styleSheet());
    connect(ui->btnNP_enter, &QToolButton::clicked, this, [this]() {
        emit accept();
    });
    connectButtons(fLine1b);
    connectButtons(fLine2b);
    connectButtons(fLine3b);
    connectButtons(fLine4b);
    fShiftOn = false;
    fCapsOn = false;
    setStyleSheet("");
    ui->btnRu->setVisible(false);

    switch(mKbdLang) {
    case 1:
        fCurrentLanguage = QStringLiteral("en");
        setupEnglish();
        break;

    case 2:
        fCurrentLanguage = QStringLiteral("am");
        setupArmenian();
        break;

    case 3:
        fCurrentLanguage = QStringLiteral("ru");
        setupRussia();
        break;

    default:
        fCurrentLanguage = QStringLiteral("am");
        setupArmenian();
        break;
    }

    ui->leResult->installEventFilter(this);
    ui->btnAm->click();
}

RKeyboard::~RKeyboard()
{
    delete ui;
}

void RKeyboard::setText(const QString &text)
{
    fText = text;
    ui->leResult->setText(fText);
}

QString RKeyboard::text() const
{
    return ui->leResult->text();
}

bool RKeyboard::eventFilter(QObject *o, QEvent *e)
{
    if(o == ui->leResult) {
        if(e->type() == QEvent::KeyPress) {
            QKeyEvent *ek = static_cast<QKeyEvent*>(e);

            if(ui->btnAm->isChecked()) {
                if(ek->key()) {
                    int index = keys.indexOf(ek->nativeVirtualKey());

                    if(index < 0) {
                        return QWidget::eventFilter(o, e);
                    }

                    QString txt = chars[index];

                    if(ek->modifiers() &Qt::ShiftModifier) {
                        txt = txt.toUpper();
                    }

                    auto *ekk = new QKeyEvent(QEvent::KeyPress, chars[index].at(0).unicode(), ek->modifiers(), txt);
                    qApp->postEvent(o, ekk);
                    return true;
                    //qDebug() << ek->key() << ek->text() << ek->nativeVirtualKey();
                }
            }
        }
    }

    return QWidget::eventFilter(o, e);
}

void RKeyboard::btnTextClicked()
{
    auto *b = static_cast<QToolButton*>(sender());
    fText.append(b->text());

    if(fShiftOn) {
        fShiftOn = false;
        setupKbd();
    }

    ui->leResult->setText(fText);
    ui->leResult->setCursorPosition(ui->leResult->text().length());
    emit textChanged(fText);
}

void RKeyboard::on_btnSpace_clicked()
{
    fText.append(" ");
    ui->leResult->setText(fText);
    emit textChanged(fText);
}

void RKeyboard::on_btnBackspace_clicked()
{
    fText.remove(fText.length() - 1, 1);
    ui->leResult->setText(fText);
    emit textChanged(fText);
}

void RKeyboard::connectButtons(QList<QToolButton*>& buttons)
{
    for(QList<QToolButton* >::const_iterator it = buttons.begin(); it != buttons.end(); it++) {
        connect(*it, SIGNAL(clicked()), this, SLOT(btnTextClicked()));
        (*it)->setStyleSheet(styleSheet());
    }
}

void RKeyboard::setButtonsText(QList<QToolButton*>& buttons, const QString &text)
{
    for(int i = 0; i < buttons.count(); i++) {
        buttons[i]->setText(text.at(i));
    }
}

void RKeyboard::setupEnglish()
{
    setButtonsText(fLine1b, QStringLiteral("`1234567890-=\\"));
    setButtonsText(fLine2b, QStringLiteral("qwertyuiop[]"));
    setButtonsText(fLine3b, QStringLiteral("asdfghjkl;'"));
    setButtonsText(fLine4b, QStringLiteral("zxcvbnm,./"));
}

void RKeyboard::setupEnglishCaps()
{
    setButtonsText(fLine1b, QStringLiteral("~!@#$%^&*()_+|"));
    setButtonsText(fLine2b, QStringLiteral("QWERTYUIOP{}"));
    setButtonsText(fLine3b, QStringLiteral("ASDFGHJKL:\""));
    setButtonsText(fLine4b, QStringLiteral("ZXCVBNM<>?"));
}

void RKeyboard::setupRussia()
{
    setButtonsText(fLine1b, QString::fromUtf8(u8"ё1234567890-=\\"));
    setButtonsText(fLine2b, QString::fromUtf8(u8"йцукенгшщзхъ"));
    setButtonsText(fLine3b, QString::fromUtf8(u8"фывапролджэ"));
    setButtonsText(fLine4b, QString::fromUtf8(u8"ячсмитьбю."));
}

void RKeyboard::setupRussiaCaps()
{
    setButtonsText(fLine1b, QString::fromUtf8(u8"Ё!\"№;%:?*()_+/"));
    setButtonsText(fLine2b, QString::fromUtf8(u8"ЙЦУКЕНГШЩЗХЪ"));
    setButtonsText(fLine3b, QString::fromUtf8(u8"ФЫВАПРОЛДЖЭ"));
    setButtonsText(fLine4b, QString::fromUtf8(u8"ЯЧСМИТЬБЮ,"));
}

void RKeyboard::setupArmenian()
{
    setButtonsText(fLine1b, QString::fromUtf8(u8"՝էթփձջւևրչճ-ժշ"));
    setButtonsText(fLine2b, QString::fromUtf8(u8"քոեռտըւիօպխծ"));
    setButtonsText(fLine3b, QString::fromUtf8(u8"ասդֆգհյկլ;՛"));
    setButtonsText(fLine4b, QString::fromUtf8(u8"զղցվբնմ,․/"));
}

void RKeyboard::setupArmenianCaps()
{
    setButtonsText(fLine1b, QString::fromUtf8(u8"՜ԷԹՓՁՋՒևՐՉՃ—ԺՇ"));
    setButtonsText(fLine2b, QString::fromUtf8(u8"ՔՈԵՌՏԸՒԻՕՊԽԾ"));
    setButtonsText(fLine3b, QString::fromUtf8(u8"ԱՍԴՖԳՀՅԿԼ։\""));
    setButtonsText(fLine4b, QString::fromUtf8(u8"ԶՂՑՎԲՆՄ<>՞"));
}

void RKeyboard::setupKbd()
{
    if((fShiftOn && !fCapsOn) || (!fShiftOn && fCapsOn)) {
        if(fCurrentLanguage == "en") {
            setupEnglishCaps();
        } else if(fCurrentLanguage == "ru") {
            setupRussiaCaps();
        } else {
            setupArmenianCaps();
        }
    } else {
        if(fCurrentLanguage == "en") {
            setupEnglish();
        } else if(fCurrentLanguage == "ru") {
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
    ui->btnRu->setChecked(false);
    ui->btnAm->setChecked(false);
    setupKbd();
    mKbdLang = 1;
}

void RKeyboard::on_btnAm_clicked()
{
    fCurrentLanguage = "am";
    ui->btnRu->setChecked(false);
    ui->btnEn->setChecked(false);
    setupKbd();
    ui->leResult->setFocus();
    ui->leResult->setSelection(ui->leResult->text().length(), 1);
    mKbdLang = 2;
}

void RKeyboard::on_btnRu_clicked()
{
    fCurrentLanguage = "ru";
    setupKbd();
    ui->leResult->setFocus();
    ui->leResult->setSelection(ui->leResult->text().length(), 1);
    mKbdLang = 3;
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
