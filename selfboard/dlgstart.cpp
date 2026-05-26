#include "dlgstart.h"

#include "dlgmenu.h"
#include "selfboarddisplay.h"

#include <QButtonGroup>
#include <QEvent>
#include <QFile>
#include <QResizeEvent>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
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

    QSettings settings(QStringLiteral("Jazzve"), QStringLiteral("SelfBoard"));
    const QString savedLocale = settings.value(QStringLiteral("locale"), QStringLiteral("en")).toString();
    int langId = kLangEn;
    if (savedLocale == QStringLiteral("ru")) {
        langId = kLangRu;
    } else if (savedLocale == QStringLiteral("hy")) {
        langId = kLangHy;
    }
    if (QAbstractButton *btn = m_langGroup->button(langId)) {
        btn->setChecked(true);
    }
    applyLanguage(savedLocale);
}

DlgStart::~DlgStart()
{
    delete ui;
}

void DlgStart::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateLanguageButtonIcons();
    }
    QDialog::changeEvent(event);
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
    const QString code = localeCode.isEmpty() ? QStringLiteral("en") : localeCode;
    if (code == m_currentLocale) {
        return;
    }

    qApp->removeTranslator(&m_translator);

    const QString qmPath = QStringLiteral(":/lang/SelfBoard_%1.qm").arg(code);
    if (m_translator.load(qmPath)) {
        qApp->installTranslator(&m_translator);
        m_currentLocale = code;
        ui->retranslateUi(this);
        updateLanguageButtonIcons();

        QSettings settings(QStringLiteral("Jazzve"), QStringLiteral("SelfBoard"));
        settings.setValue(QStringLiteral("locale"), code);
    }
}

void DlgStart::onLanguageClicked(int id)
{
    applyLanguage(localeCodeForButton(id));
}

void DlgStart::onTakeAwayClicked()
{
    openMenu(DlgMenu::ServiceMode::TakeAway);
}

void DlgStart::onDineInClicked()
{
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
