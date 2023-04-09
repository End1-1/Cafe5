#include "c5mainwindow.h"
#include "ui_c5mainwindow.h"
#include "c5message.h"
#include "c5connection.h"
#include "c5login.h"
#include "c5cashdoc.h"
#include "cr5cashdetailed.h"
#include "c5permissions.h"
#include "c5widget.h"
#include "cr5commonsales.h"
#include "checkforupdatethread.h"
#include "cr5usersgroups.h"
#include "cr5materialinstoreuncomplect.h"
#include "cr5consumptionbysales.h"
#include "cr5consumptionbysalesdraft.h"
#include "cr5storereason.h"
#include "cr5carvisits.h"
#include "cr5documents.h"
#include "cr5goodsclasses.h"
#include "c5translatorform.h"
#include "c5goodsspecialprices.h"
#include "cr5breezeservice.h"
#include "cr5reports.h"
#include "cr5mfgeneralreport.h"
#include "cr5routereport.h"
#include "c5saledoc.h"
#include "cr5saleremoveddishes.h"
#include "cr5goodsreservations.h"
#include "c5broadcasting.h"
#include "cr5goodsqtyreminder.h"
#include "cr5cashmovement.h"
#include "cr5currencies.h"
#include "cr5currencycrossrate.h"
#include "cr5currencycrossratehistory.h"
#include "c5costumerdebtpayment.h"
#include "cr5cashnames.h"
#include "cr5dish.h"
#include "cr5settings.h"
#include "cr5complectations.h"
#include "c5selector.h"
#include "cr5goodsmovement.h"
#include "c5salarydoc.h"
#include "c5reporttemplatedriver.h"
#include "cr5creditcards.h"
#include "cr5dishpart1.h"
#include "c5toolbarwidget.h"
#include "cr5generalreportonlydate.h"
#include "cr5mfactions.h"
#include "cr5mfdaily.h"
#include "cr5preorders.h"
#include "cr5currencyratehistory.h"
#include "c5dishselfcostgenprice.h"
#include "cr5custom.h"
#include "cr5salefromstoretotal.h"
#include "cr5materialmoveuncomplect.h"
#include "cr5dishcomment.h"
#include "cr5salesbydishes.h"
#include "cr5tstoreextra.h"
#include "cr5costumerdebts.h"
#include "cr5goodsimages.h"
#include "cr5storedocuments.h"
#include "cr5dishpart2.h"
#include "cr5ordermarks.h"
#include "cr5draftoutputbyrecipe.h"
#include "cr5dishpriceselfcost.h"
#include "c5storeinventory.h"
#include "cr5discountsystem.h"
#include "cr5dishpackage.h"
#include "cr5salesandstore.h"
#include "cr5goodsgroup.h"
#include "cr5menureview.h"
#include "cr5mfproduct.h"
#include "cr5databases.h"
#include "cr5routedaily.h"
#include "cr5consuptionreason.h"
#include "cr5storereason.h"
#include "cr5salarybyworkers.h"
#include "cr5goodspartners.h"
#include "cr5discountstatisics.h"
#include "cr5goodswaste.h"
#include "cr5salefromstore.h"
#include "cr5goodsunit.h"
#include "c5datasynchronize.h"
#include "cr5menunames.h"
#include "cr5mfactionstage.h"
#include "cr5mfactivetasks.h"
#include "cr5mfworkshops.h"
#include "cr5tables.h"
#include "cr5hall.h"
#include "cr5materialsinstore.h"
#include "c5changepassword.h"
#include "c5route.h"
#include "cr5dishremovereason.h"
#include "cr5goods.h"
#include "c5user.h"
#include "cr5users.h"
#include "cr5goodsstorages.h"
#include "c5storedoc.h"
#include <QCloseEvent>
#include <QShortcut>
#include <QProcess>
#include <QMenu>
#include <QPushButton>
#include <QSpacerItem>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QToolButton>
#include <QParallelAnimationGroup>

C5MainWindow *__mainWindow;
QStringList mainDbParams;
QString fUdpBroadcastSession;
const quint32 DATAGRAM_PORT = 3390;

C5MainWindow::C5MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::C5MainWindow)
{
    ui->setupUi(this);
    mainDbParams = __c5config.dbParams();
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    __c5config.fParentWidget = this;
    fStatusLabel = new QLabel(tr("Disconnected"));
    ui->statusBar->addWidget(fStatusLabel);
    fTab = ui->tabWidget;
    enableMenu(false);
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabChange(int)));
    fReportToolbar = nullptr;
    fRightToolbar = new QToolBar();
    addToolBar(fRightToolbar);
    fToolbarWidget = new C5ToolBarWidget(this);
    fToolbarWidget->setUpdateButtonVisible(false);
    fRightToolbar->addWidget(fToolbarWidget);
    ui->actionHome->setEnabled(false);

    QShortcut *f3 = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(f3, SIGNAL(activated()), this, SLOT(hotKey()));
    QShortcut *esc = new QShortcut(QKeySequence("ESC"), this);
    connect(esc, SIGNAL(activated()), this, SLOT(hotKey()));
    QShortcut *ctrlPlush = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
    connect(ctrlPlush, SIGNAL(activated()), this, SLOT(hotKey()));
    QShortcut *ctrlLog = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L), this);
    connect(ctrlLog, &QShortcut::activated, [this]() {
        ui->wLog->setVisible(!ui->wLog->isVisible());
    });
    C5Dialog::setMainWindow(this);
    __mainWindow = this;

    QVariant menuPanelIsVisible = C5Config::getRegValue("menupanel");
    if (menuPanelIsVisible == QVariant::Invalid) {
        menuPanelIsVisible = true;
    }
    ui->wLog->setVisible(false);
    ui->actionLogout->setVisible(false);
    ui->actionChange_password->setVisible(false);
    connect(&fUpdateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
    fUpdateTimer.start(10000);
    ui->wMenu->resize(C5Config::getRegValue("twdbsize", 300).toInt(), 0);
}

C5MainWindow::~C5MainWindow()
{
    fBroadcastListeners.clear();
    fUpdateTimer.stop();
    delete ui;
}

