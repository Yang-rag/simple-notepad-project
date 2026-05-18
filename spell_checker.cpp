#include "spell_checker.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <vector>

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
    QString cleaned = word.toLower().remove(QRegularExpression("[^a-z]"));
    if (cleaned.isEmpty()) return true;
    return m_words.count(cleaned.toStdString()) > 0;
}

QStringList SpellChecker::suggestions(const QString& word) const
{
    QStringList result;
    std::string target = word.toLower().toStdString();

    auto editDistance = [](const std::string& a, const std::string& b) -> int {
        int m = a.size(), n = b.size();
        if (abs(m - n) > 2) return 99;
        std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1));
        for (int i = 0; i <= m; ++i) dp[i][0] = i;
        for (int j = 0; j <= n; ++j) dp[0][j] = j;
        for (int i = 1; i <= m; ++i)
            for (int j = 1; j <= n; ++j) {
                dp[i][j] = a[i-1] == b[j-1]
                    ? dp[i-1][j-1]
                    : 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
                if (i > 1 && j > 1 && a[i-1] == b[j-2] && a[i-2] == b[j-1])
                    dp[i][j] = std::min(dp[i][j], dp[i-2][j-2] + 1);
            }
        return dp[m][n];
    };

    std::vector<std::pair<int,std::string>> candidates;
    for (const auto& w : m_words) {
        if (abs((int)w.size() - (int)target.size()) > 2) continue;
        int dist = editDistance(target, w);
        if (dist == 1)
            candidates.push_back({dist, w});
    }

    if (candidates.size() < 5) {
        for (const auto& w : m_words) {
            if (abs((int)w.size() - (int)target.size()) > 2) continue;
            int dist = editDistance(target, w);
            if (dist == 2)
                candidates.push_back({dist, w});
            if (candidates.size() >= 20) break;
        }
    }

    std::sort(candidates.begin(), candidates.end());

    for (int i = 0; i < std::min((int)candidates.size(), 5); ++i)
        result << QString::fromStdString(candidates[i].second);

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