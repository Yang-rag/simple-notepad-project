#pragma once
#include <QString>

// Stateless text transformation utilities
namespace TextTransform {
    QString toUpperCase(const QString& text);
    QString toLowerCase(const QString& text);
    QString toCapitalize(const QString& text);   // first letter of each word
    QString toSentenceCase(const QString& text); // first letter of each sentence
    QString toSwapCase(const QString& text);
}