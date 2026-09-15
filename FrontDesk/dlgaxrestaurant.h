#pragma once

#include <QByteArray>
#include <QDialog>
#include <QJsonArray>
#include <QPixmap>

class C5User;

namespace Ui
{
class DlgAxRestaurant;
}

class DlgAxRestaurant : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAxRestaurant(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgAxRestaurant() override;

    int recordId() const { return mId; }

private slots:
    void loadData();
    void trySave();
    void tryDelete();
    void browseImage();
    void clearImage();

private:
    void setPreviewFromPixmap(const QPixmap &pm);
    void setPreviewFromUrl(const QString &url);
    void clearPreview();
    void fillNationalityCombo(const QJsonArray &items);
    void selectNationality(int id);

    Ui::DlgAxRestaurant *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
    QByteArray mPendingImage;
    bool mImageChanged = false;
    bool mRemoveImage = false;
};
