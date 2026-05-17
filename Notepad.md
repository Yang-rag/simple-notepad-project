# Notepad Implementation Notes

## Required Features

### 1. Exception Handling
Defined a four-class hierarchy in `notepad_exception.h`:
- `notepad_exception` (base, extends `std::runtime_error`)
- `file_not_found_exception` — thrown when the file path does not exist
- `file_read_exception` — thrown when `QFile::open` fails for reading
- `file_write_exception` — thrown when `QFile::open` fails for writing

`open_file()` and `save_file()` in `main_window.cpp` are wrapped in `try/catch(notepad_exception)` blocks. Errors are shown via `QMessageBox::critical`.

### 2. Spell Checker
`SpellChecker` extends `QSyntaxHighlighter`. At startup it reads `data/words.txt` into a `std::set<std::string>` for O(log n) lookup.

- **Real-time highlighting**: `highlightBlock()` runs on every edited paragraph and underlines unknown words in red using `QTextCharFormat::SpellCheckUnderline`.
- **Right-click suggestions**: a custom context menu finds up to 5 candidate words (same-length words differing by one character, plus prefix matches). Clicking a suggestion replaces the misspelled word.
- **Tools > Check Spelling**: calls `rehighlight()` to rescan the whole document.

## Optional Features Chosen

### 1. Cursor Line / Column Indicator
A second `QLabel` (`m_cursorLabel`) is added to the status bar. It is updated on every `cursorPositionChanged` signal and displays "Ln X, Col Y".

### 2. Font Dialog
`Format > Font...` opens Qt's built-in `QFontDialog`. If text is selected, the chosen font is applied only to the selection via `mergeCharFormat`; otherwise it becomes the editor's default font.

### 3. Color Picker
`Format > Text Color...` opens `QColorDialog`. The chosen color is applied to the current selection via `mergeCharFormat`.

### 8. Zoom
`View > Zoom In / Zoom Out / Reset Zoom` (shortcuts Ctrl++, Ctrl+-, Ctrl+0) adjust the editor's font point size by ±2 pt. Reset restores the initial size stored in `m_baseFontSize`.

## Design Decisions
- `SpellChecker` is a `QSyntaxHighlighter` subclass because Qt integrates it directly into the document rendering pipeline — no manual repaint required.
- Exceptions are caught at the UI boundary (menu action handlers) so error dialogs are always shown in context.
- `LineNumberArea` is a separate `QWidget` placed to the left of the editor in a horizontal layout, keeping concerns separated.