void C5MainWindow::closeEvent(QCloseEvent *event)
{
    if (C5Message::question(tr("Are you sure to close application?")) != QDialog::Accepted) {
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}

void C5MainWindow::removeTab(QWidget *w)
{
    for (int i = 0; i < fTab->count(); i++) {
        if (w == fTab->widget(i)) {
            tabCloseRequested(i);
            return;
        }
    }
}

void C5MainWindow::writeLog(const QString &message)
{
    ui->ptLog->appendPlainText(QString("%1: %2")
                               .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR))
                               .arg(message));
}

void C5MainWindow::addBroadcastListener(C5Widget *w)
{
    if (fBroadcastListeners.values().contains(w)) {
        return;
    }
    fBroadcastListeners.insert(w->fWindowUuid, w);
}

void C5MainWindow::removeBroadcastListener(C5Widget *w)
{
    if (fBroadcastListeners.contains(w->fWindowUuid)) {
        fBroadcastListeners.remove(w->fWindowUuid);
    }
}

void C5MainWindow::tabCloseRequested(int index)
{
    auto *w = static_cast<C5Widget*>(fTab->widget(index));
    QString prevWindow, currWindow;
    if (fPrevTabUuid.count() > 1) {
        prevWindow = fPrevTabUuid.at(fPrevTabUuid.count() - 2);
    }
    currWindow = w->fWindowUuid;
    fTab->removeTab(index);
    delete w;
    if (!prevWindow.isEmpty() && index == fTab->currentIndex()) {
        for (int i = 0; i < fTab->count(); i++) {
            w = static_cast<C5Widget*>(fTab->widget(i));
            if (w->fWindowUuid == prevWindow) {
                fPrevTabUuid.append(w->fWindowUuid);
                fTab->setCurrentIndex(i);
                fPrevTabUuid.removeAll(w->fWindowUuid);
                return;
            }
        }
    } else {
        fPrevTabUuid.removeAll(currWindow);
    }
}

void C5MainWindow::currentTabChange(int index)
{
    if (fReportToolbar) {
        removeToolBar(fReportToolbar);
    }
    fReportToolbar = nullptr;
    auto *w = dynamic_cast<C5ReportWidget*>(fTab->widget(index));
    auto *w2 = dynamic_cast<C5Widget*>(fTab->widget(index));
    if (!w && !w2) {
        return;
    }
    fPrevTabUuid.append(w2->fWindowUuid);
    if (w) {
        fReportToolbar = w->toolBar();
    } else if (w2) {
        fReportToolbar = w2->toolBar();
    }
    if (fReportToolbar) {
        insertToolBar(fRightToolbar, fReportToolbar);
        fReportToolbar->show();
    }
}

void C5MainWindow::on_actionConnection_triggered()
{
    C5Connection::go<C5Connection>(C5Config::dbParams());
}

void C5MainWindow::on_actionLogin_triggered()
{
    if (!C5Connection::go<C5Login>(C5Config::dbParams())) {
        return;
    }
    showMaximized();
    fStatusLabel->setText(__user->fullName());

    C5Database db(C5Config::fDBHost, C5Config::fDBPath, C5Config::fDBUser, C5Config::fDBPassword);
    db[":f_user"] = __user->id();
    db.exec("select f_name, f_description, f_host, f_db, f_user, f_password, f_main from s_db "
            "where f_id in (select f_db from s_db_access where f_user=:f_user and f_permit=1)");

    while (db.nextRow()) {
        QStringList dbl;
        for (int i = 2; i < db.columnCount(); i++) {
            dbl.append(db.getString(i));
        }
        dbl.append(db.getString("f_description"));
        fDatabases[db.getString("f_description")] = dbl;
    }

    QString lastdb = __c5config.getRegValue("lastdb").toString();
    if(lastdb.isEmpty()) {
        if (fDatabases.count() > 0) {
            if (!fDatabases.contains(lastdb)) {
                QMap<QString, QStringList>::const_iterator it = fDatabases.constBegin();
                lastdb = it.key();
            }
        }
    } else {
        if (!fDatabases.contains(lastdb) && fDatabases.count() > 0) {
            QMap<QString, QStringList>::const_iterator it = fDatabases.constBegin();
            lastdb = it.key();
        }
    }

    QStringList dbParams = fDatabases[__c5config.getRegValue("lastdb").toString()];
    if (dbParams.count() > 3) {
        C5Config::fDBHost = dbParams.at(0);
        C5Config::fDBPath = dbParams.at(1);
        C5Config::fDBUser = dbParams.at(2);
        C5Config::fDBPassword = dbParams.at(3);
        C5Config::initParamsFromDb();
    }

    if (!lastdb.isEmpty()) {
        setDB(lastdb);
    }

    enableMenu(true);
    ui->actionHome->setEnabled(true);
    ui->actionLogin->setVisible(false);
    ui->actionLogout->setVisible(true);
    ui->actionChange_password->setVisible(true);
    if (fUdpBroadcastSession.isEmpty()) {
        fUdpBroadcastSession = C5Database::uuid();
        if (!fUdpSocket.bind(DATAGRAM_PORT, QUdpSocket::ShareAddress)) {

        }
        connect(&fUdpSocket, &QUdpSocket::readyRead, this, &C5MainWindow::datagramRead);
    }
    C5ReportTemplateDriver::init(__user->group());
}

void C5MainWindow::updateTimeout()
{
    fUpdateTimer.stop();
    CheckForUpdateThread *ut = new CheckForUpdateThread(this);
    ut->fApplication = _MODULE_;
    ut->fDbParams = C5Config::dbParams();
    connect(ut, SIGNAL(checked(bool, int, QString)), this, SLOT(updateChecked(bool, int, QString)));
    ut->start();
}

