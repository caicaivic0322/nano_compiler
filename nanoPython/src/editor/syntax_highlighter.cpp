#include "syntax_highlighter.h"

#include <QTextDocument>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    m_keywordFormat.setForeground(QColor(0, 119, 170));
    m_keywordFormat.setFontWeight(QFont::Bold);

    m_builtinFormat.setForeground(QColor(0, 119, 170));

    m_functionFormat.setForeground(QColor(0, 119, 170));

    m_stringFormat.setForeground(QColor(221, 68, 85));

    m_numberFormat.setForeground(QColor(0, 153, 153));

    m_commentFormat.setForeground(QColor(153, 153, 136));
    m_commentFormat.setFontItalic(true);

    m_decoratorFormat.setForeground(QColor(153, 0, 85));
    m_decoratorFormat.setFontWeight(QFont::Bold);

    HighlightingRule rule;

    // Python 关键字
    QStringList keywords = {
        "False", "None", "True", "and", "as", "assert", "async", "await",
        "break", "class", "continue", "def", "del", "elif", "else",
        "except", "finally", "for", "from", "global", "if", "import",
        "in", "is", "lambda", "nonlocal", "not", "or", "pass", "raise",
        "return", "try", "while", "with", "yield"
    };

    for (const QString &pattern : keywords) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    // 内置函数
    QStringList builtins = {
        "abs", "all", "any", "bin", "bool", "bytearray", "bytes",
        "chr", "classmethod", "compile", "complex", "delattr", "dict",
        "dir", "divmod", "enumerate", "eval", "exec", "filter", "float",
        "format", "frozenset", "getattr", "globals", "hasattr", "hash",
        "help", "hex", "id", "input", "int", "isinstance", "issubclass",
        "iter", "len", "list", "locals", "map", "max", "memoryview",
        "min", "next", "object", "oct", "open", "ord", "pow", "print",
        "property", "range", "repr", "reversed", "round", "set",
        "setattr", "slice", "sorted", "staticmethod", "str", "sum",
        "super", "tuple", "type", "vars", "zip", "__import__"
    };

    for (const QString &pattern : builtins) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = m_builtinFormat;
        m_highlightingRules.append(rule);
    }

    // 函数定义
    rule.pattern = QRegularExpression("\\bdef\\s+(\\w+)\\b");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);

    // 类定义
    rule.pattern = QRegularExpression("\\bclass\\s+(\\w+)\\b");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);

    // 字符串
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.format = m_stringFormat;
    m_highlightingRules.append(rule);

    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = m_stringFormat;
    m_highlightingRules.append(rule);

    // 数字
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*([eE][+-]?[0-9]+)?\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);

    // 注释
    rule.pattern = QRegularExpression("#[^\\n]*");
    rule.format = m_commentFormat;
    m_highlightingRules.append(rule);

    // 装饰器
    rule.pattern = QRegularExpression("^\\s*@[\\w.]+");
    rule.format = m_decoratorFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : m_highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
