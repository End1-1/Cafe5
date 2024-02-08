#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T11:27:26
#
#-------------------------------------------------

QT       += core gui printsupport network sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shop
TEMPLATE = app

RC_FILE = res.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../../../Soft/OpenSSL-Win32/include/openssl/applink.c \
    ../../XLSX/src/xlsx.cpp \
    ../../XLSX/src/xlsxborder.cpp \
    ../../XLSX/src/xlsxcell.cpp \
    ../../XLSX/src/xlsxcontenttype.cpp \
    ../../XLSX/src/xlsxdocpropsapp.cpp \
    ../../XLSX/src/xlsxdocpropscore.cpp \
    ../../XLSX/src/xlsxdocument.cpp \
    ../../XLSX/src/xlsxrels.cpp \
    ../../XLSX/src/xlsxsharedstring.cpp \
    ../../XLSX/src/xlsxsheet.cpp \
    ../../XLSX/src/xlsxstyles.cpp \
    ../../XLSX/src/xlsxtheme.cpp \
    ../../XLSX/src/xlsxworkbook.cpp \
    ../../XLSX/src/xlsxwriter.cpp \
    ../Cafe5/c5cafecommon.cpp \
    ../Cafe5/c5double.cpp \
    ../Cafe5/c5logsystem.cpp \
    ../Cafe5/c5replication.cpp \
    ../Cafe5/c5serverhandler.cpp \
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../Classes/c5airlog.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5document.cpp \
    ../Classes/c5filelogwriter.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5orderdriver.cpp \
    ../Classes/c5ordertabledelegate.cpp \
    ../Classes/c5permissions.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printjson.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/c5printreceiptthread50mm.cpp \
    ../Classes/c5printrecipta4.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Classes/c5printservicethread.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5replacecharacter.cpp \
    ../Classes/c5reporttemplatedriver.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5translator.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5waiterorderdoc.cpp \
    ../Classes/c5widget.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/calculator.cpp \
    ../Classes/checkforupdatethread.cpp \
    ../Classes/dataonline.cpp \
    ../Classes/dlgpassword.cpp \
    ../Classes/doubledatabase.cpp \
    ../Classes/fileversion.cpp \
    ../Classes/idram.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/outputofheader.cpp \
    ../Classes/proxytablewidgetdatabase.cpp \
    ../Classes/removeshopsale.cpp \
    ../Classes/rkeyboard.cpp \
    ../Classes/serverconnection.cpp \
    ../Classes/socketconnection.cpp \
    ../Classes/thread.cpp \
    ../Classes/threadsendmessage.cpp \
    ../Configs/breezeconfig.cpp \
    ../Configs/cashboxconfig.cpp \
    ../Configs/configs.cpp \
    ../Controls/c5cleartablewidget.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Controls/c5guicontrols.cpp \
    ../DbData/datadriver.cpp \
    ../DbData/dbbodystate.cpp \
    ../DbData/dbcar.cpp \
    ../DbData/dbcurrency.cpp \
    ../DbData/dbdata.cpp \
    ../DbData/dbdishcomments.cpp \
    ../DbData/dbdishes.cpp \
    ../DbData/dbdishpart1.cpp \
    ../DbData/dbdishpart2.cpp \
    ../DbData/dbdishremovereason.cpp \
    ../DbData/dbdishspecial.cpp \
    ../DbData/dbgoods.cpp \
    ../DbData/dbgoodsgroup.cpp \
    ../DbData/dbhalls.cpp \
    ../DbData/dbmenu.cpp \
    ../DbData/dbmenuname.cpp \
    ../DbData/dbmenupackagelist.cpp \
    ../DbData/dbmenupackages.cpp \
    ../DbData/dboheader.cpp \
    ../DbData/dbopreorder.cpp \
    ../DbData/dbpartner.cpp \
    ../DbData/dbservicevalues.cpp \
    ../DbData/dbstore.cpp \
    ../DbData/dbstoredoctype.cpp \
    ../DbData/dbtables.cpp \
    ../DbData/dbunit.cpp \
    ../DbData/dbusers.cpp \
    ../Forms/c5printtaxanywhere.cpp \
    ../Forms/dlgdataonline.cpp \
    ../Forms/dlgpaymentchoose.cpp \
    ../Forms/dlgqrcode.cpp \
    ../Forms/dlgreservgoods.cpp \
    ../Forms/dlgserversettings.cpp \
    ../Printing/printreceipt.cpp \
    ../Printing/printreceiptgroup.cpp \
    ../Printing/selectprinters.cpp \
    ../Service5/handlers/httpheader.cpp \
    ../Service5/handlers/requesthandler.cpp \
    ../Service5/handlers/requestmanager.cpp \
    ../Service5/handlers/socketdata.cpp \
    ../Service5/socket/sslsocket.cpp \
    ../Service5/utils/commandline.cpp \
    ../Service5/utils/configini.cpp \
    ../Service5/utils/database.cpp \
    ../Service5/utils/databaseconnectionmanager.cpp \
    ../Service5/utils/gtranslator.cpp \
    ../Service5/utils/jsonhandler.cpp \
    ../Service5/utils/logwriter.cpp \
    ../Service5/utils/networktable.cpp \
    ../Service5/utils/os.cpp \
    ../Service5/utils/sqlqueries.cpp \
    ../Service5/utils/storemovement.cpp \
    ../Service5/utils/tablerecord.cpp \
    ../Service5Working/raw/raw.cpp \
    ../Service5Working/raw/rawbalancehistory.cpp \
    ../Service5Working/raw/rawcarnear.cpp \
    ../Service5Working/raw/rawcoordinate.cpp \
    ../Service5Working/raw/rawdllop.cpp \
    ../Service5Working/raw/rawhello.cpp \
    ../Service5Working/raw/rawmessage.cpp \
    ../Service5Working/raw/rawmonitor.cpp \
    ../Service5Working/raw/rawpluginobject.cpp \
    ../Service5Working/raw/rawregistersms.cpp \
    ../Service5Working/raw/rawsilentauth.cpp \
    ../Service5Working/raw/rawyandexkey.cpp \
    ../Service5Working/socket/sslserver.cpp \
    ../Service5Working/socket/sslsocket.cpp \
    ../Service5Working/utils/commandline.cpp \
    ../Service5Working/utils/configini.cpp \
    ../Service5Working/utils/database.cpp \
    ../Service5Working/utils/databaseconnectionmanager.cpp \
    ../Service5Working/utils/gtranslator.cpp \
    ../Service5Working/utils/jsonhandler.cpp \
    ../Service5Working/utils/logwriter.cpp \
    ../Service5Working/utils/networktable.cpp \
    ../Service5Working/utils/os.cpp \
    ../Service5Working/utils/sqlqueries.cpp \
    ../Service5Working/utils/storemovement.cpp \
    ../Service5Working/utils/tablerecord.cpp \
    ../TableRecord/aheader.cpp \
    ../TableRecord/bclientdebts.cpp \
    ../TableRecord/bhistory.cpp \
    ../TableRecord/c5dbrecord.cpp \
    ../TableRecord/cpartners.cpp \
    ../TableRecord/ecash.cpp \
    ../TableRecord/odraftsale.cpp \
    ../TableRecord/odraftsalebody.cpp \
    ../TableRecord/ogoods.cpp \
    ../TableRecord/oheader.cpp \
    ../TableRecord/otax.cpp \
    c5tempsale.cpp \
    cashcollection.cpp \
    dlgdate.cpp \
    dlggetidname.cpp \
    dlggiftcardsale.cpp \
    dlggoodslist.cpp \
    dlgpin.cpp \
    dlgreturnitem.cpp \
    dlgsearchpartner.cpp \
    dlgshowcolumns.cpp \
    dlgsplashscreen.cpp \
    goodsreturnreason.cpp \
    imageloader.cpp \
    loghistory.cpp \
        main.cpp \
    replicadialog.cpp \
    sales.cpp \
    searchitems.cpp \
    selectstaff.cpp \
    settingsselection.cpp \
    storeinput.cpp \
    taxprint.cpp \
    threadcheckmessage.cpp \
    threadreadmessage.cpp \
    threadworker.cpp \
    userphoto.cpp \
    viewinputitem.cpp \
    vieworder.cpp \
    wcustomerdisplay.cpp \
        working.cpp \
    worder.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Controls/c5lineedit.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Classes/c5cache.cpp \
    ../../NewTax/Src/printtaxn.cpp \
    dqty.cpp \
    ../Controls/c5checkbox.cpp

