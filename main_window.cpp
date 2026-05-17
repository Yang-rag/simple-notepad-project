#include "main_window.h"
#include "notepad_exception.h"
#include "spell_checker.h"
#include "text_transform.h"
#include "sort.h"
#include "find_replace_dialog.h"
#include "word_frequency_dialog.h"
#include "line_number_area.h"

#include <QTextEdit>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QAction>
#include <QInputDialog>
#include <functional>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Central widget: line numbers + editor side by side
    QWidget* central = new QWidget(this);
    QHBoxLayout* hlay = new QHBoxLayout(central);
    hlay->setContentsMargins(0, 0, 0, 0);
    hlay->setSpacing(0);

    m_lineNumbers = new LineNumberArea(nullptr, central);
    hlay->addWidget(m_lineNumbers);

    m_editor = new QTextEdit(central);
    m_editor->setFont(QFont("Courier New", m_baseFontSize));
    hlay->addWidget(m_editor);

    setCentralWidget(central);

    setupMenus();
    setupToolBar();
    setupStatusBar();

    // Load spell checker dictionary
    m_spellChecker = new SpellChecker(m_editor->document());
    if (!m_spellChecker->loadWordList("data/words.txt")) {
        // Not fatal – spell check just won't highlight anything
        statusBar()->showMessage("Warning: could not load data/words.txt", 3000);
    }

    // Wire up status-bar and line-number updates
    connect(m_editor, &QTextEdit::textChanged,       this, &MainWindow::updateStatusBar);
    connect(m_editor, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    connect(m_editor->verticalScrollBar(), &QScrollBar::valueChanged,
            this, [this](int){ refreshLineNumbers(); });
    connect(m_editor, &QTextEdit::textChanged, this, &MainWindow::refreshLineNumbers);

    // Context menu for spell suggestions
    m_editor->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_editor, &QTextEdit::customContextMenuRequested, this, [this](const QPoint& pos){
        QMenu* menu = m_editor->createStandardContextMenu();

        QTextCursor cursor = m_editor->cursorForPosition(pos);
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selectedText();

        if (!word.isEmpty() && !m_spellChecker->isCorrect(word)) {
            menu->addSeparator();
            QStringList sugg = m_spellChecker->suggestions(word);
            if (sugg.isEmpty()) {
                menu->addAction("(No suggestions)")->setEnabled(false);
            } else {
                for (const QString& s : sugg) {
                    QAction* act = menu->addAction(s);
                    connect(act, &QAction::triggered, this, [this, cursor, s]() mutable {
                        cursor.insertText(s);
                    });
                }
            }
        }

        menu->exec(m_editor->viewport()->mapToGlobal(pos));
        delete menu;
    });

    // Mark modified
    connect(m_editor->document(), &QTextDocument::modificationChanged,
            this, [this](bool mod){ m_modified = mod; updateTitle(); });

    updateTitle();
    updateStatusBar();
    refreshLineNumbers();

    resize(900, 650);
}

