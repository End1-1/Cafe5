#include "printtaxno.h"
#include <QByteArray>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QJsonObject>
#include <QMutex>
#include <QRegularExpression>
#include <QTcpSocket>
#include <winsock2.h>

#define DEBUG_RESULT 0;

//#define ___LOG_FISCAL___
#ifdef ___LOG_FISCAL___
#include "logwriter.h"
#endif

#define OPENSSL_SUPPRESS_DEPRECATED
#include <openssl/des.h>

static quint8 firstdata[] = {213, 128, 212, 180, 213, 132, 0, 5, 2, 0, 0, 0};
QMap<int, QString> PrintTaxNO::fErrors;
int PrintTaxNO::mDebugRseq = 0;
static QMutex fTaxMutex;

#define float_str_pt(value, f) \
    QString::number(value, 'f', f) \
        .remove(QRegularExpression("\\.0+$")) \
        .remove(QRegularExpression("\\.$"))

int PrintTaxNO::connectToHost(QString &err)
{
    fTcpSocket.connectToHost(QHostAddress(fIP), fPort);

    if(!fTcpSocket.waitForConnected(2000)) {
        err = QString("%1: %2 (%3, %4:%5)")
              .arg(tr("Connection error"), QString::number(fTcpSocket.error() * -1), fTcpSocket.errorString(), fIP,
                   QString::number(fPort));
        return fTcpSocket.error() * -1;
    }

    return pt_err_ok;
}

bool PrintTaxNO::jsonLogin(QByteArray &out)
{
    if(fTaxCashier.isEmpty()) {
        //C5Database().logEvent(QString("Tax jsonLogin: fTaxCashier is empty"));
        return false;
    }

    fPassSHA256 = QCryptographicHash::hash(fPassword.toLatin1(), QCryptographicHash::Sha256).mid(0, 24);
    QByteArray authStr = QString("{\"password\":\"%1\",\"cashier\":%2,\"pin\":%3}").arg(fPassword, fTaxCashier,
                         fTaxPin).toUtf8();
    logMessage(authStr);
    cryptData(fPassSHA256, authStr, out);
    return true;
}

bool PrintTaxNO::jsonLogout(QByteArray &out)
{
    fPassSHA256 = QCryptographicHash::hash(fPassword.toLatin1(), QCryptographicHash::Sha256).mid(0, 24);
    QByteArray authStr = QString("{\"seq\":2}").toUtf8();
    cryptData(fPassSHA256, authStr, out);
    return true;
}

void PrintTaxNO::makeRequestHeader(quint8 *dst, quint8 request, quint16 dataLen)
{
    memcpy(dst, &firstdata[0], 12);
    dst[8] = request;
    quint8 chLen[2];
    memcpy(&chLen[0], &dataLen, sizeof(qint16));
    dst[10] = chLen[1];
    dst[11] = chLen[0];
}

int PrintTaxNO::getResponse(QByteArray &out, QString &err)
{
    out.clear();
    quint8 fd[11];
    qint64 bytesTotal;
    QElapsedTimer et;
    et.start();

    if(fTcpSocket.waitForReadyRead(130000)) {
        bytesTotal = fTcpSocket.bytesAvailable();
        fTcpSocket.read(reinterpret_cast<char*>(&fd[0]), 11);
        bytesTotal -= 11;
        quint16 dataLen;
        memcpy(&dataLen, &fd[7], 2);
        dataLen = ntohs(dataLen);
        out.clear();

        while(bytesTotal > 0) {
            char buff[1024];
            qint64 bytesRead = fTcpSocket.read(&buff[0], 1024);

            if(bytesRead == 0) {
                err = QString("%1: %2").arg(tr("Connection error"), tr("Socket read error"));
                return pt_err_socket_read;
            }

            bytesTotal -= bytesRead;
            out.append(&buff[0], bytesRead);
        }
    } else {
        fTcpSocket.close();
        err = QString("%1: %2 (%3 ms)").arg(tr("Connection error"), tr("Data read timeout"), QString::number(et.elapsed()));
        return pt_err_nodata_login;
    }

    quint16 result;
    memcpy(&result, &fd[5], sizeof(quint16));
    result = ntohs(result);
    return result;
}

