#include "dlgstart.h"

#include "appsettings.h"
#include "dlgmenu.h"
#include "selfboardlanguage.h"
#include "dlgserversettings.h"
#include "menucache.h"
#include "selfboarddisplay.h"
#include "serverconfig.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCursor>
#include <QEvent>
#include <QScreen>
#include <QShortcut>
#include <QFile>
#include <QResizeEvent>
#include <QShowEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QPixmap>
#include <QPushButton>
#include "selfboardsettings.h"

#include <QSettings>
#include <QTimer>
#include <QToolButton>

#include "ui_dlgstart.h"

namespace {

constexpr int kLangRu = 0;
constexpr int kLangEn = 1;
constexpr int kLangHy = 2;

QString localeCodeForButton(int id)
{
    switch (id) {
    case kLangRu:
        return QStringLiteral("ru");
    case kLangHy:
        return QStringLiteral("hy");
    case kLangEn:
    default:
        return QStringLiteral("en");
    }
}

} // namespace

DlgStart::DlgStart(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStart)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setFixedSize(1080, 1920);

    setupAppearance();
    setupBackground();
    setupIcons();
    setupLanguageGroup();

    SelfboardLanguage::instance().loadSavedLocale();
    const QString savedLocale = SelfboardLanguage::instance().currentLocale();
    int langId = kLangEn;
    if (savedLocale == QStringLiteral("ru")) {
        langId = kLangRu;
    } else if (savedLocale == QStringLiteral("hy")) {
        langId = kLangHy;
    }
    if (QAbstractButton *btn = m_langGroup->button(langId)) {
        btn->setChecked(true);
    }

    auto *settingsShortcut = new QShortcut(QKeySequence(Qt::Key_F3), this);
    settingsShortcut->setContext(Qt::ApplicationShortcut);
    connect(settingsShortcut, &QShortcut::activated, this, [this]() { openServerSettings(false); });
}

DlgStart::~DlgStart()
{
    delete ui;
}

void DlgStart::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (!m_startupDone) {
        m_startupDone = true;
        QTimer::singleShot(0, this, [this]() { runStartup(); });
    }
}

void DlgStart::runStartup()
{
    if (!ServerConfig::isConfigured()) {
        openServerSettings(true);
        return;
    }
    startMenuPreload();
}

void DlgStart::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateLanguageButtonIcons();
    }
    QDialog::changeEvent(event);
}

void DlgStart::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F4 && (event->modifiers() & Qt::AltModifier)) {
        close();
        event->accept();
        return;
    }
    QDialog::keyPressEvent(event);
}

