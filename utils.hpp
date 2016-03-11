#pragma once

#include <QString>
#include <functional>

void doAction(const QString& action, std::function<void()>&& func);
void errorMessage(const QString& title, const QString& message);