void C5MainWindow::datagramRead()
{
    while (fUdpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(fUdpSocket.pendingDatagramSize());
        QHostAddress remoteAddress;
        quint16 remotePort;
        fUdpSocket.readDatagram(datagram.data(), datagram.size(), &remoteAddress, &remotePort);
        qDebug() << remoteAddress << datagram;
        QJsonParseError jerr;
        QJsonDocument jdoc = QJsonDocument::fromJson(datagram, &jerr);
        QJsonObject jreply;
        jreply["reply"] = 1;
        if (jerr.error == QJsonParseError::NoError) {
            QJsonObject jobj = jdoc.object();
            jreply["what"] = jobj["what"].toInt();
            QString windowsUuid = jobj["windowuuid"].toString();
            switch (jobj["what"].toInt()) {
            case WHAT_GETSERVER: {
                jreply["uuid"] = jobj["uuid"];
                jreply["server_uuid"] = fUdpBroadcastSession;
                jreply["accept"] = ACCEPT_ACCEPT;
                break;
            }
            case WHAT_GETDOCS: {
                QJsonArray jdocs;
                for (C5Widget *w: fBroadcastListeners.values()) {
                    C5StoreDoc *d = dynamic_cast<C5StoreDoc*>(w);
                    if (d) {
                        QJsonObject jdoc;
                        jdoc["date"] = d->docProperty("date").toDate().toString(FORMAT_DATE_TO_STR);
                        jdoc["uuid"] = d->docProperty("uuid").toString();
                        jdoc["docnumber"] = d->docProperty("docnumber").toString();
                        jdoc["inputstorename"] = d->docProperty("inputstorename").toString();
                        jdoc["outputstorename"] = d->docProperty("outputstorename").toString();
                        jdoc["typename"] = d->docProperty("typename").toString();
                        jdoc["typeid"] = d->docProperty("typeid").toInt();
                        jdoc["windowid"] = d->fWindowUuid;
                        jdocs.append(jdoc);
                    }
                }
                QJsonObject jdocobj;
                jdocobj["documents"] = jdocs;
                jreply["data"] = jdocobj;
                break;
            }
            case WHAT_PARSE_STORE_STRING:
            case WHAT_STORE_APPEND_ITEM:
            case WHAT_PARSE_STORE_QTY:
            case WHAT_PARSE_STORE_PRICE:
            case WHAT_PARSE_STORE_AMOUNT:
                for (C5Widget *w: fBroadcastListeners.values()) {
                    if (!windowsUuid.isEmpty()) {
                        if (w->fWindowUuid != windowsUuid) {
                            continue;
                        }
                    }
                    QString replystr;
                    if (!w->parseBroadcastMessage(jobj["what"].toInt(), jobj["data"].toString(), replystr)) {
                        jreply["error"] = 1;
                        jreply["message"] = replystr;
                    }
                    jreply["data"] = QJsonDocument::fromJson(replystr.toUtf8()).object();
                }
                break;
            case WHAT_SET_ACTIVE_WINDOW: {
                bool found = false;
                if (!windowsUuid.isEmpty()) {
                    for (C5Widget *w: fBroadcastListeners.values()) {
                        if (w->fWindowUuid == windowsUuid) {
                            ui->tabWidget->setCurrentWidget(w);
                            found = true;
                        }
                    }
                }
                if (found) {

                } else {
                    jreply["error"] = 1;
                    jreply["message"] = tr("Requested window not exists");
                }
                break;
            }
            }
        } else {
            jreply["error"] = 1;
            jreply["message"] = jerr.errorString();
        }
        datagram = QJsonDocument(jreply).toJson(QJsonDocument::Compact);
        qDebug() << datagram;
        fUdpSocket.writeDatagram(datagram, remoteAddress, remotePort);
    }
}

void C5MainWindow::updateChecked(bool needUpdate, int source, const QString &path)
{
    if (!needUpdate) {
        fUpdateTimer.start(60000);
        return;
    }
    if (path.isEmpty()) {
        fToolbarWidget->setUpdateButtonVisible(true);
        C5Message::info(tr("Update exists, but you will update manually"));
        fUpdateTimer.start(60000);
        return;
    }
    fToolbarWidget->setUpdateButtonVisible(true);
    switch (source) {
    case usNone:
        fUpdateTimer.start(60000);
        break;
    case usLocalnet: {
        QStringList args;
        args << "-lhttp://" + C5Config::fDBHost;
        args << "-s0";
        QProcess p;
        p.start(qApp->applicationDirPath() + "/updater.exe", args);
        break;
    }
    }
}

void C5MainWindow::hotKey()
{
    QShortcut *s = static_cast<QShortcut*>(sender());
    C5Widget *w = static_cast<C5Widget*>(fTab->currentWidget());
    if (!w) {
        return;
    }
    w->hotKey(s->key().toString());
}

void C5MainWindow::enableMenu(bool v)
{
    if (!v) {
        QLayoutItem *li;
        while ((li = ui->lMenu->takeAt(0)) != nullptr) {
            li->widget()->deleteLater();
            delete li;
        }
    }
    fLogin = v;
    if (v) {
        autoLogin();
    }
}

void C5MainWindow::addTreeL3Item(QListWidget *l, int permission, const QString &text, const QString &icon)
{
    if (permission > 0) {
        if (!__user->check(permission)) {
            return;
        }
    }
    QListWidgetItem *item = new QListWidgetItem(l);
    item->setData(Qt::UserRole, permission);
    item->setData(Qt::UserRole + 1, icon);
    int h = (qApp->font().pointSize() * 2) + 2;
    item->setSizeHint(QSize(l->width(), h));

    QWidget *w = new QWidget();
    QHBoxLayout *hl = new QHBoxLayout();
    hl->setSpacing(1);
    hl->setMargin(0);
    QLabel *image = new QLabel();
    image->setMaximumSize(QSize(h - 2, h - 2));
    image->setMinimumSize(QSize(h - 2, h - 2));
    QPixmap p(icon);
    image->setPixmap(p.scaled(image->width() - 1, image->height() - 1, Qt::KeepAspectRatio));
    hl->addWidget(image);
    QLabel *label = new QLabel(text);
    bool active = __c5config.getRegValue(QString("favorite-active-%1").arg(permission)).toBool();
    QToolButton *favorite = new QToolButton();
    favorite->setIcon(active ? QIcon(":/star-active.png") : QIcon(":/star-passive.png"));
    favorite->setMaximumSize(QSize(h - 2, h - 2));
    favorite->setMinimumSize(QSize(h - 2, h - 2));
    favorite->setProperty("cp", permission);
    favorite->setProperty("name", text);
    favorite->setProperty("active", active);
    connect(favorite, SIGNAL(clicked()), this, SLOT(on_btnFavoriteClicked()));
    hl->addWidget(favorite);
    hl->addWidget(label);
    hl->addStretch();
    w->setLayout(hl);


    l->addItem(item);
    l->setItemWidget(item, w);
}

