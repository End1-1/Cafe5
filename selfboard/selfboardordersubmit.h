#ifndef SELFBOARDORDERSUBMIT_H
#define SELFBOARDORDERSUBMIT_H

#include "ordercart.h"

#include <QObject>
#include <QString>
#include <functional>

enum class SelfBoardServiceMode { TakeAway, DineIn };

class SelfBoardOrderSubmit
{
public:
    using FinishedCallback = std::function<void(bool ok, const QString &orderNumber, const QString &error)>;

    static void submit(OrderCart *cart,
                       SelfBoardServiceMode serviceMode,
                       QObject *context,
                       FinishedCallback finished);
};

#endif // SELFBOARDORDERSUBMIT_H
