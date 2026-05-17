#pragma once
#include <QWidget>

class MainWindow;

// A small widget painted on the left side of the editor showing line numbers.
// It is a child widget of the editor's viewport parent (MainWindow).
class LineNumberArea : public QWidget {
    Q_OBJECT
public:
    explicit LineNumberArea(QWidget* editor, QWidget* parent = nullptr);

    // Call this whenever the editor's document or scroll position changes
    void update(int lineCount, int firstVisibleLine, int lineHeight, int topOffset);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_lineCount      = 0;
    int m_firstVisible   = 1;
    int m_lineHeight     = 16;
    int m_topOffset      = 0;
};