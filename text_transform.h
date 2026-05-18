#pragma once
#include <QString>

namespace TextTransform {
    QString toUpperCase(const QString& text);
    QString toLowerCase(const QString& text);
    QString toCapitalize(const QString& text);
    QString toSentenceCase(const QString& text);
    QString toSwapCase(const QString& text);
}