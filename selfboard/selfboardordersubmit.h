#ifndef SELFBOARDORDERSUBMIT_H
#define SELFBOARDORDERSUBMIT_H

#include "ordercart.h"

#include <QObject>
#include <QString>
#include <functional>

enum class SelfBoardServiceMode { TakeAway, DineIn };

struct SelfBoardSubmitResult
{
    bool ok = false;
    QString orderNumber;
    QString publicToken;
    QString statusUrl;
    QString error;
};

class SelfBoardOrderSubmit
{
public:
    using FinishedCallback = std::function<void(const SelfBoardSubmitResult &result)>;

    static void submit(OrderCart *cart,
                       SelfBoardServiceMode serviceMode,
                       QObject *context,
                       FinishedCallback finished);
};

#endif // SELFBOARDORDERSUBMIT_H
