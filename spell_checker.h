#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <set>
#include <string>

class SpellChecker : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit SpellChecker(QTextDocument* parent = nullptr);
    bool loadWordList(const QString& path);
    bool isCorrect(const QString& word) const;
    QStringList suggestions(const QString& word) const;

protected:
    void highlightBlock(const QString& text) override;

private:
    std::set<std::string> m_words;
    QTextCharFormat m_errorFormat;
    QRegularExpression m_wordPattern;
};