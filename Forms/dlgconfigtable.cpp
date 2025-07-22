#include "dlgconfigtable.h"
#include "ui_dlgconfigtable.h"
#include "QRCodeGenerator.h"
#include "c5config.h"

DlgConfigTable::DlgConfigTable() :
    C5Dialog(),
    ui(new Ui::DlgConfigTable)
{
    ui->setupUi(this);
    int levelIndex = 1;
    int versionIndex = 0;
    bool bExtent = true;
    int maskIndex = -1;
    //Storage name, IP, port, login, password, store, only read
    QString encodeString = __c5config.getValue(param_shop_config_mobile_client);
    CQR_Encode qrEncode;
    bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
                              encodeString.toUtf8().data() );
    if (!successfulEncoding) {
        //fLog.append("Cannot encode qr image");
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
    QPixmap pix = QPixmap::fromImage(encodeImage).scaled(180, 180);
    ui->lbQR->setPixmap(pix);
}

DlgConfigTable::~DlgConfigTable()
{
    delete ui;
}
