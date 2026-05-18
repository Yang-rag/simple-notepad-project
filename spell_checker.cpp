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
    // Strip non-alpha characters, lowercase
    QString cleaned = word.toLower().remove(QRegularExpression("[^a-z]"));
    if (cleaned.isEmpty()) return true;
    return m_words.count(cleaned.toStdString()) > 0;
}

QStringList SpellChecker::suggestions(const QString& word) const
{
    QStringList result;
    std::string target = word.toLower().toStdString();

    // 只用一个策略：找所有和目标词开头字母相同的词
    // 然后计算编辑距离，选最近的5个
    char firstChar = target.empty() ? 0 : target[0];

    // 计算两个字符串的编辑距离（Levenshtein distance）
    auto editDistance = [](const std::string& a, const std::string& b) -> int {
        int m = a.size(), n = b.size();
        std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1));
        for (int i = 0; i <= m; ++i) dp[i][0] = i;
        for (int j = 0; j <= n; ++j) dp[0][j] = j;
        for (int i = 1; i <= m; ++i)
            for (int j = 1; j <= n; ++j)
                dp[i][j] = a[i-1] == b[j-1]
                    ? dp[i-1][j-1]
                    : 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        return dp[m][n];
    };

    // 收集首字母相同、编辑距离<=2的词
    std::vector<std::pair<int,std::string>> candidates;
    for (const auto& w : m_words) {
        if (w.empty() || w[0] != firstChar) continue;
        int dist = editDistance(target, w);
        if (dist <= 2 && dist > 0)
            candidates.push_back({dist, w});
    }

    // 按编辑距离排序，距离小的排前面
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