#pragma once
#include <QMainWindow>
#include <QString>

class QTextEdit;
class QLabel;
class SpellChecker;
class FindReplaceDialog;
class LineNumberArea;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    // File
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();

    // Edit
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void showFindReplace();
    void showWordFrequency();

    // Format – rich text
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();

    // Format – text case transforms
    void transformUpperCase();
    void transformLowerCase();
    void transformCapitalize();
    void transformSentenceCase();
    void transformSwapCase();

    // Optional feature 1: Font dialog
    void showFontDialog();

    // Optional feature 3: Color picker
    void showColorDialog();

    // Optional feature 8: Zoom
    void zoomIn();
    void zoomOut();
    void zoomReset();

    // Tools
    void checkSpelling();
    void showSortDialog();

    // Status bar updates
    void updateStatusBar();

    // Line number refresh
    void refreshLineNumbers();

private:
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupLineNumbers();
    void updateTitle();
    void applyTransform(const std::function<QString(const QString&)>& fn);

    QTextEdit*       m_editor       = nullptr;
    QLabel*          m_statusLabel  = nullptr;  // words / lines
    QLabel*          m_cursorLabel  = nullptr;  // line:col  (optional feature 1)
    SpellChecker*    m_spellChecker = nullptr;
    LineNumberArea*  m_lineNumbers  = nullptr;

    QString          m_currentFile;
    bool             m_modified     = false;
    int              m_baseFontSize = 12;       // for zoom
};