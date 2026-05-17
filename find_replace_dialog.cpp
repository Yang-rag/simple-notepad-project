#include "find_replace_dialog.h"
#include "ui_find_replace_dialog.h"
#include <QTextEdit>
#include <QMessageBox>

FindReplaceDialog::FindReplaceDialog(QTextEdit* editor, QWidget* parent)
    : QDialog(parent), ui(new Ui::FindReplaceDialog), m_editor(editor)
{
    ui->setupUi(this);
    connect(ui->findNextBtn,   &QPushButton::clicked, this, &FindReplaceDialog::findNext);
    connect(ui->replaceBtn,    &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(ui->replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
    connect(ui->closeBtn,      &QPushButton::clicked, this, &QDialog::accept);
}

FindReplaceDialog::~FindReplaceDialog() { delete ui; }

void FindReplaceDialog::findNext()
{
    QString term = ui->findEdit->text();
    if (term.isEmpty()) return;
    if (!m_editor->find(term))
        QMessageBox::information(this, "Find", "No more occurrences found.");
}

void FindReplaceDialog::replace()
{
    QString term = ui->findEdit->text();
    QString repl = ui->replaceEdit->text();
    if (term.isEmpty()) return;

    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == term) {
        cursor.insertText(repl);
    }
    // Move to next occurrence
    if (!m_editor->find(term))
        QMessageBox::information(this, "Replace", "No more occurrences found.");
}

void FindReplaceDialog::replaceAll()
{
    QString term = ui->findEdit->text();
    QString repl = ui->replaceEdit->text();
    if (term.isEmpty()) return;

    // Start from beginning
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(cursor);

    int count = 0;
    while (m_editor->find(term)) {
        m_editor->textCursor().insertText(repl);
        ++count;
    }
    QMessageBox::information(this, "Replace All",
        QString("Replaced %1 occurrence(s).").arg(count));
}