HEADERS += \
    ../../../Soft/OpenSSL-Win32/include/openssl/__DECC_INCLUDE_EPILOGUE.H \
    ../../../Soft/OpenSSL-Win32/include/openssl/__DECC_INCLUDE_PROLOGUE.H \
    ../../../Soft/OpenSSL-Win32/include/openssl/aes.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/asn1.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/asn1_mac.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/asn1err.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/asn1t.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/async.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/asyncerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/bio.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/bioerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/blowfish.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/bn.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/bnerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/buffer.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/buffererr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/camellia.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/cast.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/cmac.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/cms.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/cmserr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/comp.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/comperr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/conf.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/conf_api.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/conferr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/crypto.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/cryptoerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ct.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/cterr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/des.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/dh.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/dherr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/dsa.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/dsaerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/dtls1.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/e_os2.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ebcdic.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ec.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ecdh.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ecdsa.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ecerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/engine.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/engineerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/err.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/evp.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/evperr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/hmac.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/idea.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/kdf.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/kdferr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/lhash.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/md2.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/md4.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/md5.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/mdc2.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/modes.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/obj_mac.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/objects.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/objectserr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ocsp.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ocsperr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/opensslconf.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/opensslv.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ossl_typ.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pem.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pem2.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pemerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pkcs12.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pkcs12err.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pkcs7.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/pkcs7err.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rand.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rand_drbg.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/randerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rc2.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rc4.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rc5.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ripemd.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rsa.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/rsaerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/safestack.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/seed.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/sha.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/srp.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/srtp.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ssl.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ssl2.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ssl3.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/sslerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/stack.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/store.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/storeerr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/symhacks.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/tls1.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ts.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/tserr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/txt_db.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/ui.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/uierr.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/whrlpool.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/x509.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/x509_vfy.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/x509err.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/x509v3.h \
    ../../../Soft/OpenSSL-Win32/include/openssl/x509v3err.h \
    ../../XLSX/src/crs32.h \
    ../../XLSX/src/xlsx.h \
    ../../XLSX/src/xlsxall.h \
    ../../XLSX/src/xlsxborder.h \
    ../../XLSX/src/xlsxcell.h \
    ../../XLSX/src/xlsxcontenttype.h \
    ../../XLSX/src/xlsxdocpropsapp.h \
    ../../XLSX/src/xlsxdocpropscore.h \
    ../../XLSX/src/xlsxdocument.h \
    ../../XLSX/src/xlsxrels.h \
    ../../XLSX/src/xlsxsharedstring.h \
    ../../XLSX/src/xlsxsheet.h \
    ../../XLSX/src/xlsxstyles.h \
    ../../XLSX/src/xlsxtheme.h \
    ../../XLSX/src/xlsxworkbook.h \
    ../../XLSX/src/xlsxwriter.h \
    ../../XLSX/src/zip.h \
    ../Cafe5/c5cafecommon.h \
    ../Cafe5/c5double.h \
    ../Cafe5/c5logsystem.h \
    ../Cafe5/c5replication.h \
    ../Cafe5/c5serverhandler.h \
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/QRCodeGenerator.h \
    ../Classes/barcode5.h \
    ../Classes/c5airlog.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5crypt.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5document.h \
    ../Classes/c5filelogwriter.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/c5menu.h \
    ../Classes/c5orderdriver.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5permissions.h \
    ../Classes/c5printing.h \
    ../Classes/c5printjson.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/c5printreceiptthread50mm.h \
    ../Classes/c5printrecipta4.h \
    ../Classes/c5printremovedservicethread.h \
    ../Classes/c5printservicethread.h \
    ../Classes/c5random.h \
    ../Classes/c5replacecharacter.h \
    ../Classes/c5reporttemplatedriver.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5tableview.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5translator.h \
    ../Classes/c5user.h \
    ../Classes/c5waiterorderdoc.h \
    ../Classes/c5widget.h \
    ../Classes/c5witerconf.h \
    ../Classes/calculator.h \
    ../Classes/chatmessage.h \
    ../Classes/checkforupdatethread.h \
    ../Classes/dataonline.h \
    ../Classes/dlgpassword.h \
    ../Classes/doubledatabase.h \
    ../Classes/fileversion.h \
    ../Classes/goodsreserve.h \
    ../Classes/idram.h \
    ../Classes/notificationwidget.h \
    ../Classes/outputofheader.h \
    ../Classes/proxytablewidgetdatabase.h \
    ../Classes/removeshopsale.h \
    ../Classes/rkeyboard.h \
    ../Classes/serverconnection.h \
    ../Classes/socketconnection.h \
    ../Classes/thread.h \
    ../Classes/threadsendmessage.h \
    ../Configs/breezeconfig.h \
    ../Configs/cashboxconfig.h \
    ../Configs/configs.h \
    ../Controls/c5cleartablewidget.h \
    ../Controls/c5graphicsview.h \
    ../Controls/c5guicontrols.h \
    ../DbData/datadriver.h \
    ../DbData/dbbodystate.h \
    ../DbData/dbcar.h \
    ../DbData/dbcurrency.h \
    ../DbData/dbdata.h \
    ../DbData/dbdishcomments.h \
    ../DbData/dbdishes.h \
    ../DbData/dbdishpart1.h \
    ../DbData/dbdishpart2.h \
    ../DbData/dbdishremovereason.h \
    ../DbData/dbdishspecial.h \
    ../DbData/dbgoods.h \
    ../DbData/dbgoodsgroup.h \
    ../DbData/dbhalls.h \
    ../DbData/dbmenu.h \
    ../DbData/dbmenuname.h \
    ../DbData/dbmenupackagelist.h \
    ../DbData/dbmenupackages.h \
    ../DbData/dboheader.h \
    ../DbData/dbopreorder.h \
    ../DbData/dbpartner.h \
    ../DbData/dbservicevalues.h \
    ../DbData/dbstore.h \
    ../DbData/dbstoredoctype.h \
    ../DbData/dbtables.h \
    ../DbData/dbunit.h \
    ../DbData/dbusers.h \
    ../Forms/c5printtaxanywhere.h \
    ../Forms/dlgdataonline.h \
    ../Forms/dlgpaymentchoose.h \
    ../Forms/dlgqrcode.h \
    ../Forms/dlgreservgoods.h \
    ../Forms/dlgserversettings.h \
    ../Printing/printreceipt.h \
    ../Printing/printreceiptgroup.h \
    ../Printing/selectprinters.h \
    ../Service5/handlers/httpheader.h \
    ../Service5/handlers/requesthandler.h \
    ../Service5/handlers/requestmanager.h \
    ../Service5/handlers/socketdata.h \
    ../Service5/socket/sslsocket.h \
    ../Service5/utils/commandline.h \
    ../Service5/utils/configini.h \
    ../Service5/utils/database.h \
    ../Service5/utils/databaseconnectionmanager.h \
    ../Service5/utils/gtranslator.h \
    ../Service5/utils/jsonhandler.h \
    ../Service5/utils/logwriter.h \
    ../Service5/utils/networktable.h \
    ../Service5/utils/os.h \
    ../Service5/utils/sqlqueries.h \
    ../Service5/utils/storemovement.h \
    ../Service5/utils/tablerecord.h \
    ../Service5Working/raw/messagelist.h \
    ../Service5Working/raw/raw.h \
    ../Service5Working/raw/rawbalancehistory.h \
    ../Service5Working/raw/rawcarnear.h \
    ../Service5Working/raw/rawcoordinate.h \
    ../Service5Working/raw/rawdllop.h \
    ../Service5Working/raw/rawhello.h \
    ../Service5Working/raw/rawmessage.h \
    ../Service5Working/raw/rawmonitor.h \
    ../Service5Working/raw/rawpluginobject.h \
    ../Service5Working/raw/rawregistersms.h \
    ../Service5Working/raw/rawsilentauth.h \
    ../Service5Working/raw/rawyandexkey.h \
    ../Service5Working/raw/structs.h \
    ../Service5Working/socket/sslserver.h \
    ../Service5Working/socket/sslsocket.h \
    ../Service5Working/utils/commandline.h \
    ../Service5Working/utils/configini.h \
    ../Service5Working/utils/database.h \
    ../Service5Working/utils/databaseconnectionmanager.h \
    ../Service5Working/utils/gtranslator.h \
    ../Service5Working/utils/jsonhandler.h \
    ../Service5Working/utils/logwriter.h \
    ../Service5Working/utils/networktable.h \
    ../Service5Working/utils/os.h \
    ../Service5Working/utils/sqlqueries.h \
    ../Service5Working/utils/storemovement.h \
    ../Service5Working/utils/tablerecord.h \
    ../TableRecord/aheader.h \
    ../TableRecord/bclientdebts.h \
    ../TableRecord/bhistory.h \
    ../TableRecord/c5dbrecord.h \
    ../TableRecord/cpartners.h \
    ../TableRecord/ecash.h \
    ../TableRecord/odraftsale.h \
    ../TableRecord/odraftsalebody.h \
    ../TableRecord/ogoods.h \
    ../TableRecord/oheader.h \
    ../TableRecord/otax.h \
    c5tempsale.h \
    cashcollection.h \
    dlgdate.h \
    dlggetidname.h \
    dlggiftcardsale.h \
    dlggoodslist.h \
    dlgpin.h \
    dlgpreorderitem.h \
    dlgreturnitem.h \
    dlgsearchpartner.h \
    dlgshowcolumns.h \
    dlgsplashscreen.h \
    goodsreturnreason.h \
    imageloader.h \
    loghistory.h \
    replicadialog.h \
    sales.h \
    searchitems.h \
    selectstaff.h \
    settingsselection.h \
    storeinput.h \
    taxprint.h \
    threadcheckmessage.h \
    threadreadmessage.h \
    threadworker.h \
    userphoto.h \
    viewinputitem.h \
    vieworder.h \
    wcustomerdisplay.h \
        working.h \
    worder.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Controls/c5lineedit.h \
    ../Controls/c5combobox.h \
    ../Controls/c5dateedit.h \
    ../Classes/c5cache.h \
    ../../NewTax/Src/printtaxn.h \
    dqty.h \
    ../Controls/c5checkbox.h

