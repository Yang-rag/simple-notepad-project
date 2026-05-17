#include "word_frequency_dialog.h"
#include "ui_word_frequency_dialog.h"
#include <QRegularExpression>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <map>
#include <vector>
#include <algorithm>

WordFrequencyDialog::WordFrequencyDialog(const QString& text, QWidget* parent)
    : QDialog(parent), ui(new Ui::WordFrequencyDialog)
{
    ui->setupUi(this);
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    std::map<QString, int> freq;
    QRegularExpression wordRe("[A-Za-z]+");
    auto it = wordRe.globalMatch(text);
    while (it.hasNext()) {
        QString word = it.next().captured().toLower();
        freq[word]++;
    }

    std::vector<std::pair<QString,int>> sorted(freq.begin(), freq.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });

    ui->tableWidget->setRowCount(static_cast<int>(sorted.size()));
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    int row = 0;
    for (const auto& [word, count] : sorted) {
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(word));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(count)));
        ++row;
    }
}

WordFrequencyDialog::~WordFrequencyDialog()
{
    delete ui;
}