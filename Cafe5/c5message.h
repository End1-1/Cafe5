#ifndef C5MESSAGE_H
#define C5MESSAGE_H

#include "c5dialog.h"

namespace Ui
{
class C5Message;
}

class C5Message : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Message();

    ~C5Message();

    static int error(const QString &errorStr, const QString &yes = QObject::tr("Close"), const QString &no = "");

    static int info(const QString &infoStr, const QString &yes = QObject::tr("OK"), const QString &no = "",
                    bool playSound = false);

    static int question(const QString &questionStr, const QString &yes = QObject::tr("Yes"),
                        const QString &no = QObject::tr("No"), const QString &a3 = "");

private slots:
    void timeout();

    void on_btnYes_clicked();

    void on_btnCancel_clicked();

    void on_btnA3_clicked();

    void on_btnCopy_clicked();

    void on_label_linkActivated(const QString &link);

private:
    Ui::C5Message* ui;

    bool fPlaySound;

    static int showMessage(const QString &text, int tp, const QString &yes, const QString &no, const QString &a3,
                           bool playsound = false);
};

#endif // C5MESSAGE_H