void C5MainWindow::animateMenu(QListWidget *l, bool hide)
{
    QPropertyAnimation *p1 = new QPropertyAnimation(l, "minimumHeight", this);
    QPropertyAnimation *p2 = new QPropertyAnimation(l, "maximumHeight", this);
    if (hide) {
        p1->setStartValue(l->height());
        p1->setEndValue(0);
        p2->setStartValue(l->height());
        p2->setEndValue(0);
    } else {
        p1->setStartValue(0);
        p1->setEndValue(l->count() * ((qApp->font().pointSize() * 2) + 3));
        p2->setStartValue(0);
        p2->setEndValue(l->count() * ((qApp->font().pointSize() * 2) + 3));
    }
    p1->setDuration(200);
    p2->setDuration(200);
    QParallelAnimationGroup *ag = new QParallelAnimationGroup(this);
    ag->addAnimation(p1);
    ag->addAnimation(p2);
    ag->start();
}

QString C5MainWindow::itemIconName(int permission)
{
    for (QListWidget *l: fMenuLists) {
        for (int i = 0; i < l->count(); i++) {
            QListWidgetItem *item = l->item(i);
            if (item->data(Qt::UserRole).toInt() == permission) {
                return item->data(Qt::UserRole + 1).toString();
            }
        }
    }
    return ":/cancel.png";
}

void C5MainWindow::removeFromFavorite(int permission)
{
    for (int j = 1; j < fMenuLists.count(); j++) {
        QListWidget *l = fMenuLists.at(j);
        for (int i = 0; i < l->count(); i++) {
            QListWidgetItem *item = l->item(i);
            if (item->data(Qt::UserRole).toInt() == permission) {
                QObjectList ol = l->itemWidget(item)->children();
                for (QObject *o: ol) {
                    if (QToolButton *b = dynamic_cast<QToolButton*>(o)) {
                        b->setProperty("active", false);
                        b->setIcon(QIcon(":/star-passive.png"));
                        __c5config.setRegValue(QString("favorite-active-%1").arg(b->property("cp").toInt()), false);
                        __c5config.setRegValue(QString("favorite-active-%1-name").arg(b->property("cp").toInt()), b->property("name").toString());
                        return;
                    }
                }
            }
        }
    }
}

void C5MainWindow::autoLogin()
{
    int doctype = 0;
    int storein = 0;
    if (__autologin_store.count() > 0) {
        for (const QString &s: __autologin_store) {
            if (s.contains("--newdoc:")) {
                doctype = s.mid(s.indexOf(":") + 1, s.length() - s.indexOf(":")).toInt();
            }
            if (s.contains("--storein:")) {
                storein = s.mid(s.indexOf(":") + 1, s.length() - s.indexOf(":")).toInt();
            }
        }
    }
    __autologin_store.clear();
    QStringList dbParams = fDatabases[__c5config.getRegValue("lastdb").toString()];
    switch (doctype) {
    case DOC_TYPE_STORE_INPUT: {
        auto *doc = createTab<C5StoreDoc>(dbParams);
        doc->setMode(C5StoreDoc::sdInput);
        doc->setStore(storein, 0);
        break;
    }
    default:
        break;
    }
}

