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