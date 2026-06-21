#include "selfboardlanguage.h"

#include "selfboardsettings.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QCoreApplication>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QTranslator>

#include <functional>

namespace {

constexpr int kPickerHeight = 60;
constexpr int kFlagSize = 36;

QPixmap circularFlagPixmap(const QString &path, int size)
{
    const QPixmap source(path);
    if (source.isNull()) {
        return {};
    }

    const QPixmap scaled = source.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap rounded(size, size);
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath clip;
    clip.addEllipse(0, 0, size, size);
    painter.setClipPath(clip);
    painter.drawPixmap(0, 0, scaled);
    return rounded;
}

class LanguagePickerButton : public QFrame
{
public:
    explicit LanguagePickerButton(QWidget *parent = nullptr)
        : QFrame(parent)
    {
        setObjectName(QStringLiteral("btnLanguage"));
        setFixedHeight(kPickerHeight);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setCursor(Qt::PointingHandCursor);
        setAttribute(Qt::WA_StyledBackground, true);

        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(14, 10, 16, 10);
        layout->setSpacing(12);

        m_flag = new QLabel(this);
        m_flag->setObjectName(QStringLiteral("lblLanguageFlag"));
        m_flag->setFixedSize(kFlagSize, kFlagSize);
        m_flag->setScaledContents(true);

        m_label = new QLabel(this);
        m_label->setObjectName(QStringLiteral("lblLanguageName"));

        m_arrow = new QLabel(QString::fromUtf8("\u25BC"), this);
        m_arrow->setObjectName(QStringLiteral("lblLanguageArrow"));
        m_arrow->setAlignment(Qt::AlignCenter);

        layout->addWidget(m_flag, 0, Qt::AlignVCenter);
        layout->addWidget(m_label, 1, Qt::AlignVCenter);
        layout->addWidget(m_arrow, 0, Qt::AlignVCenter);

        for (QLabel *part : {m_flag, m_label, m_arrow}) {
            part->installEventFilter(this);
        }
    }

    void setOnClicked(std::function<void()> handler)
    {
        m_onClicked = std::move(handler);
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (event->type() == QEvent::MouseButtonPress) {
            const auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                triggerClick();
                return true;
            }
        }
        return QFrame::eventFilter(watched, event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            triggerClick();
            event->accept();
            return;
        }
        QFrame::mousePressEvent(event);
    }

private:
    void triggerClick()
    {
        if (m_onClicked) {
            m_onClicked();
        }
    }

    QLabel *m_flag = nullptr;
    QLabel *m_label = nullptr;
    QLabel *m_arrow = nullptr;
    std::function<void()> m_onClicked;
};

} // namespace

SelfboardLanguage &SelfboardLanguage::instance()
{
    static SelfboardLanguage self;
    return self;
}

SelfboardLanguage::SelfboardLanguage(QObject *parent)
    : QObject(parent)
    , m_translator(new QTranslator(this))
{
    Q_UNUSED(parent);
}

QString SelfboardLanguage::currentLocale() const
{
    return m_currentLocale;
}

QString SelfboardLanguage::displayName(const QString &locale) const
{
    if (locale == QStringLiteral("ru")) {
        return tr("Русский");
    }
    if (locale == QStringLiteral("hy")) {
        return tr("Հայերեն");
    }
    return tr("English");
}

QString SelfboardLanguage::flagIconPath(const QString &locale) const
{
    if (locale == QStringLiteral("ru")) {
        return QStringLiteral(":/res/flag_ru.png");
    }
    if (locale == QStringLiteral("hy")) {
        return QStringLiteral(":/res/flag_am.png");
    }
    return QStringLiteral(":/res/flag_en.png");
}

void SelfboardLanguage::loadSavedLocale()
{
    QSettings settings = SelfBoardSettings::store();
    applyLocale(settings.value(QStringLiteral("locale"), QStringLiteral("en")).toString());
}