void C5MainWindow::on_listWidgetItemClicked(const QModelIndex &index)
{
    QListWidget *l = static_cast<QListWidget*>(sender());
    QStringList dbParams = fDatabases[__c5config.getRegValue("lastdb").toString()];
    QListWidgetItem *item = l->item(index.row());
    int permission = item->data(Qt::UserRole).toInt();
    switch (permission) {
    case cp_t1_usergroups:
        createTab<CR5UsersGroups>(dbParams);
        break;
    case cp_t1_users:
        createTab<CR5Users>(dbParams);
        break;
    case cp_t1_databases:
        createTab<CR5Databases>(dbParams);
        break;
    case cp_t1_settigns:
        createTab<CR5Settings>(dbParams);
        break;
    case cp_t1_breeze:
        createTab<CR5BreezeService>(dbParams);
        break;
    case cp_t2_store_input: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdInput);
        break;
    }
    case cp_t2_store_output: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdOutput);
        break;
    }
    case cp_t2_store_move: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdMovement);
        break;
    }
    case cp_t2_calculate_self_cost: {
        createTab<C5DishSelfCostGenPrice>(dbParams);
        break;
    }
    case cp_t2_store_inventory:
        createTab<C5StoreInventory>(dbParams);
        break;
    case cp_t2_count_output_of_sale:
        createTab<CR5ConsumptionBySales>(dbParams);
        break;
    case cp_t2_store_complectation: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdComplectation);
        break;
    }
    case cp_t2_store_decomplectation: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdDeComplectation);
        break;
    }
    case cp_t2_goods_reservations:
        createTab<CR5GoodsReservations>(dbParams);
        break;
    case cp_t2_reatail_trade: {
        auto *retaildoc = createTab<C5SaleDoc>(dbParams);
        retaildoc->setMode(1);
        break;
    }
    case cp_t2_whosale_trade: {
        auto *whosaledoc = createTab<C5SaleDoc>(dbParams);
        whosaledoc->setMode(2);
        break;
    }
    case cp_t3_sales_common:
        createTab<CR5CommonSales>(dbParams);
        break;
    case cp_t3_documents:
        createTab<CR5Documents>(dbParams);
        break;
    case cp_t3_documents_store:
        createTab<CR5StoreDocuments>(dbParams);
        break;
    case cp_t3_store:
        createTab<CR5MaterialsInStore>(dbParams);
        break;
    case cp_t3_store_movement:
        createTab<CR5GoodsMovement>(dbParams);
        break;
    case cp_t3_tstore_extra:
        createTab<CR5TStoreExtra>(dbParams);
        break;
    case cp_t3_store_sale:
        createTab<CR5SaleFromStore>(dbParams);
        break;
    case cp_t3_debts:
        createTab<CR5CostumerDebts>(dbParams);
        break;
    case cp_t3_car_visits:
        createTab<CR5CarVisits>(dbParams);
        break;
    case cp_t3_sale_removed_dishes:
        createTab<CR5SaleRemovedDishes>(dbParams);
        break;
    case cp_t3_sale_dishes:
        createTab<CR5SalesByDishes>(dbParams);
        break;
    case cp_t3_sale_from_store_total:
        createTab<CR5SaleFromStoreTotal>(dbParams);
        break;
    case cp_t3_discount_statistics:
        createTab<CR5DiscountStatisics>(dbParams);
        break;
    case cp_t3_consuption_reason:
        createTab<CR5ConsuptionReason>(dbParams);
        break;
    case cp_t3_preorders:
        createTab<CR5Preorders>(dbParams);
        break;
    case cp_t3_sale_effectiveness:
        createTab<CR5SalesEffectiveness>(dbParams);
        break;
    case cp_t3_storage_uncomplected:
        createTab<CR5MaterialInStoreUncomplect>(dbParams);
        break;
    case cp_t3_move_uncomplected:
        createTab<CR5MaterialMoveUncomplect>(dbParams);
        break;
    case cp_t3_count_output_of_sale_draft:
        createTab<CR5ConsumptionBySalesDraft>(dbParams);
        break;
    case cp_t3_custom_reports:
        createTab<CR5Custom>(dbParams);
        break;
    case cp_t3_draft_output_recipes:
        createTab<CR5DraftOutputByRecipe>(dbParams);
        break;
    case cp_t4_part1:
        createTab<CR5DishPart1>(dbParams);
        break;
    case cp_t4_part2:
        createTab<CR5DishPart2>(dbParams);
        break;
    case cp_t4_dishes:
        createTab<CR5Dish>(dbParams);
        break;
    case cp_t4_dishes_packages:
        createTab<CR5DishPackage>(dbParams);
        break;
    case cp_t4_menu_names:
        createTab<CR5MenuNames>(dbParams);
        break;
    case cp_t4_dish_remove_reason:
        createTab<CR5DishRemoveReason>(dbParams);
        break;
    case cp_t4_dish_comments:
        createTab<CR5DishComment>(dbParams);
        break;
    case cp_t4_dish_price_self_cost:
        createTab<CR5DishPriceSelfCost>(dbParams);
        break;
    case cp_t4_menu_review:
        createTab<CR5MenuReview>(dbParams);
        break;
    case cp_t6_units:
        createTab<CR5GoodsUnit>(dbParams);
        break;
    case cp_t6_groups:
        createTab<CR5GoodsGroup>(dbParams);
        break;
    case cp_t6_goods:
        createTab<CR5Goods>(dbParams);
        break;
    case cp_t6_goods_special_prices:
        createTab<C5GoodsSpecialPrices>(dbParams);
        break;
    case cp_t6_waste:
        createTab<CR5GoodsWaste>(dbParams);
        break;
    case cp_t6_storage:
        createTab<CR5GoodsStorages>(dbParams);
        break;
    case cp_t6_goods_images:
        createTab<CR5GoodsImages>(dbParams);
        break;
    case cp_t6_qty_reminder:
        createTab<CR5GoodsQtyReminder>(dbParams);
        break;
    case cp_t6_complectations:
        createTab<CR5Complectations>(dbParams);
        break;
    case cp_t7_partners:
        createTab<CR5GoodsPartners>(dbParams);
        break;
    case cp_t6_classes:
        createTab<CR5GoodsClasses>(dbParams);
        break;
    case cp_t7_credit_card:
        createTab<CR5CreditCards>(dbParams);
        break;
    case cp_t7_discount_system:
        createTab<CR5DiscountSystem>(dbParams);
        break;
    case cp_t7_halls:
        createTab<CR5Hall>(dbParams);
        break;
    case cp_t7_tables:
        createTab<CR5Tables>(dbParams);
        break;
    case cp_t7_translator:
        createTab<C5TranslatorForm>(dbParams);
        break;
    case cp_t7_store_reason:
        createTab<CR5StoreReason>(dbParams);
        break;
    case cp_t7_order_marks:
        createTab<CR5OrderMarks>(dbParams);
        break;
    case cp_t7_route:
        createTab<C5Route>(dbParams);
        break;
    case cp_t7_route_exec:
        createTab<CR5RouteDaily>(dbParams);
        break;
    case cp_t8_cash_names:
        createTab<CR5CashNames>(dbParams);
        break;
    case cp_t8_cash_doc:
        createTab<C5CashDoc>(dbParams);
        break;
    case cp_t8_cash_detailed_report:
        createTab<CR5CashDetailed>(dbParams);
        break;
    case cp_t8_cash_movement:
        createTab<CR5CashMovement>(dbParams);
        break;
    case cp_t8_currency:
        createTab<CR5Currencies>(dbParams);
        break;
    case cp_t8_edit_currency:
        createTab<CR5CurrencyRateHistory>(dbParams);
        break;
    case cp_t8_currency_cross_rate:
        createTab<CR5CurrencyCrossRate>(dbParams);
        break;
    case cp_t8_currency_cross_rate_history:
        createTab<CR5CurrencyCrossRateHistory>(dbParams);
        break;
    case cp_t9_salary_doc:
        createTab<C5SalaryDoc>(dbParams);
        break;
    case cp_t9_report:
        createTab<CR5SalaryByWorkers>(dbParams);
        break;
    case cp_t10_action_list:
        createTab<CR5MfActions>(dbParams);
        break;
    case cp_t10_daily:
        createTab<CR5MfDaily>(dbParams);
        break;
    case cp_t10_product_list:
        createTab<CR5MFProduct>(dbParams);
        break;
    case cp_t10_general_report:
        createTab<CR5MFGeneralReport>(dbParams);
        break;
    case cp_t10_general_report_only_date:
        createTab<CR5GeneralReportOnlyDate>(dbParams);
        break;
    case cp_t10_actions_stages:
        createTab<CR5MFActionStage>(dbParams);
        break;
    case cp_t10_workshops:
        createTab<CR5MFWorkshops>(dbParams);
        break;
    case cp_t10_active_tasks:
        createTab<CR5MFActiveTasks>(dbParams);
        break;
    default:
        if (permission < 0) {
            auto *reports = createTab<CR5Reports>(dbParams);
            reports->setReport(permission * -1);
            fTab->setTabText(fTab->count() - 1, reports->label());
        }
        break;
    }
}

