#pragma once
#include <QWidget>

class MainWindow;

class LineNumberArea : public QWidget {
    Q_OBJECT
public:
    explicit LineNumberArea(QWidget* editor, QWidget* parent = nullptr);

    void update(int lineCount, int firstVisibleLine, int lineHeight, int topOffset);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_lineCount      = 0;
    int m_firstVisible   = 1;
    int m_lineHeight     = 16;
    int m_topOffset      = 0;
};