// ─────────────────────────────────────────────
//  Menu setup
// ─────────────────────────────────────────────
void MainWindow::setupMenus()
{
    // ── File ──
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New",       this, &MainWindow::newFile,  QKeySequence::New);
    fileMenu->addAction("&Open...",   this, &MainWindow::openFile, QKeySequence::Open);
    fileMenu->addAction("&Save",      this, &MainWindow::saveFile, QKeySequence::Save);
    fileMenu->addAction("Save &As...",this, &MainWindow::saveFileAs);
    fileMenu->addSeparator();
    fileMenu->addAction("&Print...",  this, [](){
        QMessageBox::information(nullptr, "Print", "Print feature not enabled in this build.");
    });
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

    // ── Edit ──
    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Undo",       this, &MainWindow::undo,         QKeySequence::Undo);
    editMenu->addAction("&Redo",       this, &MainWindow::redo,         QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("Cu&t",        this, &MainWindow::cut,          QKeySequence::Cut);
    editMenu->addAction("&Copy",       this, &MainWindow::copy,         QKeySequence::Copy);
    editMenu->addAction("&Paste",      this, &MainWindow::paste,        QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction("Select &All", this, &MainWindow::selectAll,    QKeySequence::SelectAll);
    editMenu->addSeparator();
    editMenu->addAction("&Find / Replace...", this, &MainWindow::showFindReplace,   QKeySequence::Find);
    editMenu->addAction("&Word Frequency...", this, &MainWindow::showWordFrequency);

    // ── Format ──
    QMenu* fmtMenu = menuBar()->addMenu("F&ormat");
    fmtMenu->addAction("&Bold",      this, &MainWindow::toggleBold,      QKeySequence::Bold);
    fmtMenu->addAction("&Italic",    this, &MainWindow::toggleItalic,    QKeySequence::Italic);
    fmtMenu->addAction("&Underline", this, &MainWindow::toggleUnderline, QKeySequence::Underline);
    fmtMenu->addSeparator();

    // Optional feature 2: Font dialog
    fmtMenu->addAction("&Font...",       this, &MainWindow::showFontDialog);
    // Optional feature 3: Color picker
    fmtMenu->addAction("Text &Color...", this, &MainWindow::showColorDialog);
    fmtMenu->addSeparator();

    QMenu* caseMenu = fmtMenu->addMenu("Text &Case");
    caseMenu->addAction("UPPER CASE",    this, &MainWindow::transformUpperCase);
    caseMenu->addAction("lower case",    this, &MainWindow::transformLowerCase);
    caseMenu->addAction("Capitalize",    this, &MainWindow::transformCapitalize);
    caseMenu->addAction("Sentence case", this, &MainWindow::transformSentenceCase);
    caseMenu->addAction("sWAP cASE",     this, &MainWindow::transformSwapCase);

    // ── View (optional feature 8: zoom) ──
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Zoom &In",    this, &MainWindow::zoomIn,    QKeySequence(Qt::CTRL | Qt::Key_Equal));
    viewMenu->addAction("Zoom &Out",   this, &MainWindow::zoomOut,   QKeySequence(Qt::CTRL | Qt::Key_Minus));
    viewMenu->addAction("&Reset Zoom", this, &MainWindow::zoomReset, QKeySequence(Qt::CTRL | Qt::Key_0));

    // ── Search ──
    QMenu* searchMenu = menuBar()->addMenu("&Search");
    searchMenu->addAction("&Find / Replace...", this, &MainWindow::showFindReplace, QKeySequence::Find);

    // ── Tools ──
    QMenu* toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction("&Check Spelling...", this, &MainWindow::checkSpelling);
    toolsMenu->addAction("&Sort Lines...",      this, &MainWindow::showSortDialog);
}

// ─────────────────────────────────────────────
//  Toolbar
// ─────────────────────────────────────────────
void MainWindow::setupToolBar()
{
    QToolBar* tb = addToolBar("Format");
    tb->addAction("B", this, &MainWindow::toggleBold);
    tb->addAction("I", this, &MainWindow::toggleItalic);
    tb->addAction("U", this, &MainWindow::toggleUnderline);
}

// ─────────────────────────────────────────────
//  Status bar  (words · lines · line:col)
// ─────────────────────────────────────────────
void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(this);
    m_cursorLabel = new QLabel(this);  // optional feature 1
    statusBar()->addPermanentWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_cursorLabel);
}

void MainWindow::updateStatusBar()
{
    QString text = m_editor->toPlainText();
    int words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
    int lines = text.isEmpty() ? 0 : text.count('\n') + 1;
    m_statusLabel->setText(QString("Words: %1  Lines: %2").arg(words).arg(lines));

    // Optional feature 1: cursor line / column
    QTextCursor cursor = m_editor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col  = cursor.columnNumber() + 1;
    m_cursorLabel->setText(QString("Ln %1, Col %2").arg(line).arg(col));

    refreshLineNumbers();
}

// ─────────────────────────────────────────────
//  Line numbers  (uses line_number_area)
// ─────────────────────────────────────────────
void MainWindow::setupLineNumbers() { /* wired in constructor */ }

void MainWindow::refreshLineNumbers()
{
    QString text = m_editor->toPlainText();
    int totalLines = text.isEmpty() ? 1 : text.count('\n') + 1;

    QTextCursor cursor = m_editor->textCursor();
    QFontMetrics fm(m_editor->font());
    int lineH = fm.height() + 2;

    // First visible line based on scroll position
    int scrollVal     = m_editor->verticalScrollBar()->value();
    int firstVisible  = scrollVal / (lineH > 0 ? lineH : 1) + 1;

    m_lineNumbers->update(totalLines, firstVisible, lineH, 0);
}

// ─────────────────────────────────────────────
//  Title
// ─────────────────────────────────────────────
void MainWindow::updateTitle()
{
    QString title = "Notepad";
    if (!m_currentFile.isEmpty())
        title = "Notepad: " + m_currentFile;
    if (m_modified)
        title += " *";
    setWindowTitle(title);
}