void PrintTaxNO::cryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData)
{
    outData.clear();
    quint8 jsonFillCount = 8 - (inData.length() % 8);

    for(int i = 0; i < jsonFillCount; i++) {
        inData.append((char)jsonFillCount);
    }

    size_t key_len = 24;
    unsigned char key[24];
    unsigned char block_key[9];
    DES_key_schedule ks, ks2, ks3;
    memcpy(key, k.data(), key_len);
    memset(key + key_len, 0x00, 24 - key_len);
    memset(block_key, 0, sizeof(block_key));
    memcpy(block_key, key + 0, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks);
    memcpy(block_key, key + 8, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks2);
    memcpy(block_key, key + 16, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks3);
    qint16 srcLen = inData.length();
    char* out = new char[srcLen];
    memset(out, 0, srcLen);

    for(int i = 0; i < srcLen; i += 8) {
        DES_ecb3_encrypt((const_DES_cblock*) &inData.data()[i], (DES_cblock*) &out[i], &ks, &ks2, &ks3, DES_ENCRYPT);
    }

    outData.append(out, srcLen);
    delete [] out;
}

void PrintTaxNO::decryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData)
{
    outData.clear();
    size_t key_len = 24;
    unsigned char key[24];
    unsigned char block_key[9];
    DES_key_schedule ks, ks2, ks3;
    memcpy(key, k.data(), key_len);
    memset(key + key_len, 0x00, 24 - key_len);
    memset(block_key, 0, sizeof(block_key));
    memcpy(block_key, key + 0, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks);
    memcpy(block_key, key + 8, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks2);
    memcpy(block_key, key + 16, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks3);
    char* out = new char[inData.length()];
    memset(out, 0, inData.length());

    for(int i = 0; i < inData.length(); i += 8) {
        DES_ecb3_encrypt((const_DES_cblock*) &inData.data()[i], (DES_cblock*) &out[i], &ks, &ks2, &ks3, DES_DECRYPT);
    }

    outData.append(out, inData.length());
    delete [] out;
}

void PrintTaxNO::logMessage(const QString &msg)
{
    Q_UNUSED(msg);
#ifdef ___LOG_FISCAL___
    LogWriter::write(LogWriterLevel::verbose, "", msg);
#endif
}

PrintTaxNO::PrintTaxNO()
{
    logMessage("Constructor");

    if(fErrors.count() == 0) {
        initErrors();
    }

    fJsonHeader["paidAmount"] = 0.0;
    fJsonHeader["paidAmountCard"] = 0.0;
    fJsonHeader["prePaymentAmount"] = 0.0;
    fJsonHeader["userExtPOS"] = "true";
    fTaxCashier = "3";
    fTaxPin = "3";
}

PrintTaxNO::PrintTaxNO(const QString &ip,
                       int port,
                       const QString &password,
                       const QString &extPos,
                       const QString &opcode,
                       const QString &oppin,
                       QObject *parent)
    : QObject(parent)
{
    logMessage("Constructor");
    setParams(ip, port, password, opcode, oppin, extPos);

    if(fErrors.count() == 0) {
        initErrors();
    }

    fTaxCashier = opcode;
    fTaxPin = oppin;
    fJsonHeader["paidAmount"] = 0.0;
    fJsonHeader["paidAmountCard"] = 0.0;
    fJsonHeader["prePaymentAmount"] = 0.0;
    fJsonHeader["userExtPOS"] = extPos;
    fCashAmountForReturn = 0;
    fCardAmountForReturn = 0;
    fPrepaymentAmountForReturn = 0;
}

void PrintTaxNO::setParams(const QString &ip, int port, const QString &password, const QString &op, const QString &pin, const QString &extPos)
{
    fIP = ip;
    fPort = port;
    fPassword = password;
    fTaxCashier = op;
    fTaxPin = pin;
    fJsonHeader["userExtPOS"] = extPos;
}

