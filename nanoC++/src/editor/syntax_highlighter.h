#ifndef SYNTAX_HIGHLIGHTER_H
#define SYNTAX_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum Language {
        CLanguage,
        CppLanguage
    };

    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);

    void setLanguage(Language lang);

protected:
    void highlightBlock(const QString &text) override;

private:
    void setupCppRules();
    void setupCRules();
    
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    
    QVector<HighlightingRule> m_highlightingRules;
    
    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;
    
    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_singleLineCommentFormat;
    QTextCharFormat m_multiLineCommentFormat;
    QTextCharFormat m_quotationFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_preprocessorFormat;
};

#endif // SYNTAX_HIGHLIGHTER_H
