#include "text_transform.h"

namespace TextTransform {

    QString toUpperCase(const QString& text) {
        return text.toUpper();
    }

    QString toLowerCase(const QString& text) {
        return text.toLower();
    }

    QString toCapitalize(const QString& text) {
        QString result = text.toLower();
        bool newWord = true;
        for (int i = 0; i < result.size(); ++i) {
            if (result[i].isSpace()) {
                newWord = true;
            } else if (newWord) {
                result[i] = result[i].toUpper();
                newWord = false;
            }
        }
        return result;
    }

