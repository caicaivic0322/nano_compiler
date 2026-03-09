#include "syntax_highlighter.h"

#include <QTextDocument>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // 设置各种格式
    m_keywordFormat.setForeground(QColor(0, 119, 170)); // 蓝色
    m_keywordFormat.setFontWeight(QFont::Bold);
    
    m_typeFormat.setForeground(QColor(0, 119, 170)); // 蓝色
    m_typeFormat.setFontWeight(QFont::Bold);
    
    m_functionFormat.setForeground(QColor(0, 119, 170)); // 蓝色
    
    m_singleLineCommentFormat.setForeground(QColor(153, 153, 136)); // 灰色
    m_singleLineCommentFormat.setFontItalic(true);
    
    m_multiLineCommentFormat.setForeground(QColor(153, 153, 136)); // 灰色
    m_multiLineCommentFormat.setFontItalic(true);
    
    m_quotationFormat.setForeground(QColor(221, 68, 85)); // 红色
    
    m_numberFormat.setForeground(QColor(0, 153, 153)); // 青色
    
    m_preprocessorFormat.setForeground(QColor(153, 0, 85)); // 紫色
    m_preprocessorFormat.setFontWeight(QFont::Bold);
    
    // 多行注释标记
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
    
    // 默认使用 C++ 规则
    setupCppRules();
}

void SyntaxHighlighter::setLanguage(Language lang)
{
    m_highlightingRules.clear();
    
    if (lang == CLanguage) {
        setupCRules();
    } else {
        setupCppRules();
    }
    
    rehighlight();
}

void SyntaxHighlighter::setupCppRules()
{
    HighlightingRule rule;
    
    // C++ 关键字
    QStringList cppKeywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
        "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr", "constinit",
        "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype",
        "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
        "explicit", "export", "extern", "false", "float", "for", "friend", "goto",
        "if", "inline", "int", "long", "mutable", "namespace", "new", "noexcept",
        "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
        "protected", "public", "register", "reinterpret_cast", "requires", "return",
        "short", "signed", "sizeof", "static", "static_assert", "static_cast",
        "struct", "switch", "template", "this", "thread_local", "throw", "true",
        "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
        "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
    };
    
    for (const QString &pattern : cppKeywords) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // C++ 类型
    QStringList cppTypes = {
        "bool", "char", "double", "float", "int", "long", "short", "signed",
        "unsigned", "void", "wchar_t", "char8_t", "char16_t", "char32_t",
        "size_t", "ssize_t", "ptrdiff_t", "nullptr_t"
    };
    
    for (const QString &pattern : cppTypes) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = m_typeFormat;
        m_highlightingRules.append(rule);
    }
    
    // 类名（首字母大写的标识符）
    rule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format = m_typeFormat;
    m_highlightingRules.append(rule);
    
    // 函数名
    rule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\s*\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);
    
    // 数字
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*([eE][+-]?[0-9]+)?[fFlL]?\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
    
    // 十六进制数字
    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
    
    // 字符串
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);
    
    // 字符
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);
    
    // 预处理器指令
    rule.pattern = QRegularExpression("^\\s*#[a-zA-Z]+\\b");
    rule.format = m_preprocessorFormat;
    m_highlightingRules.append(rule);
    
    // 单行注释
    rule.pattern = QRegularExpression("//[^\\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::setupCRules()
{
    HighlightingRule rule;
    
    // C 关键字
    QStringList cKeywords = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "inline", "int", "long", "register", "restrict", "return", "short",
        "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof",
        "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn",
        "_Static_assert", "_Thread_local"
    };
    
    for (const QString &pattern : cKeywords) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // C 类型
    QStringList cTypes = {
        "char", "double", "float", "int", "long", "short", "signed",
        "unsigned", "void", "size_t", "ssize_t", "ptrdiff_t", "bool",
        "complex", "imaginary"
    };
    
    for (const QString &pattern : cTypes) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = m_typeFormat;
        m_highlightingRules.append(rule);
    }
    
    // 函数名
    rule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\s*\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);
    
    // 数字
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*([eE][+-]?[0-9]+)?[fFlL]?\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
    
    // 十六进制数字
    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
    
    // 字符串
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);
    
    // 字符
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);
    
    // 预处理器指令
    rule.pattern = QRegularExpression("^\\s*#[a-zA-Z]+\\b");
    rule.format = m_preprocessorFormat;
    m_highlightingRules.append(rule);
    
    // 单行注释
    rule.pattern = QRegularExpression("//[^\\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // 应用高亮规则
    for (const HighlightingRule &rule : m_highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    
    // 处理多行注释
    setCurrentBlockState(0);
    
    int startIndex = 0;
    if (previousBlockState() != 1) {
        QRegularExpressionMatch match = m_commentStartExpression.match(text);
        startIndex = match.capturedStart();
    }
    
    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = m_commentEndExpression.match(text, startIndex);
        int endIndex = endMatch.capturedStart();
        int commentLength;
        
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        
        QRegularExpressionMatch nextMatch = m_commentStartExpression.match(text, startIndex + commentLength);
        startIndex = nextMatch.capturedStart();
    }
}