void PrintTaxNO::initErrors()
{
    fErrors[pt_err_ok] = "Բարեհաջող ավարտ։";
    fErrors[pt_err_crn_empty] = "Crn field is empty";
    fErrors[500] = "ՀԴՄ ներքին սխալ:  Ընդհանուր տիպի չդասակարգված սխալ:";
    fErrors[400] =
        "Հարցման սխալ:  Վերադարձվում է երբ հարցումը չի վերծանվում:";
    fErrors[402] = "Սխալ արձանագրության տարբերակ:";
    fErrors[403] =
        "Չարտոնագրված միացում: Վերադարձվում է, երբ ՀԴՄ-ում կարագավորված IP հասցեն չի համընկնում կապ հաստատաց սարքի IP հասցեի հետ:";
    fErrors[404] =
        "Սխալ գործողության կոդ:  Վերադարձվում է, երբ Գլխագրում նշված գործողության կոդը սխալ է:";
    fErrors[101] = "Գաղտնաբառով կոդավորման սխալ:";
    fErrors[102] = "Սեսիայի բանալիով կոդավորման սխալ:";
    fErrors[103] = "Գլխագրի ֆորմատի սխալ:";
    fErrors[104] = "Հարցման հերթական համարի սխալ:";
    fErrors[105] = "JSON ֆորմատավորման սխալ:";
    fErrors[141] = "Վերջին կտրոնի գրառումը բացակայում է:";
    fErrors[142] = "Վերջին կտրոնը պատկանում է այլ օգտատիրի:";
    fErrors[143] = "Տպիչի ընդհանուր սխալ:";
    fErrors[144] = "Տպիչի նույնականացման սխալ:";
    fErrors[145] = "Տպիչում վերջացել է թուղթը:";
    fErrors[111] = "Օպերատորի գաղտնաբառի սխալ:";
    fErrors[112] =
        "Այդպիսի օպերատոր գոյություն չունի: Հնարավոր է երեք դեպք՝ 1.  Տվյալ օգտվողի դերը օպերատոր չէ  2.  Տվյալ օգտվողը ակտիվ չէ 3.  Այդպիսի օգտվող գրանցված չէ:";
    fErrors[113] = "Օպերատորը ակտիվ չէ:";
    fErrors[121] = "Սխալ օգտվող:";
    fErrors[151] =
        "Այդպիսի բաժին գոյություն չունի:  Այս սխալը կվերադարձվի նաև այն դեպքում, երբ տվյալ բաժինը կցված չէ օպերատորին:";
    fErrors[152] = "Վճարված գումարը ընդհանուր գումարից պակաս է:";
    fErrors[153] = "Կտրոնի գումարը գերազանզում է սահմանված շեմը:";
    fErrors[154] = "Կտրոնի գումարը պետք է լինի դրական թիվ:";
    fErrors[155] = "Անհրաժեշտ է համաժամանակեցնել ՀԴՄ-ն:";
    fErrors[157] = "Սխալ վերադարձի կտրոնի համար:";
    fErrors[158] = "Կտրոնը արդեն վերադարձված է:";
    fErrors[159] = "Ապրանքի գինը և քանակը չի կարող լինել ոչ դրական:";
    fErrors[160] = "Զեղչի տոկոսը պետք է լինի ոչ բացասական թիվ՝ 100-ից փոքր:";
    fErrors[161] = "Ապրանքի կոդը սխալ է:";
    fErrors[162] = "Ապրանքի անվանումը սխալ է:";
    fErrors[163] = "Ապրանքի չափման Միավորի դաշտը չի կարող լինել դատարկ:";
    fErrors[164] = "Անկանխիկ վճարման խափանում:";
    fErrors[165] = "Ապրանքի գինը չի կարող   լինել 0:";
    fErrors[166] = "Վերջնական գնի հաշվարկի սխալ:";
    fErrors[167] =
        "Անկանխիկ գումարը ավելի մեծ է քան կտրոնի ընդհանուր գումարը:";
    fErrors[168] =
        "Անկանխիկ գումարը ծածկում է ընդհանուր գումարը (Կանխիկ գումարը ավելորդ է):";
    fErrors[169] =
        "Ֆիսկալ հաշվետվության ֆիլտրների սխալ ընտրություն (մեկից ավել ֆիլտրերի դաշտ է ուղարկվել):";
    fErrors[170] =
        "Ֆիսկալ հաշվետվության ժամանակ սխալ ամսաթվային միջակայք է ուղարկվել: Միջակայքը չպետք է գերազանցի 2 ամիսը:";
    fErrors[171] = "Ապրանքի գնի անթույլատրելի արժեք:";
    fErrors[172] = "Կտրոնը ապրանքներով կտրոն չէ:";
    fErrors[173] = "Սխալ զեղչի տեսակ:";
    fErrors[174] = "Վերադարձվող կտրոնը գոյություն չունի:";
    fErrors[175] = "Վերադարձվող կտրոնի սխալ գրանցման համար:";
    fErrors[176] = "Վերջին կտրոնը գոյություն չունի:";
    fErrors[177] =
        "Նշված տիպի կտրոնների համար վերադարձ հնարավոր չէ կատարել:";
    fErrors[178] = "Հարցված գումարը հնարավոր չէ վերադարձնել:";
    fErrors[179] =
        "Մասնակի վճարում: Կտրոնը պետք է վերադարձվի ամբողջությամբ:";
    fErrors[180] = "Ամբողջական վերադարձի ավել գումար:";
    fErrors[181] = "Վերադարձվող ապրանքի սխալ քանակ:";
    fErrors[182] =
        "Վերադարձվող կտրոնը իրենից ներկայացնում է վերադարձ տիպի կտրոն:";
    fErrors[183] = "Սխալ ԱՏԳԱԱ/ԱԴԳՏ կոդ:";
    fErrors[184] = "Կանխավճարի վերադարձի անթույլատրելի հարցում:";
    fErrors[185] =
        "Հնարավոր չէ կատարել տվյալ կտրոնի վերադարձը: Անհրաժեշտ է ՀԴՄ ծրագրի համաժամանակեցում:";
    fErrors[186] = "Կանխավճարի դեպքում սխալ գումար:";
    fErrors[187] = "Կանխավճարի դեպքում սխալ ցուցակ:";
    fErrors[188] = "Սխալ գումարներ:";
    fErrors[189] = "Սխալ կլորացում:";
    fErrors[190] = "Վճարումը հասանելի չէ:";
    fErrors[191] = "Կանխիկի մուտքի/ելքի ժամանակ գումարը պետք է լինի մեծ 0-ից:";
    fErrors[192] = "ԱՏԳԱԱ/ԱԴԳՏ կոդը բացակայում է:";
    fErrors[193] = "Գնորդի ՀՎՀՀ-ի ֆորմատը սխալ է";
    fErrors[194] = "Կանխավճարի դեպքում չպետք է ուղարկվեն emark կոդեր";
    fErrors[195] = "emark կոդի ֆորմատի սխալ";
}

