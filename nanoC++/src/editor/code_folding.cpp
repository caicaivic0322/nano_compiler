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
    
    detectCppBlocks();
    
    m_updating = false;
    emit foldingChanged();
}

void CodeFolding::detectCppBlocks()
{
    QTextDocument *doc = m_editor->document();
    if (!doc) return;
    
    QVector<int> braceStack;
    QVector<int> parenStack;
    
    // 第一遍：找到所有 { 和 } 的位置
    for (int i = 0; i < doc->blockCount(); ++i) {
        QTextBlock block = doc->findBlockByNumber(i);
        QString text = block.text();
        
        // 跳过空行和注释行（简单处理）
        QString trimmed = text.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith("//")) {
            continue;
        }
        
        // 检测函数定义、类定义、命名空间等
        bool isBlockStart = false;
        
        // 检测函数定义
        if (trimmed.contains(QRegularExpression("^\\s*(void|int|char|float|double|bool|auto|\\w+_t|std::|vector|map|set|string)\\s+\\w+\\s*\\(")) ||
            trimmed.contains(QRegularExpression("^\\s*\\w+\\s*\\w+\\s*\\(")) ||
            trimmed.contains(QRegularExpression("^\\s*main\\s*\\("))) {
            isBlockStart = true;
        }
        
        // 检测类/结构体/命名空间
        if (trimmed.startsWith("class ") || 
            trimmed.startsWith("struct ") || 
            trimmed.startsWith("namespace ") ||
            trimmed.startsWith("enum ") ||
            trimmed.startsWith("union ")) {
            isBlockStart = true;
        }
        
        // 检测控制语句
        if (trimmed.startsWith("if ") || 
            trimmed.startsWith("else if ") ||
            trimmed.startsWith("else ") ||
            trimmed.startsWith("for ") ||
            trimmed.startsWith("while ") ||
            trimmed.startsWith("do ") ||
            trimmed.startsWith("switch ")) {
            isBlockStart = true;
        }
        
        // 查找行内的 {
        int bracePos = text.indexOf('{');
        while (bracePos != -1) {
            // 检查这个 { 是否在字符串或注释中
            bool inString = false;
            bool inChar = false;
            bool inComment = false;
            
            for (int j = 0; j < bracePos; ++j) {
                QChar c = text[j];
                if (j > 0 && text[j-1] == '\\') continue;
                
                if (c == '"' && !inChar) inString = !inString;
                else if (c == '\'' && !inString) inChar = !inChar;
                else if (j < text.length() - 1 && c == '/' && text[j+1] == '/') {
                    inComment = true;
                    break;
                }
            }
            
            if (!inString && !inChar && !inComment) {
                braceStack.append(i);
            }
            
            bracePos = text.indexOf('{', bracePos + 1);
        }
        
        // 查找行内的 }
        int closeBracePos = text.indexOf('}');
        while (closeBracePos != -1) {
            // 检查是否在字符串或注释中
            bool inString = false;
            bool inChar = false;
            bool inComment = false;
            
            for (int j = 0; j < closeBracePos; ++j) {
                QChar c = text[j];
                if (j > 0 && text[j-1] == '\\') continue;
                
                if (c == '"' && !inChar) inString = !inString;
                else if (c == '\'' && !inString) inChar = !inChar;
                else if (j < text.length() - 1 && c == '/' && text[j+1] == '/') {
                    inComment = true;
                    break;
                }
            }
            
            if (!inString && !inChar && !inComment) {
                if (!braceStack.isEmpty()) {
                    int startLine = braceStack.takeLast();
                    // 只保留有意义的折叠区域（至少2行）
                    if (i - startLine >= 1) {
                        FoldRegion region(startLine, i);
                        region.placeholder = createPlaceholder(region);
                        m_regions.append(region);
                        m_lineToRegion[startLine] = m_regions.size() - 1;
                    }
                }
            }
            
            closeBracePos = text.indexOf('}', closeBracePos + 1);
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
    if (!doc) return "{...}";
    
    QTextBlock startBlock = doc->findBlockByNumber(region.startLine);
    QString firstLine = startBlock.text().trimmed();
    
    // 提取关键信息
    if (firstLine.contains("class ")) {
        QRegularExpression re("class\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(firstLine);
        if (match.hasMatch()) {
            return "class " + match.captured(1) + " {...}";
        }
    } else if (firstLine.contains("struct ")) {
        QRegularExpression re("struct\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(firstLine);
        if (match.hasMatch()) {
            return "struct " + match.captured(1) + " {...}";
        }
    } else if (firstLine.contains("namespace ")) {
        QRegularExpression re("namespace\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(firstLine);
        if (match.hasMatch()) {
            return "namespace " + match.captured(1) + " {...}";
        }
    } else if (firstLine.contains("(")) {
        // 函数
        QRegularExpression re("(\\w+)\\s*\\(");
        QRegularExpressionMatch match = re.match(firstLine);
        if (match.hasMatch()) {
            return match.captured(1) + "(...) {...}";
        }
    }
    
    return "{...}";
}
