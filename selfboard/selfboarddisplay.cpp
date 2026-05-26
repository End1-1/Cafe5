#include "selfboarddisplay.h"

#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScreen>
#include <QVBoxLayout>

namespace {

class RotatedShell : public QWidget
{
public:
    explicit RotatedShell(QWidget *content, QWidget *parent = nullptr)
        : QWidget(parent)
        , m_content(content)
    {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

        m_scene = new QGraphicsScene(this);
        m_view = new QGraphicsView(m_scene, this);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setFrameShape(QFrame::NoFrame);
        m_view->setRenderHint(QPainter::SmoothPixmapTransform);
        m_view->setAlignment(Qt::AlignCenter);
        m_view->setStyleSheet(QStringLiteral("background-color: #000000; border: none;"));

        m_content->setWindowFlags(Qt::Widget);
        m_proxy = m_scene->addWidget(m_content);

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_view);

        connect(m_content, &QObject::destroyed, this, &QWidget::close);
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        layoutRotated();
    }

private:
    void layoutRotated()
    {
        const QRect viewRect = m_view->viewport()->rect();
        m_scene->setSceneRect(QRectF(viewRect));

        const qreal cw = m_content->width();
        const qreal ch = m_content->height();

        m_proxy->setTransformOriginPoint(0, 0);
        m_proxy->setRotation(90);
        const qreal x = (viewRect.width() - ch) / 2.0;
        const qreal y = (viewRect.height() - cw) / 2.0;
        m_proxy->setPos(x + ch, y);
    }

    QWidget *m_content = nullptr;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    QGraphicsProxyWidget *m_proxy = nullptr;
};

} // namespace

namespace SelfBoardDisplay {

bool shouldRotate(const QScreen *screen)
{
    if (qEnvironmentVariableIsSet("SELFBOARD_ROTATE")) {
        return qEnvironmentVariableIntValue("SELFBOARD_ROTATE") != 0;
    }

    const QScreen *scr = screen ? screen : QApplication::primaryScreen();
    if (!scr) {
        return false;
    }

    return scr->geometry().width() > scr->geometry().height();
}

void showFullscreen(QWidget *content)
{
    if (!content) {
        return;
    }

    QScreen *screen = QApplication::screenAt(content->pos());
    if (!screen) {
        screen = QApplication::primaryScreen();
    }

    if (shouldRotate(screen)) {
        auto *shell = new RotatedShell(content);
        shell->showFullScreen();
        return;
    }

    content->showFullScreen();
}

} // namespace SelfBoardDisplay
