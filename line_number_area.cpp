#include "line_number_area.h"
#include <QPainter>
#include <QPaintEvent>

LineNumberArea::LineNumberArea(QWidget* editor, QWidget* parent)
    : QWidget(parent)
{
    Q_UNUSED(editor);
    setFixedWidth(45);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(45, 45, 45));
    setPalette(pal);
}
void LineNumberArea::update(int lineCount, int firstVisibleLine,
                            int lineHeight, int topOffset)
{
    m_lineCount    = lineCount;
    m_firstVisible = firstVisibleLine;
    m_lineHeight   = lineHeight > 0 ? lineHeight : 16;
    m_topOffset    = topOffset;
    QWidget::update(); // trigger repaint
}
void LineNumberArea::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(45, 45, 45));
    painter.setPen(QColor(130, 130, 130));
    painter.setFont(QFont("Courier New", 10));

    int y = m_topOffset;
    for (int line = m_firstVisible; line <= m_lineCount; ++line) {
        painter.drawText(0, y, width() - 4, m_lineHeight,
                         Qt::AlignRight | Qt::AlignVCenter,
                         QString::number(line));
        y += m_lineHeight;
        if (y > height()) break;
    }
}