int PrintTaxNO::printJSON(QByteArray &jsonData, QString &err, quint8 opcode)
{
    logMessage(jsonData);
    int result = pt_err_ok;
    QByteArray out;
    logMessage("Print JSON, connect to host");
    result = connectToHost(err);

    if(result) {
        logMessage("Print JSON, connection failed");
        return result;
    }

    jsonLogin(out);
    quint8 fd[12];
    makeRequestHeader(&fd[0], opcode_login, out.length());
    logMessage("Print JSON, connected");
    fTcpSocket.write(reinterpret_cast<const char*>(&fd[0]), 12);
    fTcpSocket.write(out, out.length());

    if(!fTcpSocket.waitForBytesWritten(30000)) {
        err = "Wait for write";
        return pt_err_wait_for_write;
    }

    fTcpSocket.flush();
    logMessage("Print JSON, header request");
    result = getResponse(out, err);

    if(result != 200) {
        if(err.isEmpty()) {
            err = "Error";
        }

        if(result > 0) {
            if(fErrors.empty()) {
                fErrors[result] = "Empty errors list";
            }

            err = QString("%1: %2 (%3)").arg(tr("Tax error code"), QString::number(result), fErrors[result]);
        } else {
            err = QString("%1: %2 (%3)").arg(tr("Connection error"), QString::number(result), err);
        }

        return result;
    }

    logMessage("Print JSON, response of header");
    decryptData(fPassSHA256, out, fSessionPass);
    char c = fSessionPass.at(fSessionPass.length() - 1);

    if(c < 8) {
        for(int i = 0; i < c; i++) {
            fSessionPass.remove(fSessionPass.length() - 1, 1);
        }
    }

    QJsonDocument jDoc = QJsonDocument::fromJson(fSessionPass);
    QJsonObject jObj(jDoc.object());
    fSessionPass = QByteArray::fromBase64(jObj.value("key").toString().toLatin1());
    cryptData(fSessionPass, jsonData, out);
    makeRequestHeader(&fd[0], opcode, out.length());
    logMessage("Print JSON, crypt main data");
    fTcpSocket.write(reinterpret_cast<const char*>(&fd[0]), 12);
    fTcpSocket.write(out, out.length());
    fTcpSocket.flush();
    logMessage("Print JSON, write main data");

    if((result = getResponse(jsonData, err)) != 200) {
        if(err.isEmpty()) {
            err = "Error";
        }

        if(result > 0) {
            if(fErrors.empty()) {
                fErrors[result] = "Empty errors list";
            }

            err = QString("%1: %2 (%3)").arg(tr("Tax error code"), QString::number(result), fErrors[result]);
        } else {
            err = QString("%1: %2 (%3)").arg(tr("Connection error"), QString::number(result), err);
        }

        return result;
    }

    logMessage("Print JSON, read response");
    decryptData(fSessionPass, jsonData, out);
    logMessage("Response " + out);

    if(out.length() > 0) {
        c = out.at(out.length() - 1);
    } else {
        c = 10;
    }

    if(c < 8) {
        for(int i = 0; i < c; i++) {
            out.remove(out.length() - 1, 1);
        }
    }

    jsonData = out;
    logMessage("Logout");
    makeRequestHeader(&fd[0], opcode_logout, out.length());
    fTcpSocket.write(reinterpret_cast<const char*>(&fd[0]), 12);
    fTcpSocket.write(out, out.length());
    fTcpSocket.flush();
    fTcpSocket.close();
    logMessage("Print JSON, end");
    return pt_err_ok;
}

