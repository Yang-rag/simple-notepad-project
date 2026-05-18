#include "spell_checker.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>

SpellChecker::SpellChecker(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    m_wordPattern = QRegularExpression("[A-Za-z]+");
}

bool SpellChecker::loadWordList(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty())
            m_words.insert(line.toLower().toStdString());
    }
    return true;
}

bool SpellChecker::isCorrect(const QString& word) const
{
    if (m_words.empty()) return true;
    // Strip non-alpha characters, lowercase
    QString cleaned = word.toLower().remove(QRegularExpression("[^a-z]"));
    if (cleaned.isEmpty()) return true;
    return m_words.count(cleaned.toStdString()) > 0;
}

QStringList SpellChecker::suggestions(const QString& word) const
{
    QStringList result;
    std::string target = word.toLower().toStdString();
    size_t tlen = target.size();

    for (const auto& w : m_words) {
        if (result.size() >= 5) break;

        if (w.size() == tlen) {
            int diffs = 0;
            for (size_t i = 0; i < tlen; ++i) {
                if (w[i] != target[i]) ++diffs;
                if (diffs > 1) break;
            }
            if (diffs == 1) {
                result << QString::fromStdString(w);
                continue;
            }
        }

        if (w.size() == tlen + 1 || w.size() + 1 == tlen) {
            const std::string& longer  = w.size() > tlen ? w : target;
            const std::string& shorter = w.size() > tlen ? target : w;
            int diffs = 0;
            size_t i = 0, j = 0;
            while (i < longer.size() && j < shorter.size()) {
                if (longer[i] != shorter[j]) {
                    ++diffs; ++i;
                } else {
                    ++i; ++j;
                }
                if (diffs > 1) break;
            }
            if (diffs <= 1)
                result << QString::fromStdString(w);
        }
    }

    return result;
}

void SpellChecker::highlightBlock(const QString& text)
{
    if (m_words.empty()) return;

    QRegularExpressionMatchIterator it = m_wordPattern.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString word = match.captured();
        if (!isCorrect(word))
            setFormat(match.capturedStart(), match.capturedLength(), m_errorFormat);
    }
}