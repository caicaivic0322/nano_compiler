#include "code_folding.h"
#include "code_editor.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QRegularExpression>
#include <QDebug>

CodeFolding::CodeFolding(CodeEditor *editor)
    : QObject(editor)
    , m_editor(editor)
    , m_updating(false)
{
}

CodeFolding::~CodeFolding() = default;

void CodeFolding::analyzeDocument()
{
    if (m_updating) return;
    m_updating = true;
    
    m_regions.clear();
    m_lineToRegion.clear();
    
    detectPythonBlocks();
    
    m_updating = false;
    emit foldingChanged();
}

void CodeFolding::detectPythonBlocks()
{
    QTextDocument *doc = m_editor->document();
    if (!doc) return;
    
    // Python 使用缩进来定义代码块
    QVector<int> indentStack;
    QVector<int> lineStack;
    
    int prevIndent = 0;
    int blockStartLine = -1;
    
    for (int i = 0; i < doc->blockCount(); ++i) {
        QTextBlock block = doc->findBlockByNumber(i);
        QString text = block.text();
        
        // 跳过空行
        if (text.trimmed().isEmpty()) {
            continue;
        }
        
        // 跳过注释行
        QString trimmed = text.trimmed();
        if (trimmed.startsWith("#")) {
            continue;
        }
        
        // 计算当前行的缩进
        int currentIndent = 0;
        for (QChar c : text) {
            if (c == ' ') {
                currentIndent++;
            } else if (c == '\t') {
                currentIndent += 4;
            } else {
                break;
            }
        }
        
        // 检测代码块开始
        bool isBlockStarter = false;
        if (trimmed.startsWith("def ") ||
            trimmed.startsWith("class ") ||
            trimmed.startsWith("if ") ||
            trimmed.startsWith("elif ") ||
            trimmed.startsWith("else:") ||
            trimmed.startsWith("for ") ||
            trimmed.startsWith("while ") ||
            trimmed.startsWith("try:") ||
            trimmed.startsWith("except") ||
            trimmed.startsWith("finally:") ||
            trimmed.startsWith("with ") ||
            trimmed.startsWith("async def") ||
            trimmed.startsWith("async for") ||
            trimmed.startsWith("async with")) {
            isBlockStarter = true;
        }
        
        // 如果缩进减少，结束之前的代码块
        while (!indentStack.isEmpty() && currentIndent <= indentStack.last()) {
            int startLine = lineStack.takeLast();
            int endLine = i - 1;
            int startIndent = indentStack.takeLast();
            
            // 只保留有意义的折叠区域（至少2行）
            if (endLine > startLine) {
                FoldRegion region(startLine, endLine);
                region.placeholder = createPlaceholder(region);
                m_regions.append(region);
                m_lineToRegion[startLine] = m_regions.size() - 1;
            }
        }
        
        // 如果是代码块开始，记录它
        if (isBlockStarter) {
            indentStack.append(currentIndent);
            lineStack.append(i);
        }
        
        prevIndent = currentIndent;
    }
    
    // 处理未闭合的代码块
    while (!indentStack.isEmpty()) {
        int startLine = lineStack.takeLast();
        int endLine = doc->blockCount() - 1;
        indentStack.takeLast();
        
        if (endLine > startLine) {
            FoldRegion region(startLine, endLine);
            region.placeholder = createPlaceholder(region);
            m_regions.append(region);
            m_lineToRegion[startLine] = m_regions.size() - 1;
        }
    }
    
    // 按起始行排序
    std::sort(m_regions.begin(), m_regions.end(), 
              [](const FoldRegion &a, const FoldRegion &b) {
                  return a.startLine < b.startLine;
              });
}

FoldRegion* CodeFolding::getFoldRegion(int line)
{
    for (int i = 0; i < m_regions.size(); ++i) {
        if (m_regions[i].startLine == line) {
            return &m_regions[i];
        }
    }
    return nullptr;
}

void CodeFolding::toggleFold(int line)
{
    FoldRegion *region = getFoldRegion(line);
    if (!region) return;
    
    region->collapsed = !region->collapsed;
    updateTextDisplay();
    emit foldingChanged();
}

void CodeFolding::collapseAll()
{
    for (auto &region : m_regions) {
        if (region.isValid()) {
            region.collapsed = true;
        }
    }
    updateTextDisplay();
    emit foldingChanged();
}

void CodeFolding::expandAll()
{
    for (auto &region : m_regions) {
        region.collapsed = false;
    }
    updateTextDisplay();
    emit foldingChanged();
}

bool CodeFolding::isLineVisible(int line) const
{
    for (const auto &region : m_regions) {
        if (region.collapsed && region.contains(line) && line > region.startLine) {
            return false;
        }
    }
    return true;
}

bool CodeFolding::canFold(int line) const
{
    for (const auto &region : m_regions) {
        if (region.startLine == line && region.isValid()) {
            return true;
        }
    }
    return false;
}

bool CodeFolding::isFolded(int line) const
{
    for (const auto &region : m_regions) {
        if (region.startLine == line) {
            return region.collapsed;
        }
    }
    return false;
}

void CodeFolding::updateTextDisplay()
{
    // 这里可以通过设置块可见性来实现折叠
    // 由于 QTextDocument 的限制，我们使用另一种方法：
    // 在绘制时跳过被折叠的行
    
    // 触发重绘
    m_editor->viewport()->update();
}

QString CodeFolding::createPlaceholder(const FoldRegion &region)
{
    QTextDocument *doc = m_editor->document();
    if (!doc) return "...";
    
    QTextBlock startBlock = doc->findBlockByNumber(region.startLine);
    QString firstLine = startBlock.text().trimmed();
    
    // 提取关键信息
    if (firstLine.startsWith("def ")) {
        QRegularExpression re("def\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(firstLine);
        if (match.hasMatch()) {
            return "def " + match.captured(1) + "(...)";
        }
    } else if (firstLine.startsWith("class ")) {
        QRegularExpression re("class\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(firstLine);
        if (match.hasMatch()) {
            return "class " + match.captured(1) + "(...)";
        }
    } else if (firstLine.startsWith("if ")) {
        return "if ...";
    } else if (firstLine.startsWith("for ")) {
        return "for ...";
    } else if (firstLine.startsWith("while ")) {
        return "while ...";
    } else if (firstLine.startsWith("try:")) {
        return "try ...";
    } else if (firstLine.startsWith("with ")) {
        return "with ...";
    }
    
    return "...";
}