void C5MainWindow::on_actionClose_application_triggered()
{
    if (C5Message::question(tr("Are you sure to close application?")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void C5MainWindow::on_actionLogout_triggered()
{
//    qDeleteAll(fMenuLists);
//    fMenuLists.clear();
    for (int i = ui->tabWidget->count() - 1; i > -1 ; i--) {
        QWidget *w = ui->tabWidget->widget(i);
        w->deleteLater();
        ui->tabWidget->removeTab(i);
    }
    QLayoutItem *li;
    while ((li = ui->lMenu->takeAt(0)) != nullptr) {
        li->widget()->deleteLater();
        delete li;
    }
    fMenuLists.clear();
    ui->actionLogin->setVisible(true);
    ui->actionLogout->setVisible(false);
    ui->actionChange_password->setVisible(false);
}

void C5MainWindow::on_actionChange_password_triggered()
{
    QString password;
    QStringList dbparams = fDatabases[__c5config.getRegValue("lastdb").toString()];
    if (!C5ChangePassword::changePassword(dbparams, password)) {
        return;
    }
    C5Message::info(tr("Password changed"));
}

void C5MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);
    C5Config::setRegValue("twdbsize", ui->wMenu->width());
}

bool C5MainWindow::addMainLevel(const QString &db, int permission, const QString &title, const QString &icon, QListWidget *&l)
{
    if (__user->check(permission)) {
        QPushButton *b = new QPushButton(QIcon(icon), title);
        b->setProperty("menu", "1");
        b->setProperty("cp", permission);
        connect(b, SIGNAL(clicked()), this, SLOT(on_btnMenuClick()));
        l = new QListWidget();
        l->setProperty("menu", "1");
        connect(l, SIGNAL(clicked(QModelIndex)), this, SLOT(on_listWidgetItemClicked(QModelIndex)));
        l->setProperty("db", db);
        l->setProperty("cp", permission);
        l->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        fMenuLists.append(l);
        b->setProperty("list", fMenuLists.count() - 1);
        ui->lMenu->addWidget(b);
        ui->lMenu->addWidget(l);
        return true;
    }
    return false;
}

void C5MainWindow::setDB(const QString &dbname)
{
    __c5config.setRegValue("lastdb", dbname);
    QStringList db = fDatabases[dbname];
    if (db.count() == 0) {
        C5Message::info(tr("No access to this database"));
        return;
    }
    QIcon icon = db.at(4).toInt() == 0 ? QIcon(":/database.png") : QIcon(":/database_main.png");
    qDeleteAll(fMenuLists);
    fMenuLists.clear();
    QLayoutItem *li;
    while ((li = ui->lMenu->takeAt(0)) != nullptr) {
        li->widget()->deleteLater();
        delete li;
    }
    QPushButton *btn = new QPushButton(icon, dbname);
    btn->setProperty("menu", "2");
    connect(btn, SIGNAL(clicked()), this, SLOT(on_btnChangeDB_clicked()));
    ui->lMenu->addWidget(btn);
    QListWidget *l = new QListWidget();
    connect(l, SIGNAL(clicked(QModelIndex)), this, SLOT(on_listWidgetItemClicked(QModelIndex)));
    fMenuLists.append(l);
    ui->lMenu->addWidget(l);

    C5Database dbpr(db.at(0),  db.at(1),  db.at(2),  db.at(3));

    if (addMainLevel(db.at(1), cp_t2_action, tr("Actions"), ":/edit.png", l)) {
        l->setProperty("reportlevel", 1);
        addTreeL3Item(l, cp_t2_store_input, tr("New store input"), ":/goods.png");
        addTreeL3Item(l, cp_t2_store_output, tr("New store output"), ":/goods.png");
        addTreeL3Item(l, cp_t2_store_move, tr("New store movement"), ":/goods.png");
        addTreeL3Item(l, cp_t2_store_complectation, tr("New store complecation"), ":/goods.png");
        addTreeL3Item(l, cp_t2_store_decomplectation, tr("New store decomplecation"), ":/goods.png");
        addTreeL3Item(l, cp_t2_store_inventory, tr("New store inventory"), ":/goods.png");
        if (__c5config.frontDeskMode() == FRONTDESK_WAITER) {
            addTreeL3Item(l, cp_t2_calculate_self_cost, tr("Calculate dishes self cost"), ":/menu.png");
        }
        addTreeL3Item(l, cp_t2_goods_reservations, tr("Goods reservations"), ":/calendar.png");
        addTreeL3Item(l, cp_t2_reatail_trade, tr("New retail traid"), ":/trading.png");
        addTreeL3Item(l, cp_t2_whosale_trade, tr("New whosale traid"), ":/trading.png");
    }

    if (addMainLevel(db.at(1), cp_t3_reports, tr("Reports"), ":/reports.png", l)) {
        l->setProperty("reportlevel", 2);
        addTreeL3Item(l, cp_t3_documents, tr("Documents"), ":/documents.png");
        addTreeL3Item(l, cp_t3_documents_store, tr("Documents in the store"), ":/documents.png");
        addTreeL3Item(l, cp_t3_store, tr("Storage"), ":/goods.png");
        addTreeL3Item(l, cp_t3_store_movement, tr("Storages movements"), ":/goods.png");
        addTreeL3Item(l, cp_t3_debts, tr("Debts"), ":/cash.png");
        addTreeL3Item(l, cp_t3_move_uncomplected, tr("Storage movement, uncomplected"), ":/goods.png");
        addTreeL3Item(l, cp_t3_storage_uncomplected, tr("Storage uncomplected"), ":/goods.png");
        addTreeL3Item(l, cp_t3_sale_from_store_total, tr("Detailed movement in the storage"), ":/graph.png");
        addTreeL3Item(l, cp_t3_tstore_extra, tr("T-account, extra"), ":/documents.png");
        addTreeL3Item(l, cp_t2_count_output_of_sale, tr("Consumption of goods based on sales"), ":/goods.png");
        addTreeL3Item(l, cp_t3_count_output_of_sale_draft, tr("Consumption of goods based on sales, draft"), ":/goods.png");
        addTreeL3Item(l, cp_t3_draft_output_recipes, tr("Draft output by recipes"), ":/goods.png");
        addTreeL3Item(l, cp_t3_consuption_reason, tr("Reason for consuption"), ":/goods.png");
        addTreeL3Item(l, cp_t3_sales_common, tr("Sales by tickets"), ":/graph.png");
        addTreeL3Item(l, cp_t3_sale_effectiveness, tr("Effectiveness of sales"), ":/effectiveness.png");
        if (__c5config.frontDeskMode() == FRONTDESK_WAITER) {
            addTreeL3Item(l, cp_t3_sale_dishes, tr("Sales, dishes"), ":/graph.png");
            addTreeL3Item(l, cp_t3_sale_removed_dishes, tr("Sales, removed dishes"), ":/delete.png");
        }
        if (__c5config.carMode()) {
            addTreeL3Item(l, cp_t3_car_visits, tr("Car visits"), ":/car.png");
        }
        addTreeL3Item(l, cp_t3_store_sale, tr("Sales by goods"), ":/graph.png");
        addTreeL3Item(l, cp_t3_discount_statistics, tr("Discount statistics"), ":/discount.png");
        addTreeL3Item(l, cp_t3_preorders, tr("Preorders"), ":/customers.png");
        addTreeL3Item(l, cp_t3_custom_reports, tr("Custom reports"), ":/constructor.png");
    }

    if (addMainLevel(db.at(1), cp_t8_cash, tr("Cash"), ":/reports.png", l)) {
        l->setProperty("reportlevel", 3);
        addTreeL3Item(l, cp_t8_cash_doc, tr("New cash document"), ":/cash.png");
        addTreeL3Item(l, cp_t8_cash_detailed_report, tr("Cash detailed report"), ":/cash.png");
        addTreeL3Item(l, cp_t8_cash_movement, tr("Movement in the cash"), ":/cash.png");
        addTreeL3Item(l, cp_t8_cash_names, tr("Cash names"), ":/cash.png");
        addTreeL3Item(l, cp_t8_currency, tr("Currency"), ":/cash.png");
        addTreeL3Item(l, cp_t8_edit_currency, tr("Currency rates"), ":/cash.png");
        addTreeL3Item(l, cp_t8_currency_cross_rate, tr("Currency cross rates"), ":/cash.png");
        addTreeL3Item(l, cp_t8_currency_cross_rate_history, tr("Currency cross rates history"), ":/cash.png");
    }

    if (addMainLevel(db.at(1), cp_t9_salary, tr("Salary"), ":/employee.png", l)) {
        l->setProperty("reportlevel", 4);
        addTreeL3Item(l, cp_t9_salary_doc, tr("New salary document"), ":/employee.png");
        addTreeL3Item(l, cp_t9_report, tr("Salary by workers"), ":/employee.png");
    }

    if (__c5config.frontDeskMode() == FRONTDESK_WAITER) {
        if (addMainLevel(db.at(1), cp_t4_menu, tr("Menu"), ":/menu.png", l)) {
            l->setProperty("reportlevel", 5);
            addTreeL3Item(l, cp_t4_part1, tr("Dish depts"), ":/menu.png");
            addTreeL3Item(l, cp_t4_part2, tr("Types of dishes"), ":/menu.png");
            addTreeL3Item(l, cp_t4_dishes, tr("Dishes list"), ":/menu.png");
            addTreeL3Item(l, cp_t4_dishes_packages, tr("Dishes packages"), ":/menu.png");
            addTreeL3Item(l, cp_t4_menu_names, tr("Menu names"), ":/menu.png");
            addTreeL3Item(l, cp_t4_dish_remove_reason, tr("Dish remove reasons"), ":/menu.png");
            addTreeL3Item(l, cp_t4_dish_comments, tr("Dish comments"), ":/menu.png");
            addTreeL3Item(l, cp_t4_dish_price_self_cost, tr("Dish self cost report"), ":/menu.png");
            addTreeL3Item(l, cp_t4_menu_review, tr("Review menu"), ":/menu.png");
        }
    }

    if (addMainLevel(db.at(1), cp_t6_goods_menu, tr("Goods"), ":/goods.png", l)) {
        l->setProperty("reportlevel", 6);
        addTreeL3Item(l, cp_t6_storage, tr("Storages"), ":/goods.png");
        addTreeL3Item(l, cp_t6_groups, tr("Groups of goods"), ":/goods.png");
        addTreeL3Item(l, cp_t6_goods, tr("Goods"), ":/goods.png");
        addTreeL3Item(l, cp_t6_waste, tr("Autowaste"), ":/goods.png");
        addTreeL3Item(l, cp_t6_units, tr("Units"), ":/goods.png");
        addTreeL3Item(l, cp_t6_classes, tr("Classes"), ":/goods.png");
        addTreeL3Item(l, cp_t6_goods_images, tr("Images"), ":/images.png");
        addTreeL3Item(l, cp_t6_qty_reminder, tr("Quantity reminder"), ":/goods.png");
        addTreeL3Item(l, cp_t6_complectations, tr("Complectations"), ":/goods.png");
        addTreeL3Item(l, cp_t6_goods_special_prices, tr("Special prices"), ":/goods.png");
    }

    if (addMainLevel(db.at(1), cp_t10_manufacture, tr("Manufacture"), ":/manufacturing.png", l)) {
        l->setProperty("reportlevel", 7);
        addTreeL3Item(l, cp_t10_active_tasks, tr("Active tasks"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_workshops, tr("Workshops"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_actions_stages, tr("Action stages"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_product_list, tr("Products"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_action_list, tr("Action list"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_daily, tr("Daily processes"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_general_report, tr("General report"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_general_report_only_date, tr("General report only date"), ":/manufacturing.png");
        addTreeL3Item(l, cp_t10_actions_stages, tr("Action stages"), ":/manufacturing.png");
    }

    if (addMainLevel(db.at(1), cp_t7_other, tr("Other"), ":/other.png", l)) {
        l->setProperty("reportlevel", 8);
        addTreeL3Item(l, cp_t7_partners, tr("Partners"), ":/partners.png");
        addTreeL3Item(l, cp_t7_halls, tr("Halls"), ":/hall.png");
        addTreeL3Item(l, cp_t7_tables, tr("Tables"), ":/table.png");
        addTreeL3Item(l, cp_t7_credit_card, tr("Credit cards"), ":/credit-card.png");
        addTreeL3Item(l, cp_t7_discount_system, tr("Discount system"), ":/discount.png");
        addTreeL3Item(l, cp_t7_store_reason, tr("Store reason"), ":/documents.png");
        addTreeL3Item(l, cp_t7_order_marks, tr("Order marks"), ":/flag.png");
        addTreeL3Item(l, cp_t7_route, tr("Route"), ":/route.png");
        addTreeL3Item(l, cp_t7_route_exec, tr("Route report"), ":/route.png");
    }

    if (addMainLevel(db.at(1), cp_t1_preference, tr("Preferences"), ":/configure.png", l)) {
        l->setProperty("reportlevel", 9);
        addTreeL3Item(l, cp_t1_usergroups, tr("Positions of employees"), ":/users_groups.png");
        addTreeL3Item(l, cp_t1_users, tr("Employees"), ":/users_groups.png");
        addTreeL3Item(l, cp_t1_databases, tr("Databases"), ":/database.png");
        addTreeL3Item(l, cp_t1_settigns, tr("Settings"), ":/configure.png");
        addTreeL3Item(l, cp_t7_translator, tr("Translator"), ":/translate.png");
        addTreeL3Item(l, cp_t1_breeze, tr("Breeze service"), ":/configure.png");
    }

    C5Database dbb(db.at(0),  db.at(1),  db.at(2),  db.at(3));
    dbb[":f_group"] = __user->group();
    dbb.exec("select r.f_id, rg.f_level, r.f_name as f_reportname "
            "from reports r "
            "inner join reports_group rg on rg.f_id=r.f_group "
            "inner join reports_permissions ra on ra.f_report=r.f_id "
            "where ra.f_access=1 and ra.f_group=:f_group ");
    while (dbb.nextRow()) {
        int reportlevel = dbb.getInt("f_level");
        for (QListWidget *lw: qAsConst(fMenuLists)) {
            if (lw->property("reportlevel").toInt() == reportlevel) {
                addTreeL3Item(lw, -1 * dbb.getInt("f_id"), dbb.getString("f_reportname"), ":/documents.png");
            }
        }
    }

    readFavoriteMenu();
    for (QListWidget *ll: fMenuLists) {
        int size = ll->count() == 0 ? 0 : (ll->count() * (ll->item(0)->sizeHint().height() + 1));
        ll->setMinimumHeight(size);
        ll->setMaximumHeight(ll->minimumHeight());
        ll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        animateMenu(ll, __c5config.getRegValue(QString("btnmenushow-%1").arg(ll->property("cp").toInt()), false).toBool());
    }
    ui->lMenu->addStretch(1);
}

void C5MainWindow::on_btnHideMenu_clicked()
{
    ui->wMenu->setVisible(!ui->wMenu->isVisible());
    ui->btnHideMenu->setText(ui->wMenu->isVisible() ? "<" : ">");
    C5Config::setRegValue("menupanel", ui->wMenu->isVisible());
}

void C5MainWindow::on_btnChangeDB_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(mainDbParams, cache_s_db, values)) {
        return;
    }
    setDB(values.at(2).toString());
}

void C5MainWindow::on_btnMenuClick()
{
    QPushButton *b = static_cast<QPushButton*>(sender());
    int i = b->property("list").toInt();
    QListWidget *l = fMenuLists.at(i);
    bool hide = l->minimumHeight() > 0;
    __c5config.setRegValue("btnmenushow-" + b->property("cp").toString(), hide);
    if (hide) {
        animateMenu(l, true);
    } else {
        animateMenu(l, false);
    }
}

void C5MainWindow::on_btnFavoriteClicked()
{
    QToolButton *b = static_cast<QToolButton*>(sender());
    bool active = !b->property("active").toBool();
    if (C5Message::question(active ? tr("Add to favorites?") : tr("Remove from favorites?")) != QDialog::Accepted){
        return;
    }
    b->setProperty("active", active);
    b->setIcon(active ? QIcon(":/star-active.png") : QIcon(":/star-passive.png"));
    __c5config.setRegValue(QString("favorite-active-%1").arg(b->property("cp").toInt()), active);
    __c5config.setRegValue(QString("favorite-active-%1-name").arg(b->property("cp").toInt()), b->property("name").toString());
    readFavoriteMenu();
    if (!active) {
        removeFromFavorite(b->property("cp").toInt());
    }
}

void C5MainWindow::readFavoriteMenu()
{
    if (fMenuLists.count() == 0) {
        return;
    }
    QListWidget *l = fMenuLists.at(0);
    l->setProperty("menu", "1");
    l->clear();
    QSettings ss(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    QStringList keys = ss.allKeys();
    for (const QString &s: keys) {
        if (!s.contains("favorite") || s.contains("-name")) {
            continue;
        }
        qDebug() << s;
        if (ss.value(s).toBool()) {
            int prm = s.right(3).toInt();
            if (__user->check(prm)) {
                addTreeL3Item(l, prm, ss.value(s + "-name").toString(), itemIconName(prm));
            }
        } else {
            ss.remove(s);
            ss.remove(s + "-name");
        }
    }
    int size = l->count() == 0 ? 0 : (l->count() * (l->item(0)->sizeHint().height() + 1));
    l->setMinimumHeight(size);
    l->setMaximumHeight(l->minimumHeight());
}
