#include "code_editor.h"
#include "syntax_highlighter.h"
#include "code_folding.h"

#include <QPainter>
#include <QTextBlock>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QTimer>

// LineNumberArea 实现
LineNumberArea::LineNumberArea(CodeEditor *editor)
    : QWidget(editor), m_codeEditor(editor)
{
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    m_codeEditor->lineNumberAreaPaintEvent(event);
}

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
    , m_highlighter(nullptr)
    , m_folding(new CodeFolding(this))
{
    setupEditor();
    
    // 连接信号 - 使用 Qt::QueuedConnection 提高响应性
    connect(this, &QPlainTextEdit::blockCountChanged, 
            this, &CodeEditor::updateLineNumberAreaWidth, Qt::QueuedConnection);
    connect(this, &QPlainTextEdit::updateRequest, 
            this, &CodeEditor::updateLineNumberArea, Qt::QueuedConnection);
    connect(this, &QPlainTextEdit::cursorPositionChanged, 
            this, &CodeEditor::highlightCurrentLine, Qt::QueuedConnection);
    
    // 文档变化时重新分析折叠区域
    connect(document(), &QTextDocument::contentsChange,
            this, &CodeEditor::onDocumentChanged, Qt::QueuedConnection);
    
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
    
    // 启用行号 - 增加宽度以容纳折叠指示器
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
                  "  selection-background-color: #0078d7;"
                  "  selection-color: #ffffff;"
                  "}");
    
    // 创建语法高亮器
    m_highlighter = new SyntaxHighlighter(document());
}

void CodeEditor::setFileType(const QString &extension)
{
    m_fileExtension = extension.toLower();
    updateHighlighter();
}

void CodeEditor::updateHighlighter()
{
    if (m_highlighter) {
        delete m_highlighter;
    }
    // nanoPython 只支持 Python 语法高亮
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
    
    // 增加行号区域宽度，包含折叠指示器区域
    int space = 25 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return qMax(space, 55); // 最小宽度55像素
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
        QColor lineColor = QColor(230, 243, 255); // 浅蓝色
        
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
    painter.fillRect(event->rect(), QColor(240, 240, 240));
    
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    
    // 设置行号字体
    QFont lineNumberFont = font();
    lineNumberFont.setPointSize(font().pointSize() - 1);
    painter.setFont(lineNumberFont);
    
    int foldIndicatorWidth = m_folding ? m_folding->foldIndicatorWidth() : 0;
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            
            // 绘制折叠指示器
            if (m_folding && m_folding->canFoldLine(blockNumber)) {
                drawFoldIndicator(painter, blockNumber, top);
            }
            
            // 当前行的行号使用深色
            if (blockNumber == textCursor().blockNumber()) {
                painter.setPen(QColor(0, 0, 0));
                painter.setFont(QFont(lineNumberFont.family(), lineNumberFont.pointSize(), QFont::Bold));
            } else {
                painter.setPen(QColor(128, 128, 128));
                painter.setFont(lineNumberFont);
            }
            
            // 绘制行号（留出折叠指示器空间）
            painter.drawText(foldIndicatorWidth + 5, top, 
                           m_lineNumberArea->width() - foldIndicatorWidth - 13, 
                           fontMetrics().height(),
                           Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::drawFoldIndicator(QPainter &painter, int line, int top)
{
    int size = 10;
    int x = 3;
    int y = top + (fontMetrics().height() - size) / 2;
    
    bool folded = m_folding->isFolded(line);
    
    // 绘制背景框
    painter.setPen(QColor(150, 150, 150));
    painter.setBrush(folded ? QColor(200, 200, 200) : QColor(255, 255, 255));
    painter.drawRect(x, y, size, size);
    
    // 绘制 + 或 - 号
    painter.setPen(QColor(80, 80, 80));
    int centerY = y + size / 2;
    painter.drawLine(x + 2, centerY, x + size - 2, centerY); // 横线
    
    if (!folded) {
        int centerX = x + size / 2;
        painter.drawLine(centerX, y + 2, centerX, y + size - 2); // 竖线（展开时显示）
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
    
    // Ctrl+Shift+F 折叠/展开当前块
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && 
        event->key() == Qt::Key_F) {
        int currentLine = textCursor().blockNumber();
        toggleFold(currentLine);
        return;
    }
    
    // Ctrl+Shift+A 折叠所有
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && 
        event->key() == Qt::Key_A) {
        collapseAll();
        return;
    }
    
    // Ctrl+Shift+E 展开所有
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && 
        event->key() == Qt::Key_E) {
        expandAll();
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
    
    // 如果行尾有 {，增加缩进
    QString trimmedLine = currentLine.trimmed();
    if (trimmedLine.endsWith("{") || 
        trimmedLine.endsWith(":") ||  // Python风格
        trimmedLine.startsWith("if") ||
        trimmedLine.startsWith("for") ||
        trimmedLine.startsWith("while") ||
        trimmedLine.startsWith("switch")) {
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

void CodeEditor::onDocumentChanged()
{
    // 延迟分析折叠区域，避免频繁更新
    if (m_folding) {
        QTimer::singleShot(500, this, [this]() {
            if (m_folding) {
                m_folding->analyzeDocument();
            }
        });
    }
}

// 代码折叠相关函数
void CodeEditor::toggleFold(int line)
{
    if (m_folding) {
        m_folding->toggleFold(line);
    }
}

void CodeEditor::collapseAll()
{
    if (m_folding) {
        m_folding->collapseAll();
    }
}

void CodeEditor::expandAll()
{
    if (m_folding) {
        m_folding->expandAll();
    }
}

bool CodeEditor::isLineFolded(int line) const
{
    return m_folding ? m_folding->isFolded(line) : false;
}

bool CodeEditor::canFoldLine(int line) const
{
    return m_folding ? m_folding->canFoldLine(line) : false;
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
    // 如果有折叠，需要特殊处理绘制
    if (m_folding) {
        // 这里可以添加折叠行的特殊绘制逻辑
        // 例如绘制折叠占位符等
    }
    
    QPlainTextEdit::paintEvent(event);
}

// LineNumberArea 鼠标事件处理
void LineNumberArea::mousePressEvent(QMouseEvent *event)
{
    if (!m_codeEditor || !m_codeEditor->folding()) return;

    int foldIndicatorWidth = m_codeEditor->folding()->foldIndicatorWidth();

    // 检查是否点击了折叠指示器区域
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (event->position().x() <= foldIndicatorWidth + 5) {
#else
    if (event->x() <= foldIndicatorWidth + 5) {
#endif
        // 计算点击的行号
        QTextBlock block = m_codeEditor->getFirstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = qRound(m_codeEditor->getBlockBoundingGeometry(block)
                        .translated(m_codeEditor->getContentOffset()).top());
        int bottom = top + qRound(m_codeEditor->getBlockBoundingRect(block).height());

        while (block.isValid()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if (event->position().y() >= top && event->position().y() < bottom) {
#else
            if (event->y() >= top && event->y() < bottom) {
#endif
                // 检查这一行是否可以折叠
                if (m_codeEditor->canFoldLine(blockNumber)) {
                    m_codeEditor->toggleFold(blockNumber);
                }
                break;
            }

            block = block.next();
            top = bottom;
            bottom = top + qRound(m_codeEditor->getBlockBoundingRect(block).height());
            ++blockNumber;
        }
    }

    QWidget::mousePressEvent(event);
}