void PrintTaxNO::addGoods(int dep, const QString &adgt, const QString &code, const QString &name, double price, double qty, double discount)
{
    QMap<QString, QJsonValue> data;
    data["dep"] = dep;
    data["adgCode"] = adgt;
    data["productCode"] = code;
    data["productName"] = QString(name).replace("\"", "\\\"");
    data["price"] = price;
    data["qty"] = qty;
    double total = price * qty;
    data["totalPrice"] = total;
    data["unit"] = QString::fromUtf8("հատ");

    if(discount > 0.01) {
        data["discount"] = discount;
        data["discountType"] = 1;
    }

    fJsonGoods.append(data);
    double totalCash = 0;

    for(int i = 0; i < fJsonGoods.count(); i++) {
        QMap<QString, QJsonValue>& g = fJsonGoods[i];
        double t = g["totalPrice"].toDouble();

        if(g.contains("discount")) {
            t -= (t * (g["discount"].toDouble() / 100));
        }

        totalCash += t;
    }

    fJsonHeader["paidAmount"] = totalCash;
}

void PrintTaxNO::addReturnItem(int row, double qty)
{
    fReturnItemList.append(QJsonObject{{"rpid", row},
        {"quantity", qty}});
}

int PrintTaxNO::makeJsonAndPrint(double card, double prepaid, QString &outInJson, QString &outOutJson, QString &err)
{
    QString emarks;

    for(QString e : fEmarks) {
        if(!emarks.isEmpty()) {
            emarks += ",";
        }

        //e.replace("'", "\\\\'");
        e.replace("\\", "\\\\");
        e.replace("\"", "\\\"");
        emarks += QString("\"%1\"").arg(e);
    }

    fJsonHeader["paidAmountCard"] = card;
    fJsonHeader["prePaymentAmount"] = prepaid;
    fJsonHeader["paidAmount"] = fJsonHeader["paidAmount"].toDouble() - card - prepaid;
    QString json = QString(
                       "{\"seq\":1,\"paidAmount\":%1, \"paidAmountCard\":%2, \"partialAmount\":0, "
                       "\"prePaymentAmount\":%3, \"useExtPOS\":%4, \"mode\":2, %5 %6 \"items\":[")
                       .arg(float_str_pt(fJsonHeader["paidAmount"].toDouble(), 2))
                       .arg(float_str_pt(fJsonHeader["paidAmountCard"].toDouble(), 2))
                       .arg(float_str_pt(fJsonHeader["prePaymentAmount"].toDouble(), 2))
                       .arg(fJsonHeader["userExtPOS"].toString())
                       .arg(fPartnerTin.length() > 0
                                ? QString("\"partnerTin\":\"%1\",").arg(fPartnerTin)
                                : "\"partnerTin\":null,")
                       .arg(fEmarks.isEmpty() ? "" : QString("\"eMarks\":[%1],").arg(emarks));
    bool first = true;

    Q_ASSERT(!fJsonGoods.isEmpty());
    for(int i = 0; i < fJsonGoods.count(); i++) {
        if(first) {
            first = false;
        } else {
            json += ",";
        }

        QMap<QString, QJsonValue>& g = fJsonGoods[i];
        QString discount;

        if(g.contains("discount")) {
            discount = QString(",\"discount\":%1, \"discountType\":%2")
                       .arg(g["discount"].toDouble())
                       .arg(g["discountType"].toInt());
        }

        json +=   QString("{\"adgCode\":\"%1\", \"dep\":%2, \"price\":%3, \"productCode\":\"%4\", "
                          "\"productName\":\"%5\", \"qty\":%6, \"unit\":\"%7\" %8}")
                  .arg(g["adgCode"].toString())
                  .arg(g["dep"].toInt())
                  .arg(g["price"].toDouble())
                  .arg(g["productCode"].toInt())
                  .arg(g["productName"].toString())
                  .arg(g["qty"].toDouble())
                  .arg(g["unit"].toString())
                  .arg(discount);
    }

    json += "]}";
    QByteArray jdata = json.toUtf8();
    int result = printJSON(jdata, err, opcode_print_receipt);
    //json.replace("\\\"", "\\\\\"");
    outInJson = json;
    outOutJson = jdata;

    if(result != 0) {
#ifdef QT_DEBUG
        outOutJson =
            QString("{\"rseq\":%1,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1676794194840,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}")
            .arg(mDebugRseq);
        outOutJson =
            QString("{\"address\":\"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Ամիրյան 4/3 \",\"change\":0.0,\"crn\":\"53235782\",\"fiscal\":\"54704153\",\"lottery\":\"\",\"prize\":0,\"rseq\":%1,\"sn\":\"00022154380\",\"taxpayer\":\"«ՊԼԱԶԱ ՍԻՍՏԵՄՍ»\",\"time\":1709630105632,\"tin\":\"02596277\",\"total\":93600.0}")
            .arg(mDebugRseq);
        result = DEBUG_RESULT;
#endif
    }

    if(result == 0) {
        while(outOutJson.length() > 0 && outOutJson.at(outOutJson.length() - 1) != "}") {
            outOutJson.remove(outOutJson.length() - 1, 1);
        }
    }

    return result;
}

