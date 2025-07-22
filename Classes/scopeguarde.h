#ifndef SCOPEGUARDE_H
#define SCOPEGUARDE_H

#pragma once

#include <utility>

template <typename F>
class QScopeExit
{
    F func;
    bool active = true;
public:
    explicit QScopeExit(F f) : func(std::move(f)) {}
    ~QScopeExit() { if(active) func(); }
    QScopeExit(const QScopeExit&) = delete;
    QScopeExit& operator=(const QScopeExit&) = delete;
    void dismiss() { active = false; }
};

template <typename F>
QScopeExit<F> qScopeExit(F f)
{
    return QScopeExit<F>(std::move(f));
}

#endif // SCOPEGUARDE_H