void SelfboardLanguage::applyLocale(const QString &code)
{
    const QString locale = code.isEmpty() ? QStringLiteral("en") : code;
    if (locale == m_currentLocale && m_localeLoaded) {
        return;
    }

    qApp->removeTranslator(m_translator);

    const QString qmPath = QStringLiteral(":/lang/SelfBoard_%1.qm").arg(locale);
    if (!m_translator->load(qmPath)) {
        return;
    }

    qApp->installTranslator(m_translator);
    m_currentLocale = locale;
    m_localeLoaded = true;

    QSettings settings = SelfBoardSettings::store();
    settings.setValue(QStringLiteral("locale"), locale);
    SelfBoardSettings::flush();

    const QList<QWidget *> widgets = qApp->topLevelWidgets();
    for (QWidget *widget : widgets) {
        const auto notifyTree = [](QWidget *root, const auto &notifyRef) -> void {
            if (!root) {
                return;
            }
            QEvent languageEvent(QEvent::LanguageChange);
            QCoreApplication::sendEvent(root, &languageEvent);
            for (QObject *child : root->children()) {
                if (auto *childWidget = qobject_cast<QWidget *>(child)) {
                    notifyRef(childWidget, notifyRef);
                }
            }
        };
        notifyTree(widget, notifyTree);
    }

    refreshPickerButtons();
    emit localeChanged(locale);
}

void SelfboardLanguage::bindPickerButton(QPushButton *button)
{
    if (!button) {
        return;
    }

    QWidget *parent = button->parentWidget();
    QLayout *parentLayout = parent ? parent->layout() : nullptr;
    if (!parentLayout) {
        return;
    }

    int index = -1;
    for (int i = 0; i < parentLayout->count(); ++i) {
        QLayoutItem *item = parentLayout->itemAt(i);
        if (item && item->widget() == button) {
            index = i;
            break;
        }
    }

    parentLayout->removeWidget(button);
    button->hide();
    button->setFixedSize(0, 0);
    // Keep ui->btnLanguage alive: retranslateUi() still references this pointer.

    auto *picker = new LanguagePickerButton(parent);
    const QPointer<LanguagePickerButton> pickerGuard(picker);
    picker->setOnClicked([this, pickerGuard]() {
        if (pickerGuard) {
            showPickerMenu(pickerGuard);
        }
    });

    if (auto *box = qobject_cast<QBoxLayout *>(parentLayout)) {
        if (index >= 0) {
            box->insertWidget(index, picker);
        } else {
            box->addWidget(picker);
        }
    } else {
        parentLayout->addWidget(picker);
    }

    m_pickers.append(picker);
    refreshPickerButtons();
}

void SelfboardLanguage::refreshPickerButtons()
{
    m_pickers.removeAll(nullptr);

    const QString flagPath = flagIconPath(m_currentLocale);
    const QString name = displayName(m_currentLocale);

    for (QWidget *button : m_pickers) {
        if (!button) {
            continue;
        }
        auto *flag = button->findChild<QLabel *>(QStringLiteral("lblLanguageFlag"));
        auto *label = button->findChild<QLabel *>(QStringLiteral("lblLanguageName"));
        if (!flag || !label) {
            continue;
        }
        flag->setPixmap(circularFlagPixmap(flagPath, kFlagSize));
        label->setText(name);
    }
}

void SelfboardLanguage::showPickerMenu(QWidget *button)
{
    if (!button) {
        return;
    }

    QMenu menu(button);
    menu.setObjectName(QStringLiteral("menuLanguage"));

    const auto addItem = [&](const QString &locale) {
        QAction *action = menu.addAction(QIcon(flagIconPath(locale)), displayName(locale));
        action->setData(locale);
        if (locale == m_currentLocale) {
            action->setCheckable(true);
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, action]() {
            applyLocale(action->data().toString());
        });
    };

    addItem(QStringLiteral("en"));
    addItem(QStringLiteral("ru"));
    addItem(QStringLiteral("hy"));

    const QPoint pos = button->mapToGlobal(QPoint(0, button->height() + 6));
    menu.setFixedWidth(button->width());
    menu.exec(pos);
}