int PrintTaxNO::makeJsonAndPrint(double cash, double card, double prepaid, QString &outInJson, QString &outOutJson, QString &err)
{
    emit started();
    QString emarks;

    for(QString e : fEmarks) {
        if(!emarks.isEmpty()) {
            emarks += ",";
        }

        //e.replace("'", "\\\\'");
        e.replace("\\", "\\\\");
        e.replace("\"", "\\\"");
        emarks += QString("\"%1\"").arg(e);
    }

    fJsonHeader["paidAmountCard"] = card;
    fJsonHeader["prePaymentAmount"] = prepaid;
    fJsonHeader["paidAmount"] = cash;
    QString json = QString(
                       "{\"seq\":1,\"paidAmount\":%1, \"paidAmountCard\":%2, \"partialAmount\":0, "
                       "\"prePaymentAmount\":%3, \"useExtPOS\":%4, \"mode\":2, %5 %6 \"items\":[")
                       .arg(float_str_pt(fJsonHeader["paidAmount"].toDouble(), 2))
                       .arg(float_str_pt(fJsonHeader["paidAmountCard"].toDouble(), 2))
                       .arg(float_str_pt(fJsonHeader["prePaymentAmount"].toDouble(), 2))
                       .arg(fJsonHeader["userExtPOS"].toString())
                       .arg(fPartnerTin.length() > 0
                                ? QString("\"partnerTin\":\"%1\",").arg(fPartnerTin)
                                : "\"partnerTin\":null,")
                       .arg(fEmarks.isEmpty() ? "" : QString("\"eMarks\":[%1],").arg(emarks));
    bool first = true;

    for(int i = 0; i < fJsonGoods.count(); i++) {
        if(first) {
            first = false;
        } else {
            json += ",";
        }

        QMap<QString, QJsonValue>& g = fJsonGoods[i];
        QString discount;

        if(g.contains("discount")) {
            discount = QString(",\"discount\":%1, \"discountType\":%2")
                       .arg(g["discount"].toDouble())
                       .arg(g["discountType"].toInt());
        }

        json +=   QString("{\"adgCode\":\"%1\", \"dep\":%2, \"price\":%3, \"productCode\":\"%4\", "
                          "\"productName\":\"%5\", \"qty\":%6, \"unit\":\"%7\" %8}")
                  .arg(g["adgCode"].toString())
                  .arg(g["dep"].toInt())
                  .arg(g["price"].toDouble())
                  .arg(g["productCode"].toInt())
                  .arg(g["productName"].toString())
                  .arg(g["qty"].toDouble())
                  .arg(g["unit"].toString())
                  .arg(discount);
    }

    json += "]}";
    QByteArray jdata = json.toUtf8();
    int result = printJSON(jdata, err, opcode_print_receipt);
    //json.replace("\\\"", "\\\\\"");
    outInJson = json;
    outOutJson = jdata;

    if(result != 0) {
#ifdef QT_DEBUG
        outOutJson =
            QString("{\"rseq\":%1,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1676794194840,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}")
            .arg(mDebugRseq);
        outOutJson =
            QString("{\"address\":\"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Ամիրյան 4/3 \",\"change\":0.0,\"crn\":\"53235782\",\"fiscal\":\"54704153\",\"lottery\":\"\",\"prize\":0,\"rseq\":%1,\"sn\":\"00022154380\",\"taxpayer\":\"«ՊԼԱԶԱ ՍԻՍՏԵՄՍ»\",\"time\":1709630105632,\"tin\":\"02596277\",\"total\":93600.0}")
            .arg(mDebugRseq++);
        result = DEBUG_RESULT;
#endif
    }

    if(result == 0) {
        while(outOutJson.length() > 0 && outOutJson.at(outOutJson.length() - 1) != "}") {
            outOutJson.remove(outOutJson.length() - 1, 1);
        }
    }

    return result;
}

