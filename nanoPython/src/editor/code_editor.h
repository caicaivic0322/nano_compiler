#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QPlainTextEdit>
#include <QObject>

class SyntaxHighlighter;
class CodeFolding;
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    // 代码折叠相关
    void toggleFold(int line);
    void collapseAll();
    void expandAll();
    bool isLineFolded(int line) const;
    bool canFoldLine(int line) const;
    CodeFolding* folding() const { return m_folding; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void onTextChanged();
    void onDocumentChanged();

private:
    void setupEditor();
    void autoIndent();
    void decreaseIndent();
    bool handleBracketMatching(QKeyEvent *event);

    // 绘制折叠指示器
    void drawFoldIndicator(QPainter &painter, int line, int top);

    QWidget *m_lineNumberArea;
    SyntaxHighlighter *m_highlighter;
    CodeFolding *m_folding;
};

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), m_codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        m_codeEditor->lineNumberAreaPaintEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override;

private:
    CodeEditor *m_codeEditor;
};

#endif // CODE_EDITOR_H