FORMS += \
    ../Cafe5/calendar.ui \
    ../Cafe5/dlgexitbyversion.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/c5printpreview.ui \
    ../Classes/calculator.ui \
    ../Classes/dlgpassword.ui \
    ../Classes/notificationwidget.ui \
    ../Classes/rkeyboard.ui \
    ../Controls/c5tablewithtotal.ui \
    ../Forms/c5printtaxanywhere.ui \
    ../Forms/dlgdataonline.ui \
    ../Forms/dlgpaymentchoose.ui \
    ../Forms/dlgqrcode.ui \
    ../Forms/dlgreservgoods.ui \
    ../Forms/dlgserversettings.ui \
    ../Printing/selectprinters.ui \
    c5tempsale.ui \
    cashcollection.ui \
    dlgdate.ui \
    dlggetidname.ui \
    dlggiftcardsale.ui \
    dlggoodslist.ui \
    dlgpin.ui \
    dlgreturnitem.ui \
    dlgsearchpartner.ui \
    dlgshowcolumns.ui \
    dlgsplashscreen.ui \
    goodsreturnreason.ui \
    loghistory.ui \
    replicadialog.ui \
    sales.ui \
    searchitems.ui \
    selectstaff.ui \
    settingsselection.ui \
    storeinput.ui \
    taxprint.ui \
    userphoto.ui \
    viewinputitem.ui \
    vieworder.ui \
    wcustomerdisplay.ui \
        working.ui \
    worder.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dqty.ui

DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += _ORGANIZATION_=\\\"Hotelicca\\\"
DEFINES += _APPLICATION_=\\\"Shop\\\"
DEFINES += _MODULE_=\\\"Shop\\\"
DEFINES += SHOP
DEFINES += _CRYPT_KEY_=\\\"shop2022!!!\\\"
DEFINES += WAITER

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../TableRecord
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Cache
INCLUDEPATH += ../Controls
INCLUDEPATH += ../DbData
INCLUDEPATH += ../Forms
INCLUDEPATH += ../Configs
INCLUDEPATH += ../Service
INCLUDEPATH += ../Printing
INCLUDEPATH += ../Service5
INCLUDEPATH += ../Service5/raw
INCLUDEPATH += ../Service5Working/raw
INCLUDEPATH += ../Service5/socket
INCLUDEPATH += ../Service5/utils
INCLUDEPATH += ../Service5/handlers
INCLUDEPATH += ../RESOURCES
INCLUDEPATH += ../../XLSX/Src
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include/openssl

ICON = bag.ico

RESOURCES += \
    ../resources/res.qrc \
    local.qrc

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

