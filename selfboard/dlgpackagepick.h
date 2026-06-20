#ifndef DLGPACKAGEPICK_H
#define DLGPACKAGEPICK_H

#include "menutypes.h"

#include <QHash>
#include <QPair>
#include <QVector>
#include <QWidget>

class QMouseEvent;
class QAbstractButton;
class QButtonGroup;
class QCheckBox;
class QFrame;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QVBoxLayout;

class DlgPackagePick : public QWidget
{
    Q_OBJECT

public:
    explicit DlgPackagePick(const MenuDish &package, QWidget *parent = nullptr);

signals:
    void accepted(const MenuDish &line, int qty, const QVector<QPair<MenuDish, int>> &extraLines);
    void rejected();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onAttributeChanged();
    void decreaseQty();
    void increaseQty();
    void onAddClicked();
    void onCloseClicked();
    void onGoToMenuClicked();
    void onBackClicked();
    void onNextClicked();

private:
    enum class PickStep {
        Personalize = 0,
        Drink = 1,
        Extra = 2,
    };

    struct RelatedPickState
    {
        MenuDish dish;
        int qty = 0;
        QLabel *qtyLabel = nullptr;
    };

    void buildUi();
    void buildHeader();
    void buildFooter();
    void buildPersonalizePage();
    QWidget *buildRelatedPickPage(const QString &title, QVector<RelatedPickState> &picks);
    void buildDrinkPage();
    void buildExtraPage();
    void initRelatedPicks();
    void buildAttributeGroups();
    void buildModificatorGroups();
    void showPackageBadges();
    void showPackageBju();
    void updatePreview();
    void updateOptionPrices();
    void updateStepUi();
    void updateFooter();
    void setStep(PickStep step);
    PickStep nextStepAfter(PickStep step) const;
    PickStep previousStepBefore(PickStep step) const;
    void selectInitialSelections();
    QVector<MenuSelectedModificator> selectedModificators() const;
    QVector<QPair<MenuDish, int>> selectedExtraLines() const;
    bool requiredModificatorsSatisfied() const;
    double selectedModificatorsPrice() const;
    double relatedSelectionTotal() const;
    double currentUnitPrice() const;
    bool attributeSelectionValid() const;
    bool canProceedFromCurrentStep() const;
    QString selectedValueForKey(const QString &key) const;
    QHash<QString, QString> currentSelections() const;
    QString measurementForKey(const QString &key, const QString &value) const;
    QString sectionTitle(const QString &key) const;
    QString formatOptionLabel(const QString &key, const QString &value) const;
    QWidget *makeStepSidebarItem(int index, const QString &title);
    void changeRelatedQty(QVector<RelatedPickState> &picks, int index, int delta);

    MenuDish m_package;
    int m_quantity = 1;
    PickStep m_step = PickStep::Personalize;
    bool m_hasDrinkStep = false;
    bool m_hasExtraStep = false;

    QWidget *m_card = nullptr;
    QPushButton *m_btnClose = nullptr;
    QLabel *m_lblImage = nullptr;
    QLabel *m_lblTitle = nullptr;
    QLabel *m_lblDescription = nullptr;
    QLabel *m_lblEmpty = nullptr;
    QWidget *m_badgesHost = nullptr;
    QHBoxLayout *m_badgesLayout = nullptr;
    QLabel *m_lblBjuPills = nullptr;
    QLineEdit *m_leInstructions = nullptr;

    QVBoxLayout *m_sidebarLayout = nullptr;
    QVector<QFrame *> m_stepItems;

    QStackedWidget *m_contentStack = nullptr;
    QWidget *m_pagePersonalize = nullptr;
    QWidget *m_pageDrink = nullptr;
    QWidget *m_pageExtra = nullptr;
    QLabel *m_lblPersonalizeTitle = nullptr;
    QVBoxLayout *m_attributesLayout = nullptr;
    QLabel *m_lblUnavailableHint = nullptr;
    QLabel *m_lblQty = nullptr;

    QPushButton *m_btnGoToMenu = nullptr;
    QLabel *m_lblTotal = nullptr;
    QPushButton *m_btnBack = nullptr;
    QPushButton *m_btnNext = nullptr;

    QVector<RelatedPickState> m_drinkPicks;
    QVector<RelatedPickState> m_extraPicks;

    QHash<QString, QButtonGroup *> m_attrGroups;
    QHash<int, QCheckBox *> m_modificatorChecks;
};

#endif // DLGPACKAGEPICK_H
