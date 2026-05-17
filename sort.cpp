#include "sort.h"
#include <QStringList>
#include <algorithm>

namespace Sort {

    QString sortLines(const QString& text, bool ascending) {
        QStringList lines = text.split('\n');
        if (ascending)
            std::sort(lines.begin(), lines.end());
        else
            std::sort(lines.begin(), lines.end(), std::greater<QString>());
        return lines.join('\n');
    }

} // namespace Sort