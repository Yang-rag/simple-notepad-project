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
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void showFindReplace();
    void showWordFrequency();

    void toggleBold();
    void toggleItalic();
    void toggleUnderline();

    void transformUpperCase();
    void transformLowerCase();
    void transformCapitalize();
    void transformSentenceCase();
    void transformSwapCase();

    void showFontDialog();

    void showColorDialog();

    void zoomIn();
    void zoomOut();
    void zoomReset();

    void checkSpelling();
    void showSortDialog();

    void updateStatusBar();

    void refreshLineNumbers();

private:
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupLineNumbers();
    void updateTitle();
    void applyTransform(const std::function<QString(const QString&)>& fn);

    QTextEdit*       m_editor       = nullptr;
    QLabel*          m_statusLabel  = nullptr;
    QLabel*          m_cursorLabel  = nullptr;
    SpellChecker*    m_spellChecker = nullptr;
    LineNumberArea*  m_lineNumbers  = nullptr;

    QString          m_currentFile;
    bool             m_modified     = false;
    int              m_baseFontSize = 12;
};