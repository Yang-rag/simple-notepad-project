#include "word_frequency_dialog.h"
#include "ui_word_frequency_dialog.h"
#include <QRegularExpression>
#include <map>
#include <vector>
#include <algorithm>
 
WordFrequencyDialog::WordFrequencyDialog(const QString& text, QWidget* parent)
    : QDialog(parent), ui(new Ui::WordFrequencyDialog)
{
    ui->setupUi(this);
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::accept);
 
    // Count word frequencies
    std::map<QString, int> freq;
    QRegularExpression wordRe("[A-Za-z]+");
    auto it = wordRe.globalMatch(text);
    while (it.hasNext()) {
        QString word = it.next().captured().toLower();
        freq[word]++;
    }
}