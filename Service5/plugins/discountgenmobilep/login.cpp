#include "login.h"
#include "ops.h"
#include "c5translator.h"
#include "waiterconnection.h"
#include "QRCodeGenerator.h"
#include <QImage>
#include <QPixmap>
#include <QDir>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QCoreApplication>

int loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in, QString &username)
{
    quint8 version;
    QString passhash, firebaseToken;
    rm.readUByte(version, in);
    rm.readString(passhash, in);
    rm.readString(firebaseToken, in);
    db[":f_passhash"] = passhash;
    int userid = 0;

    if (db.exec("select f_id, concat_ws(' ', f_last, f_first) as f_fullname from s_user where f_passhash=:f_passhash") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return 0;
    }
    if (db.next()) {
        rm.putUByte(1);
        username = db.string("f_fullname");
        userid = db.integer("f_id");
        db[":f_firebase_token"] = firebaseToken;
        if (!db.update("s_user", "f_id", userid)) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
        return userid;
    } else {
        rm.putUByte(0);
        rm.putString(ntr("Username or password incorrect", ntr_am));
        return 0;
    }

    return 0;
}

int loginPin(RawMessage &rm, Database &db, const QByteArray &in, QString &username)
{
    quint8 version;
    QString pin, passhash, firebaseToken;
    int userid = 0;
    rm.readUByte(version, in);
    rm.readString(pin, in);
    rm.readString(firebaseToken, in);
    db[":f_pin"] = pin;

    db[":f_pin"] = QString::fromLatin1(QCryptographicHash::hash(pin.toUtf8(), QCryptographicHash::Md5).toHex());
    if (db.exec("select f_id, concat_ws(' ', f_last, f_first) as f_fullname from s_user where f_altpassword=:f_pin ") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return 0;
    }
    if (db.next()) {
        userid = db.integer("f_id");
        passhash = db.uuid();
        username =  db.string("f_fullname");
        db[":f_passhash"] = passhash;
        db[":f_firebase_token"] = firebaseToken;
        if (!db.update("s_user", "f_id", userid)) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
    } else {
        rm.putUByte(0);
        rm.putString(ntr("Username or password incorrect", ntr_am));
        return 0;
    }

    rm.putUByte(1);
    rm.putString(passhash);
    rm.putString(username);
    return userid;
}

void createQrDiscount(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w)
{
    quint8 protocol;
    QString phone;
    rm.readUByte(protocol, in);
    rm.readString(phone, in);
    qDebug() << "Create QrDiscount" << phone;

    db[":f_phone"] = phone;
    db.exec("select * from c_partners where f_phone=:f_phone");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(ntr("Please, contact system administrator", ntr_am));
        return;
    }
    QString qr = db.uuid();
    db[":f_mode"] = 7;
    db[":f_value"] = 0.1;
    db[":f_code"] = qr;
    db[":f_client"] = 1;
    db[":f_createdby"] = db.integer("f_id");
    db[":f_datestart"] = QDate::currentDate();
    db[":f_dateend"] = QDate::currentDate().addDays(365);
    db[":f_active"] = 1;
    if (db.insert("b_cards_discount") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }

    int levelIndex = 1;
    int versionIndex = 0;
    int maskIndex = -1;
    bool bExtent = true;
    QString encodeString(qr);
    CQR_Encode qrEncode;
    bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
    if (!successfulEncoding) {
        rm.putUByte(0);
        rm.putString("Cannot create QR code");
        return;
    }
    int qrImageSize = qrEncode.m_nSymbleSize;
    int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
    QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    encodeImage.fill(1);
    for ( int i = 0; i < qrImageSize; i++ ) {
        for ( int j = 0; j < qrImageSize; j++ ) {
            if ( qrEncode.m_byModuleData[i][j] ) {
                encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
            }
        }
    }
    QPixmap pix = QPixmap::fromImage(encodeImage);
    pix = pix.scaled(300, 300);
    pix.save(QString("%1\\%2.png").arg(QDir::tempPath(), qr));

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"img\"; filename=\"%1.png\"").arg(qr));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));

    QFile file(QString("%1\\%2.png").arg(QDir::tempPath(), qr));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file" << qr;
    }
    QByteArray fileContent(file.readAll());
    filePart.setBody(fileContent);
    multiPart->append(filePart);

    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl("https://cview.am/discountapp/hello.php"));
    req.setRawHeader("Authorization", QByteArray("85ab6c5f-97e1-11ed-80bd-1078d2d2b808").data());

    QNetworkReply *reply(mgr.post(req, multiPart));
    bool wait = reply->isFinished();
    while (!wait) {
        QCoreApplication::processEvents();
        wait = reply->isFinished();
        QThread::msleep(1000);
    }
    qDebug() << reply->readAll();

    rm.putUByte(1);
    rm.putString(qr);
}

void checkBonuses(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w)
{
    QString phone;
    rm.readString(phone, in);
    qDebug() << "Check bonuses" << phone;

    db[":f_phone"] = phone;
    db.exec("select * from c_partners where f_phone=:f_phone");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(ntr("Please, contact system administrator", ntr_am));
        return;
    }
    db[":f_createdby"] = db.integer("f_id");
    db.exec("select sum(oh.f_amounttotal/1000) "
            "from b_history bh "
            "left join b_cards_discount bc on bc.f_id=bh.f_card "
            "left join o_header oh on oh.f_id=bh.f_id "
            "where bc.f_createdby=:f_createdby ");
    db.next();
    int bonus = db.integer(0);
    rm.putUByte(bonus);
    return;

//    rm.putUByte(0);
//    rm.putString(ntr("Phone is not registered", ntr_am));
}
