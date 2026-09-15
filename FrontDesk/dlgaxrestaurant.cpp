#include "dlgaxrestaurant.h"
#include "ui_dlgaxrestaurant.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QBuffer>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QImage>
#include <QIntValidator>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace {

QByteArray encodeJpegPreview(const QImage &img)
{
    QImage scaled = img;
    if (scaled.width() > 1280 || scaled.height() > 1280) {
        scaled = scaled.scaled(1280, 1280, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    scaled.save(&buf, "JPG", 85);
    return ba;
}

} // namespace

DlgAxRestaurant::DlgAxRestaurant(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgAxRestaurant)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->leScore->setValidator(new QIntValidator(0, 999999, ui->leScore));
    ui->leLat->setValidator(new QDoubleValidator(-90.0, 90.0, 8, ui->leLat));
    ui->leLng->setValidator(new QDoubleValidator(-180.0, 180.0, 8, ui->leLng));
    ui->btnDelete->setVisible(mId > 0);
    setWindowTitle(mId > 0 ? tr("Edit restaurant") : tr("New restaurant"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgAxRestaurant::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgAxRestaurant::tryDelete);
    connect(ui->btnBrowseImage, &QPushButton::clicked, this, &DlgAxRestaurant::browseImage);
    connect(ui->btnClearImage, &QPushButton::clicked, this, &DlgAxRestaurant::clearImage);

    loadData();
}

DlgAxRestaurant::~DlgAxRestaurant()
{
    delete ui;
}

void DlgAxRestaurant::setPreviewFromPixmap(const QPixmap &pm)
{
    if (pm.isNull()) {
        clearPreview();
        return;
    }
    ui->lbPreview->setPixmap(pm.scaled(ui->lbPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->lbPreview->setText(QString());
}

void DlgAxRestaurant::clearPreview()
{
    ui->lbPreview->setPixmap(QPixmap());
    ui->lbPreview->setText(tr("No image"));
}

void DlgAxRestaurant::setPreviewFromUrl(const QString &url)
{
    if (url.trimmed().isEmpty()) {
        clearPreview();
        return;
    }
    auto *nam = new QNetworkAccessManager(this);
    QNetworkRequest req{QUrl(url)};
    QNetworkReply *reply = nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, nam]() {
        reply->deleteLater();
        nam->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            return;
        }
        QPixmap pm;
        if (pm.loadFromData(reply->readAll())) {
            setPreviewFromPixmap(pm);
        }
    });
}

void DlgAxRestaurant::browseImage()
{
    const QString fn = QFileDialog::getOpenFileName(
        this,
        tr("Image"),
        QString(),
        QStringLiteral("Images (*.jpg *.jpeg *.png *.bmp *.webp)"));
    if (fn.isEmpty()) {
        return;
    }

    QImage img;
    if (!img.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }

    mPendingImage = encodeJpegPreview(img);
    if (mPendingImage.isEmpty()) {
        C5Message::error(tr("Could not encode image"));
        return;
    }
    mImageChanged = true;
    mRemoveImage = false;
    ui->leImageUrl->clear();
    setPreviewFromPixmap(QPixmap::fromImage(img));
}

void DlgAxRestaurant::clearImage()
{
    mPendingImage.clear();
    mImageChanged = false;
    mRemoveImage = true;
    ui->leImageUrl->clear();
    clearPreview();
}

void DlgAxRestaurant::fillNationalityCombo(const QJsonArray &items)
{
    const int previousId = ui->cbNationality->currentData().toInt();
    ui->cbNationality->clear();
    ui->cbNationality->addItem(tr("Not selected"), 0);

    for (const QJsonValue &value : items) {
        const QJsonObject row = value.toObject();
        const int id = row.value(QStringLiteral("f_id")).toInt();
        const QString name = row.value(QStringLiteral("f_name")).toString().trimmed();
        ui->cbNationality->addItem(name.isEmpty() ? QString::number(id) : name, id);
    }

    if (previousId > 0 && ui->cbNationality->findData(previousId) < 0) {
        ui->cbNationality->addItem(tr("Unknown (%1)").arg(previousId), previousId);
    }
}

