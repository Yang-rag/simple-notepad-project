#pragma once
#include <QDialog>

namespace Ui { class FindReplaceDialog; }
class QTextEdit;

class FindReplaceDialog : public QDialog {
    Q_OBJECT
public:
    explicit FindReplaceDialog(QTextEdit* editor, QWidget* parent = nullptr);
    ~FindReplaceDialog();

private slots:
    void findNext();
    void replace();
    void replaceAll();

private:
    Ui::FindReplaceDialog* ui;
    QTextEdit* m_editor;
};