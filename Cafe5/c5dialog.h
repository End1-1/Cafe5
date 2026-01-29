#pragma once

#include <QDialog>
#include <QJsonArray>

class NInterface;
class C5User;

class C5Dialog : public QDialog
{
    Q_OBJECT

public:
    C5Dialog(C5User *user);

    ~C5Dialog();

    static void setMainWindow(QWidget *widget);

    virtual bool preambule();

    template<typename T>
    static bool go()
    {
        T *t = new T();
        bool result = false;

        if(t->preambule()) {
            result = t->exec() == QDialog::Accepted;
        }

        delete t;
        return result;
    }

    virtual void selectorCallback(int row, const QJsonArray &values);

    void setFieldValue(const QString &name, const QString &value);

    void setFieldValue(const QString &name, double value);

    void setFieldValue(const QString &name, int value);

    int getFieldIntValue(const QString &name);

    double getFieldDoubleValue(const QString &name);

    QString getFieldStringValue(const QString &name);

    static int mScreen;

    C5User* mUser;

    QPoint mInitialPos = QPoint(-1, -1);

public slots:
    void updateRequired(const QString &msg, const QString &appName, const QString &newVersion);

protected:
    virtual void resizeEvent(QResizeEvent *e) override;

    virtual void moveEvent(QMoveEvent *e) override;

    virtual void keyPressEvent(QKeyEvent *e) override;

    virtual void keyEnter();

    virtual void keyControlPlusEnter();

    virtual void keyAlterPlusEnter();

    virtual void showEvent(QShowEvent *e) override;

    virtual void paintEvent(QPaintEvent *e) override;

    NInterface* fHttp;

protected slots:
    void handleError(int err, const QString &msg);

private:
    QVector<QColor> mTopBorder;

    QVector<QColor> mBottomBorder;

    QVector<QColor> mLeftBorder;

    QVector<QColor> mRightBorder;

    QImage mBgCache;

    void updateBackgroundCache();

    QImage grabDesktopArea(const QRect &r);

    void buildAdaptiveBorder();

    QColor contrast(const QColor &c);
signals:
    void escape();
};