int PrintTaxNO::makeJsonAndPrintSimple(
    int dep, double card, double prepaid, const QString &useExtPos, QString &outInJson, QString &outOutJson, QString &err)
{
    QString emarks;

    for(QString e : fEmarks) {
        if(!emarks.isEmpty()) {
            emarks += ",";
        }

        //e.replace("'", "\\\\'");
        e.replace("\\", "\\\\");
        e.replace("\"", "\\\"");
        emarks += QString("\"%1\"").arg(e);
    }

    fJsonHeader["paidAmountCard"] = card;
    fJsonHeader["prePaymentAmount"] = prepaid;
    fJsonHeader["paidAmount"] = fJsonHeader["paidAmount"].toDouble() - card - prepaid;
    QString json = QString("{\"seq\":1,\"paidAmount\":%1, \"paidAmountCard\":%2, \"partialAmount\":0, "
                           "\"dep\":%7,"
                           "\"prePaymentAmount\":%3, \"useExtPOS\":%4, \"mode\":1, %5 %6 \"items\":null}")
                       .arg(float_str_pt(fJsonHeader["paidAmount"].toDouble(), 2))
                       .arg(float_str_pt(fJsonHeader["paidAmountCard"].toDouble(), 2))
                       .arg(float_str_pt(fJsonHeader["prePaymentAmount"].toDouble(), 2))
                       .arg(useExtPos)
                       .arg(fPartnerTin.length() > 0 ? QString("\"partnerTin\":\"%1\",").arg(fPartnerTin) : "\"partnerTin\":null,")
                       .arg(fEmarks.isEmpty() ? QString("\"eMarks\":[],") : QString("\"eMarks\":[%1],").arg(emarks))
                       .arg(dep);
    QByteArray jdata = json.toUtf8();

    int result = printJSON(jdata, err, opcode_print_receipt);
    //json.replace("\\\"", "\\\\\"");
    outInJson = json;
    outOutJson = jdata;

    if (result != 0) {
#ifdef QT_DEBUG
        outOutJson = QString("{\"rseq\":%1,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ "
                             "կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց "
                             "34\",\"time\":1676794194840,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,"
                             "\"change\":0.0}")
                         .arg(mDebugRseq);
        outOutJson = QString("{\"address\":\"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Ամիրյան 4/3 "
                             "\",\"change\":0.0,\"crn\":\"53235782\",\"fiscal\":\"54704153\",\"lottery\":\"\",\"prize\":0,\"rseq\":%1,"
                             "\"sn\":\"00022154380\",\"taxpayer\":\"«ՊԼԱԶԱ "
                             "ՍԻՍՏԵՄՍ»\",\"time\":1709630105632,\"tin\":\"02596277\",\"total\":93600.0}")
                         .arg(mDebugRseq);
        result = DEBUG_RESULT;
#endif
    }

    if (result == 0) {
        while (outOutJson.length() > 0 && outOutJson.at(outOutJson.length() - 1) != "}") {
            outOutJson.remove(outOutJson.length() - 1, 1);
        }
    }

    return result;
}

