#pragma once
#include <QDialog>

namespace Ui { class WordFrequencyDialog; }

class WordFrequencyDialog : public QDialog {
    Q_OBJECT
public:
    explicit WordFrequencyDialog(const QString& text, QWidget* parent = nullptr);
    ~WordFrequencyDialog();
private:
    Ui::WordFrequencyDialog* ui;
};