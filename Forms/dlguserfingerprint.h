#ifndef DLGUSERFINGERPRINT_H
#define DLGUSERFINGERPRINT_H

#include "c5dialog.h"

class ZkfingerprintReader;

namespace Ui {
class DlgUserFingerprint;
}

class DlgUserFingerprint : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgUserFingerprint(int userId, C5User *user);
    ~DlgUserFingerprint() override;

    static void edit(int userId, C5User *user);

private slots:
    void on_btnReplace_clicked();
    void on_btnRemove_clicked();
    void on_btnClose_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void loadUser();
    void updateUi();
    void stopEnrollment();
    void saveTemplate(const QByteArray &templateData, int verifyScore);

    Ui::DlgUserFingerprint *ui;
    ZkfingerprintReader *mReader = nullptr;
    int mUserId = 0;
    bool mHasFingerprint = false;
    QString mUpdatedAt;
    bool mEnrolling = false;
};

#endif // DLGUSERFINGERPRINT_H
