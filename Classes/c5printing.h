#ifndef C5PRINTING_H
#define C5PRINTING_H

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPrinter>

class C5Printing : public QObject
{
    Q_OBJECT
public:
    explicit C5Printing();
    virtual ~C5Printing();

    void setSceneParams(qreal width, qreal height, qreal logicalDpiX);
    void reset();

    // Настройки текста
    void setFont(const QFont &font);
    void setFontBold(bool bold);
    void setFontSize(int size);

    // Рисование
    void line(int lineWidth = 1);
    void ltext(const QString &text, qreal x = 0, qreal textWidth = 0);
    void ctext(const QString &text);
    void rtext(const QString text);
    void lrtext(const QString &leftText, const QString &rightText, qreal textWidth = 0);
    void image(const QPixmap &img, Qt::Alignment align = Qt::AlignLeft);

    bool br(qreal height = 0);
    bool print(QPrinter &prn);
    int fTop;
    int fLineHeight;

    QJsonArray jsonData() const;

    void addToJson(const QString &type, const QVariantMap &params);

    QImage resultImage() const;

    void image(const QString &fileName, Qt::Alignment align);

private:
    QImage fImage;
    QPainter fPainter;

    int fTempTop;
    int fNormalWidth;

    qreal fMM;
    qreal fLogicalDpiX;

    QFont fFont;
    QPen fLinePen;
    QJsonArray fJsonData;

    void updateLineHeight();
};

#endif // C5PRINTING_H
