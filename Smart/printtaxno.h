#ifndef PRINTTAXN_H
#define PRINTTAXN_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QElapsedTimer>
#include <QDate>
#include <QJsonArray>
#include <QTime>

#define pt_err_wait_for_write -8
#define pt_err_crn_empty -7
#define pt_err_no_tax_in_db -6
#define pt_err_socket_read -5
#define pt_err_nodata_data -4
#define pt_err_nodata_login -3
#define pt_err_weak_3des_key -2
#define pt_err_cannot_connect_to_host -1
#define pt_err_ok 0

#define opcode_login 2
#define opcode_logout 3
#define opcode_print_receipt 4
#define opcode_taxback 6
#define opcode_report 9

#define report_x 1
#define report_z 2

class PrintTaxNO : public QObject
{
    Q_OBJECT
public:
    QString fIP;
    quint16 fPort;
    QString fPassword;
    QByteArray fPassSHA256;
    QByteArray fSessionPass;
    QTcpSocket fTcpSocket;
    static QMap<int, QString> fErrors;
    int connectToHost(QString &err);
    bool jsonLogin(QByteArray &out);
    bool jsonLogout(QByteArray &out);
    void makeRequestHeader(quint8 *dst, quint8 request, quint16 dataLen);
    int getResponse(QByteArray &out, QString &err);
    void cryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData);
    void decryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData);
    QMap<QString, QVariant> fJsonHeader;
    QList<QMap<QString, QJsonValue> > fJsonGoods;
    static int mDebugRseq;

public:
    explicit PrintTaxNO(const QString &ip,
                        int port,
                        const QString &password,
                        const QString &extPos,
                        const QString &opcode,
                        const QString &oppin,
                        QObject *parent = nullptr);
    void setParams(const QString &ip, int port, const QString &password, const QString &op, const QString &pin,
                   const QString &extPos);
    void initErrors();
    int printJSON(QByteArray &jsonData, QString &err, quint8 opcode);
    void addGoods(int dep, const QString &adgt, const QString &code, const QString &name, double price, double qty,
                  double discount);
    void addReturnItem(int row, double qty);
    int makeJsonAndPrint(double card, double prepaid, QString &outInJson, QString &outOutJson, QString &err);
    int makeJsonAndPrint(double cash, double card, double prepaid, QString &outInJson, QString &outOutJson, QString &err);
    int makeJsonAndPrintSimple(
        int dep, double card, double prepaid, const QString &useExtPos, QString &outInJson, QString &outOutJson, QString &err);
    int printAdvanceJson(double advanceCash, double advanceCard, QString &outInJson, QString &outOutJson, QString &err);
    int printTaxback(int number, const QString &crn, QString &outInJson, QString &outOutJson, QString &err);
    int printReport(const QDate &d1, const QDate &d2, int type, QString &inJson, QString &outJson, QString &err);
    QJsonDocument fJSONDoc;
    QString fPartnerTin;
    QString fTaxCashier;
    QString fTaxPin;
    QStringList fEmarks;
    double fCashAmountForReturn;
    double fCardAmountForReturn;
    double fPrepaymentAmountForReturn;
signals:
    void started();
    void done(int code, const QString &message);

private:
    PrintTaxNO();
    void logMessage(const QString &msg);
    QJsonArray fReturnItemList;
};

#endif // PRINTTAXN_H
