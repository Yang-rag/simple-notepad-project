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
    QString toSentenceCase(const QString& text) {
        QString result = text.toLower();
        bool newSentence = true;
        for (int i = 0; i < result.size(); ++i) {
            if (result[i] == '.' || result[i] == '!' || result[i] == '?') {
                newSentence = true;
            } else if (newSentence && !result[i].isSpace()) {
                result[i] = result[i].toUpper();
                newSentence = false;
            }
        }
        return result;
    }
    QString toSwapCase(const QString& text) {
        QString result = text;
        for (int i = 0; i < result.size(); ++i) {
            if (result[i].isUpper())
                result[i] = result[i].toLower();
            else if (result[i].isLower())
                result[i] = result[i].toUpper();
        }
        return result;
    }
}