int PrintTaxNO::printAdvanceJson(double advanceCash, double advanceCard, QString &outInJson, QString &outOutJson, QString &err)
{
    outInJson = QString("{\"seq\":1, "
                        "\"paidAmount\":%1, \"paidAmountCard\":%2,"
                        "\"prePaymentAmount\":0.0,"
                        "\"mode\":3, \"useExtPOS\":%3}")
                    .arg(advanceCash)
                    .arg(advanceCard)
                    .arg(fJsonHeader["userExtPOS"].toString());
    QByteArray jdata = outInJson.toUtf8();
    int result = printJSON(jdata, err, opcode_print_receipt);

    outOutJson = jdata;
#ifdef QT_DEBUG
    outOutJson
        = QString(
              "{\"rseq\":%1,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա "
              "ՍՊԸ»\",\"address\":\"Արշակունյանց "
              "34\",\"time\":1676794194840,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}")
              .arg(mDebugRseq);
    outOutJson = QString("{\"address\":\"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Ամիրյան 4/3 "
                         "\",\"change\":0.0,\"crn\":\"53235782\",\"fiscal\":\"54704153\",\"lottery\":\"\",\"prize\":0,\"rseq\":%1,\"sn\":"
                         "\"00022154380\",\"taxpayer\":\"«ՊԼԱԶԱ "
                         "ՍԻՍՏԵՄՍ»\",\"time\":1709630105632,\"tin\":\"02596277\",\"total\":93600.0}")
                     .arg(mDebugRseq);
    result = DEBUG_RESULT;
#endif

    if (result == 0) {
        while (outOutJson.length() > 0 && outOutJson.at(outOutJson.length() - 1) != "}") {
            outOutJson.remove(outOutJson.length() - 1, 1);
        }
    }

    return result;
}

int PrintTaxNO::printTaxback(int number, const QString &crn, QString &outInJson, QString &outOutJson, QString &err)
{
    QString emarksStr;

    for (QString e : fEmarks) {
        if (!emarksStr.isEmpty()) {
            emarksStr += ",";
        }

        //e.replace("'", "\\\\'");
        e.replace("\\", "\\\\");
        e.replace("\"", "\\\"");
        emarksStr += QString("\"%1\"").arg(e);
    }

    emarksStr = ", \"emarks\": [ " + emarksStr + " ]";
    outInJson = QString("{\"crn\":\"%1\",\"returnTicketId\":\"%2\",\"seq\":1 %3 %4 %5}")
                    .arg(crn, QString::number(number))
                    .arg(fReturnItemList.isEmpty() ? ""
                                                   : QString(",\"cashAmountForReturn\":%1,"
                                                             "\"cardAmountForReturn\":%2,"
                                                             "\"prePaymentAmountForReturn\":%3")
                                                         .arg(fCashAmountForReturn)
                                                         .arg(fCardAmountForReturn)
                                                         .arg(fPrepaymentAmountForReturn))
                    .arg(fReturnItemList.isEmpty() ? "" : ",\"returnItemList\":" + QString(QJsonDocument(fReturnItemList).toJson()))
                    .arg(fEmarks.isEmpty() ? "" : emarksStr);
    QByteArray jdata = outInJson.toUtf8();
    int result = printJSON(jdata, err, opcode_taxback);
#ifdef QT_DEBUG
    result = DEBUG_RESULT;
#endif
    outOutJson = jdata;
    return result;
}

int PrintTaxNO::printReport(const QDate &d1, const QDate &d2, int type, QString &inJson, QString &outJson, QString &err)
{
    //    QDateTime start = QDateTime::fromMSecsSinceEpoch(d1.toMSecsSinceEpoch() + 1000);
    //    QDateTime end = QDateTime::fromMSecsSinceEpoch(d2.toMSecsSinceEpoch() + 86399000);
    QDateTime start = QDateTime::fromString(d1.toString("yyyy-MM-dd") + " 00:00:01", "yyyy-MM-dd HH:mm:ss");
    QDateTime end = QDateTime::fromString(d2.toString("yyyy-MM-dd") + " 23:59:59", "yyyy-MM-dd HH:mm:ss");
    inJson = QString("{\"seq\":1,\"reportType\":%1,\"startDate\":%2,\"endDate\":%3}")
                 .arg(type)
                 .arg(start.toMSecsSinceEpoch())
                 .arg(end.toMSecsSinceEpoch());
    QByteArray jdata = inJson.toUtf8();
    int result = printJSON(jdata, err, opcode_report);
    outJson = jdata;
    return result;
}
