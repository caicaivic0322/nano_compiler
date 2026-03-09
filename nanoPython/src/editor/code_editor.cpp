#include "code_editor.h"
#include "syntax_highlighter.h"

#include <QPainter>
#include <QTextBlock>
#include <QKeyEvent>
#include <QScrollBar>
#include <QApplication>
#include <QScreen>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
    , m_highlighter(nullptr)
{
    setupEditor();

    // 连接信号 - 使用 Qt::QueuedConnection 提高响应性
    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth, Qt::QueuedConnection);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea, Qt::QueuedConnection);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine, Qt::QueuedConnection);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

CodeEditor::~CodeEditor() = default;

void CodeEditor::setupEditor()
{
    // 根据屏幕分辨率调整字体大小
    QScreen *screen = QApplication::primaryScreen();
    qreal dpi = screen->logicalDotsPerInch();
    int baseFontSize = (dpi >= 120) ? 11 : 14; // 低分辨率使用更大字体

    // 设置字体 - 使用更适合低分辨率显示器的字体
    QFont font("Consolas", baseFontSize);
    if (!QFontInfo(font).fixedPitch()) {
        // 如果 Consolas 不可用，使用备用字体
        font.setFamily("Courier New");
    }
    font.setFixedPitch(true);
    font.setStyleHint(QFont::Monospace);
    setFont(font);

    // 设置制表符宽度为4个空格
    setTabStopDistance(QFontMetricsF(font).horizontalAdvance(' ') * 4);

    // 启用行号
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);

    // 性能优化设置
    setCenterOnScroll(false);
    setWordWrapMode(QTextOption::NoWrap); // 不换行提高性能

    // 设置样式 - 优化低分辨率显示
    setStyleSheet("QPlainTextEdit {"
                  "  background-color: #ffffff;"
                  "  border: 1px solid #cccccc;"
                  "  border-radius: 4px;"
                  "  padding: 4px;"
                  "  selection-background-color: #107c10;"
                  "  selection-color: #ffffff;"
                  "}");

    // 创建语法高亮器
    m_highlighter = new SyntaxHighlighter(document());
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 增加行号区域宽度，低分辨率更清晰
    int space = 15 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return qMax(space, 45); // 最小宽度45像素
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        // 使用更明显的当前行高亮颜色
        QColor lineColor = QColor(232, 245, 233); // 浅绿色

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(245, 245, 245));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    // 设置行号字体
    QFont lineNumberFont = font();
    lineNumberFont.setPointSize(font().pointSize() - 1);
    painter.setFont(lineNumberFont);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            // 当前行的行号使用深色
            if (blockNumber == textCursor().blockNumber()) {
                painter.setPen(QColor(0, 0, 0));
                painter.setFont(QFont(lineNumberFont.family(), lineNumberFont.pointSize(), QFont::Bold));
            } else {
                painter.setPen(QColor(128, 128, 128));
                painter.setFont(lineNumberFont);
            }

            painter.drawText(0, top, m_lineNumberArea->width() - 8, fontMetrics().height(),
                           Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    // 处理括号匹配
    if (handleBracketMatching(event)) {
        return;
    }

    // 自动缩进处理
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QPlainTextEdit::keyPressEvent(event);
        autoIndent();
        return;
    }

    // Tab 键处理 - 使用空格代替制表符
    if (event->key() == Qt::Key_Tab) {
        insertPlainText("    "); // 插入4个空格
        return;
    }

    // Shift+Tab 减少缩进
    if (event->key() == Qt::Key_Backtab) {
        decreaseIndent();
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::autoIndent()
{
    QTextCursor cursor = textCursor();
    int currentPos = cursor.position();

    // 获取当前行
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString currentLine = cursor.selectedText();

    // 计算前导空格
    int indent = 0;
    for (QChar c : currentLine) {
        if (c == ' ') indent++;
        else if (c == '\t') indent += 4;
        else break;
    }

    // 如果行尾有 :，增加缩进（Python风格）
    QString trimmedLine = currentLine.trimmed();
    if (trimmedLine.endsWith(":") ||
        trimmedLine.endsWith("{")) {
        indent += 4;
    }

    // 插入缩进
    if (indent > 0) {
        cursor.setPosition(currentPos);
        cursor.insertText(QString(indent, ' '));
    }
}

void CodeEditor::decreaseIndent()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();

    // 计算前导空格
    int spacesToRemove = 0;
    for (QChar c : line) {
        if (c == ' ' && spacesToRemove < 4) {
            spacesToRemove++;
        } else {
            break;
        }
    }

    if (spacesToRemove > 0) {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, spacesToRemove);
        cursor.removeSelectedText();
    }
}

bool CodeEditor::handleBracketMatching(QKeyEvent *event)
{
    QString text = event->text();
    if (text.isEmpty()) return false;

    QChar ch = text.at(0);
    QString closeBracket;

    switch (ch.unicode()) {
        case '(': closeBracket = ")"; break;
        case '[': closeBracket = "]"; break;
        case '{': closeBracket = "}"; break;
        case '"': closeBracket = "\""; break;
        case '\'': closeBracket = "'"; break;
        default: return false;
    }

    QTextCursor cursor = textCursor();
    cursor.insertText(ch + closeBracket);
    cursor.movePosition(QTextCursor::Left);
    setTextCursor(cursor);
    return true;
}

void CodeEditor::onTextChanged()
{
    // 可以在这里添加更多文本变化处理
}