void DlgAxRestaurant::selectNationality(int id)
{
    int index = ui->cbNationality->findData(id);
    if (index < 0 && id > 0) {
        ui->cbNationality->addItem(tr("Unknown (%1)").arg(id), id);
        index = ui->cbNationality->findData(id);
    }
    ui->cbNationality->setCurrentIndex(index >= 0 ? index : 0);
}

void DlgAxRestaurant::loadData()
{
    mPendingImage.clear();
    mImageChanged = false;
    mRemoveImage = false;

    if (mId <= 0) {
        ui->leName->clear();
        ui->leScore->setText(QStringLiteral("0"));
        ui->leCategory->clear();
        ui->leImageUrl->clear();
        ui->leLat->clear();
        ui->leLng->clear();
        clearPreview();
    }

    NInterface::query1(QStringLiteral("/engine/v2/officen/ax-restaurants/get"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("f_id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           fillNationalityCombo(jdoc.value(QStringLiteral("nationalities")).toArray());
                           const QJsonObject row = jdoc.value(QStringLiteral("row")).toObject();
                           mId = row.value(QStringLiteral("f_id")).toInt(mId);
                           ui->leName->setText(row.value(QStringLiteral("f_name")).toString());
                           ui->leScore->setText(QString::number(row.value(QStringLiteral("f_score")).toInt()));
                           ui->leCategory->setText(row.value(QStringLiteral("f_category")).toString());
                           selectNationality(row.value(QStringLiteral("f_nationality_id")).toInt());
                           const QString imageUrl = row.value(QStringLiteral("f_image_url")).toString();
                           ui->leImageUrl->setText(imageUrl);
                           setPreviewFromUrl(imageUrl);
                           if (row.value(QStringLiteral("f_lat")).isNull()) {
                               ui->leLat->clear();
                           } else {
                               ui->leLat->setDouble(row.value(QStringLiteral("f_lat")).toDouble());
                           }
                           if (row.value(QStringLiteral("f_lng")).isNull()) {
                               ui->leLng->clear();
                           } else {
                               ui->leLng->setDouble(row.value(QStringLiteral("f_lng")).toDouble());
                           }
                       });
}

void DlgAxRestaurant::trySave()
{
    const QString name = ui->leName->text().trimmed();
    if (name.isEmpty()) {
        C5Message::error(tr("Name is required"));
        return;
    }

    const QString latText = ui->leLat->text().trimmed();
    const QString lngText = ui->leLng->text().trimmed();
    if (latText.isEmpty() != lngText.isEmpty()) {
        C5Message::error(tr("Latitude and longitude must be set together"));
        return;
    }

    QJsonObject payload{
        {QStringLiteral("f_id"), mId},
        {QStringLiteral("f_name"), name},
        {QStringLiteral("f_score"), ui->leScore->text().trimmed().toInt()},
        {QStringLiteral("f_category"), ui->leCategory->text().trimmed()},
        {QStringLiteral("f_nationality_id"), ui->cbNationality->currentData().toInt()},
        {QStringLiteral("f_image_url"), ui->leImageUrl->text().trimmed()},
    };
    if (mRemoveImage) {
        payload.insert(QStringLiteral("f_remove_image"), 1);
    } else if (mImageChanged && !mPendingImage.isEmpty()) {
        payload.insert(QStringLiteral("f_image_data"), QString::fromLatin1(mPendingImage.toBase64()));
    }
    if (!latText.isEmpty() && !lngText.isEmpty()) {
        payload.insert(QStringLiteral("f_lat"), ui->leLat->getDouble());
        payload.insert(QStringLiteral("f_lng"), ui->leLng->getDouble());
    } else {
        payload.insert(QStringLiteral("f_lat"), QJsonValue());
        payload.insert(QStringLiteral("f_lng"), QJsonValue());
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/ax-restaurants/save"),
                      mUser->mSessionKey,
                      this,
                      payload,
                      [this](const QJsonObject &jdoc) {
                          mId = jdoc.value(QStringLiteral("f_id")).toInt(mId);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

void DlgAxRestaurant::tryDelete()
{
    if (mId <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected restaurant?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/ax-restaurants/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("f_id"), mId}},
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