void DlgStart::setupAppearance()
{
    setAutoFillBackground(false);

    QFile styleFile(QStringLiteral(":/res/selfboard.css"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    ui->widgetHeader->setAutoFillBackground(false);
}

void DlgStart::setupBackground()
{
    if (!m_background) {
        m_background = new QLabel(this);
        m_background->setObjectName(QStringLiteral("lblBackground"));
        m_background->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_background->setScaledContents(true);
        m_background->lower();
    }

    const QPixmap src(QStringLiteral(":/res/background_food.png"));
    if (!src.isNull()) {
        m_background->setPixmap(src.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
    m_background->setGeometry(rect());
    m_background->lower();
}

void DlgStart::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    setupBackground();
    if (m_loadingOverlay && m_loadingOverlay->isVisible()) {
        updateLoadingOverlayGeometry();
    }
}

void DlgStart::updateLoadingOverlayGeometry()
{
    if (!m_loadingOverlay) {
        return;
    }
    int headerBottom =
        ui->widgetHeader->mapTo(this, QPoint(0, ui->widgetHeader->height())).y();
    if (headerBottom <= 0) {
        headerBottom = ui->widgetHeader->minimumHeight() + 40;
    }
    m_loadingOverlay->setGeometry(0, headerBottom, width(), qMax(0, height() - headerBottom));
}

void DlgStart::setupIcons()
{
    ui->lblLogo->setPixmap(QPixmap(QStringLiteral(":/res/main_logo.png")));
    ui->btnTakeAway->setIcon(QIcon(QStringLiteral(":/res/icon_takeaway.png")));
    ui->btnDineIn->setIcon(QIcon(QStringLiteral(":/res/icon_dinein.png")));
    ui->lblQrCode->setPixmap(QPixmap(QStringLiteral(":/res/qr_placeholder.png")));
    ui->lblFooterLogo->setPixmap(QPixmap(QStringLiteral(":/res/footer_logo_myqr.png")));
    ui->btnYoutube->setIcon(QIcon(QStringLiteral(":/res/icon_youtube.png")));
    ui->btnFacebook->setIcon(QIcon(QStringLiteral(":/res/icon_facebook.png")));

    updateLanguageButtonIcons();
}

void DlgStart::setupLanguageGroup()
{
    m_langGroup = new QButtonGroup(this);
    m_langGroup->setExclusive(true);
    m_langGroup->addButton(ui->btnLangRu, kLangRu);
    m_langGroup->addButton(ui->btnLangEn, kLangEn);
    m_langGroup->addButton(ui->btnLangHy, kLangHy);

    connect(m_langGroup, &QButtonGroup::idClicked, this, &DlgStart::onLanguageClicked);
    connect(ui->btnTakeAway, &QToolButton::clicked, this, &DlgStart::onTakeAwayClicked);
    connect(ui->btnDineIn, &QToolButton::clicked, this, &DlgStart::onDineInClicked);
}

void DlgStart::updateLanguageButtonIcons()
{
    const QSize iconSize(36, 36);

    auto setLangIcon = [&](QPushButton *btn, const char *path) {
        btn->setIcon(QIcon(QString::fromUtf8(path)));
        btn->setIconSize(iconSize);
    };

    setLangIcon(ui->btnLangRu, ":/res/flag_ru.png");
    setLangIcon(ui->btnLangEn, ":/res/flag_en.png");
    setLangIcon(ui->btnLangHy, ":/res/flag_am.png");
}

void DlgStart::applyLanguage(const QString &localeCode)
{
    SelfboardLanguage::instance().applyLocale(localeCode);
    m_currentLocale = SelfboardLanguage::instance().currentLocale();

    int langId = kLangEn;
    if (m_currentLocale == QStringLiteral("ru")) {
        langId = kLangRu;
    } else if (m_currentLocale == QStringLiteral("hy")) {
        langId = kLangHy;
    }
    if (QAbstractButton *btn = m_langGroup->button(langId)) {
        btn->setChecked(true);
    }
}

void DlgStart::onLanguageClicked(int id)
{
    applyLanguage(localeCodeForButton(id));
}

void DlgStart::setMenuLoadingUi(bool loading, const QString &message)
{
    if (!m_loadingOverlay) {
        m_loadingOverlay = new QLabel(this);
        m_loadingOverlay->setAlignment(Qt::AlignCenter);
        m_loadingOverlay->setStyleSheet(
            QStringLiteral("background-color: rgba(0,0,0,160); color: white; font-size: 28px; padding: 24px;"));
        m_loadingOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }

    if (loading) {
        m_loadingOverlay->setText(message.isEmpty() ? tr("Loading menu...") : message);
        updateLoadingOverlayGeometry();
        m_loadingOverlay->raise();
        m_loadingOverlay->show();
        ui->btnTakeAway->setEnabled(false);
        ui->btnDineIn->setEnabled(false);
    } else {
        m_loadingOverlay->hide();
        ui->btnTakeAway->setEnabled(m_menuReady);
        ui->btnDineIn->setEnabled(m_menuReady);
    }
}

void DlgStart::openServerSettings(bool required)
{
    const bool hadOverlay = m_loadingOverlay && m_loadingOverlay->isVisible();
    const QString overlayMessage = hadOverlay ? m_loadingOverlay->text() : QString();

    if (m_loadingOverlay) {
        m_loadingOverlay->hide();
    }

    DlgServerSettings dlg(nullptr);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setWindowFlag(Qt::Window, true);
    dlg.setWindowFlag(Qt::WindowStaysOnTopHint, true);
    dlg.adjustSize();
    if (QScreen *screen = QApplication::screenAt(QCursor::pos())) {
        const QRect area = screen->availableGeometry();
        dlg.move(area.center() - QPoint(dlg.width() / 2, dlg.height() / 2));
    }
    const int result = dlg.exec();

    AppSettings::loadFromSettings();

    if (result == QDialog::Accepted && dlg.sessionObtained()) {
        startMenuPreload();
        return;
    }

    if (hadOverlay) {
        setMenuLoadingUi(true, overlayMessage);
    } else if (required && !ServerConfig::isConfigured()) {
        setMenuLoadingUi(true, tr("Server settings are required to load the menu."));
    }
}

void DlgStart::startMenuPreload()
{
    m_menuReady = false;
    setMenuLoadingUi(true, tr("Loading menu..."));
    MenuCache::instance().preload(this, [this](bool ok) {
        m_menuReady = ok;
        if (!ok) {
            const QString err = MenuCache::instance().lastError();
            setMenuLoadingUi(
                true,
                tr("Menu load failed") + QStringLiteral("\n") + err + QStringLiteral("\n")
                    + tr("Press F3 to open server settings."));
            return;
        }
        setMenuLoadingUi(false);
    });
}

void DlgStart::onTakeAwayClicked()
{
    if (!m_menuReady) {
        return;
    }
    openMenu(DlgMenu::ServiceMode::TakeAway);
}

void DlgStart::onDineInClicked()
{
    if (!m_menuReady) {
        return;
    }
    openMenu(DlgMenu::ServiceMode::DineIn);
}

void DlgStart::openMenu(DlgMenu::ServiceMode mode)
{
    QWidget *host = window();

    auto *menu = new DlgMenu(mode, nullptr);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    connect(menu, &QDialog::finished, this, [host]() {
        host->showFullScreen();
        host->raise();
        host->activateWindow();
    });

    host->hide();
    SelfBoardDisplay::showFullscreen(menu);
}
