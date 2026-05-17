#include "spell_checker.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>

SpellChecker::SpellChecker(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    // Red underline for misspelled words
    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    // Match sequences of alphabetic characters only
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
    if (m_words.empty()) return true; // no dictionary loaded = no errors shown
    // Strip non-alpha characters, lowercase
    QString cleaned = word.toLower().remove(QRegularExpression("[^a-z]"));
    if (cleaned.isEmpty()) return true;
    return m_words.count(cleaned.toStdString()) > 0;
}

QStringList SpellChecker::suggestions(const QString& word) const
{
    QStringList result;
    std::string target = word.toLower().toStdString();

    // Simple suggestion: find words that differ by at most 1 character (substitution)
    for (const auto& w : m_words) {
        if (result.size() >= 5) break;
        if (w.size() != target.size()) continue;
        int diffs = 0;
        for (size_t i = 0; i < w.size(); ++i) {
            if (w[i] != target[i]) ++diffs;
            if (diffs > 1) break;
        }
        if (diffs == 1)
            result << QString::fromStdString(w);
    }

    // If not enough, also try words with same first 3 chars
    if (result.size() < 5 && target.size() >= 3) {
        std::string prefix = target.substr(0, 3);
        auto it = m_words.lower_bound(prefix);
        while (it != m_words.end() && result.size() < 5) {
            if (it->substr(0, 3) != prefix) break;
            QString candidate = QString::fromStdString(*it);
            if (!result.contains(candidate) && candidate.toLower() != word.toLower())
                result << candidate;
            ++it;
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