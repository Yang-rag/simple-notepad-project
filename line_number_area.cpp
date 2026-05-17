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