// ─────────────────────────────────────────────
//  File operations  (wrapped in try/catch)
// ─────────────────────────────────────────────
void MainWindow::newFile()
{
    if (m_modified) {
        auto btn = QMessageBox::question(this, "Notepad",
            "Unsaved changes. Discard?",
            QMessageBox::Yes | QMessageBox::No);
        if (btn != QMessageBox::Yes) return;
    }
    m_editor->clear();
    m_currentFile.clear();
    m_modified = false;
    updateTitle();
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Open File", "",
        "Text Files (*.txt);;All Files (*)");
    if (path.isEmpty()) return;

    try {
        QFile file(path);
        if (!file.exists())
            throw file_not_found_exception(path.toStdString());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw file_read_exception(path.toStdString());

        QTextStream in(&file);
        m_editor->setPlainText(in.readAll());
        m_currentFile = path;
        m_modified    = false;
        m_editor->document()->setModified(false);
        updateTitle();
    } catch (const notepad_exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::saveFile()
{
    if (m_currentFile.isEmpty()) { saveFileAs(); return; }

    try {
        QFile file(m_currentFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            throw file_write_exception(m_currentFile.toStdString());

        QTextStream out(&file);
        out << m_editor->toPlainText();
        m_modified = false;
        m_editor->document()->setModified(false);
        updateTitle();
    } catch (const notepad_exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::saveFileAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Save File As", "",
        "Text Files (*.txt);;All Files (*)");
    if (path.isEmpty()) return;
    m_currentFile = path;
    saveFile();
}

// ─────────────────────────────────────────────
//  Edit actions
// ─────────────────────────────────────────────
void MainWindow::undo()       { m_editor->undo(); }
void MainWindow::redo()       { m_editor->redo(); }
void MainWindow::cut()        { m_editor->cut(); }
void MainWindow::copy()       { m_editor->copy(); }
void MainWindow::paste()      { m_editor->paste(); }
void MainWindow::selectAll()  { m_editor->selectAll(); }

void MainWindow::showFindReplace()
{
    FindReplaceDialog dlg(m_editor, this);
    dlg.exec();
}

void MainWindow::showWordFrequency()
{
    WordFrequencyDialog dlg(m_editor->toPlainText(), this);
    dlg.exec();
}

// ─────────────────────────────────────────────
//  Rich text formatting
// ─────────────────────────────────────────────
void MainWindow::toggleBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(m_editor->fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    m_editor->mergeCurrentCharFormat(fmt);
}

void MainWindow::toggleItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(!m_editor->fontItalic());
    m_editor->mergeCurrentCharFormat(fmt);
}

void MainWindow::toggleUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(!m_editor->fontUnderline());
    m_editor->mergeCurrentCharFormat(fmt);
}

// ─────────────────────────────────────────────
//  Optional feature 2: Font dialog
// ─────────────────────────────────────────────
void MainWindow::showFontDialog()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_editor->font(), this, "Select Font");
    if (ok) {
        // Apply to selection, or to whole document if nothing selected
        QTextCursor cursor = m_editor->textCursor();
        if (cursor.hasSelection()) {
            QTextCharFormat fmt;
            fmt.setFont(font);
            cursor.mergeCharFormat(fmt);
        } else {
            m_editor->setFont(font);
            m_baseFontSize = font.pointSize();
        }
        refreshLineNumbers();
    }
}

// ─────────────────────────────────────────────
//  Optional feature 3: Color picker
// ─────────────────────────────────────────────
void MainWindow::showColorDialog()
{
    QColor color = QColorDialog::getColor(m_editor->textColor(), this, "Select Text Color");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        m_editor->mergeCurrentCharFormat(fmt);
    }
}

// ─────────────────────────────────────────────
//  Optional feature 8: Zoom
// ─────────────────────────────────────────────
void MainWindow::zoomIn()
{
    QFont f = m_editor->font();
    f.setPointSize(f.pointSize() + 2);
    m_editor->setFont(f);
    refreshLineNumbers();
}

void MainWindow::zoomOut()
{
    QFont f = m_editor->font();
    int sz = f.pointSize() - 2;
    if (sz < 6) sz = 6;
    f.setPointSize(sz);
    m_editor->setFont(f);
    refreshLineNumbers();
}

void MainWindow::zoomReset()
{
    QFont f = m_editor->font();
    f.setPointSize(m_baseFontSize);
    m_editor->setFont(f);
    refreshLineNumbers();
}

// ─────────────────────────────────────────────
//  Text case transforms
// ─────────────────────────────────────────────
void MainWindow::applyTransform(const std::function<QString(const QString&)>& fn)
{
    QTextCursor cursor = m_editor->textCursor();
    if (!cursor.hasSelection()) cursor.select(QTextCursor::Document);
    cursor.insertText(fn(cursor.selectedText()));
}

void MainWindow::transformUpperCase()   { applyTransform(TextTransform::toUpperCase); }
void MainWindow::transformLowerCase()   { applyTransform(TextTransform::toLowerCase); }
void MainWindow::transformCapitalize()  { applyTransform(TextTransform::toCapitalize); }
void MainWindow::transformSentenceCase(){ applyTransform(TextTransform::toSentenceCase); }
void MainWindow::transformSwapCase()    { applyTransform(TextTransform::toSwapCase); }

// ─────────────────────────────────────────────
//  Spell check (re-highlight whole document)
// ─────────────────────────────────────────────
void MainWindow::checkSpelling()
{
    m_spellChecker->rehighlight();
    QMessageBox::information(this, "Spell Check", "Spell check complete.");
}

// ─────────────────────────────────────────────
//  Sort lines
// ─────────────────────────────────────────────
void MainWindow::showSortDialog()
{
    QStringList options = {"Ascending", "Descending"};
    bool ok;
    QString choice = QInputDialog::getItem(this, "Sort Lines", "Order:", options, 0, false, &ok);
    if (!ok) return;
    bool asc = (choice == "Ascending");
    m_editor->setPlainText(Sort::sortLines(m_editor->toPlainText(), asc));
}