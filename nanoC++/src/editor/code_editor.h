#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QTextBlock>

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
    
    void setFileType(const QString &extension);
    
    // 代码折叠相关
    void toggleFold(int line);
    void collapseAll();
    void expandAll();
    bool isLineFolded(int line) const;
    bool canFoldLine(int line) const;
    CodeFolding* folding() const { return m_folding; }
    
    // 用于 LineNumberArea 访问的公共接口
    QTextBlock getFirstVisibleBlock() const { return firstVisibleBlock(); }
    QRectF getBlockBoundingGeometry(const QTextBlock &block) const { return blockBoundingGeometry(block); }
    QPointF getContentOffset() const { return contentOffset(); }
    QRectF getBlockBoundingRect(const QTextBlock &block) const { return blockBoundingRect(block); }

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
    void updateHighlighter();
    void autoIndent();
    void decreaseIndent();
    bool handleBracketMatching(QKeyEvent *event);
    void insertCompletion(const QString &completion);
    
    // 绘制折叠指示器
    void drawFoldIndicator(QPainter &painter, int line, int top);

    QWidget *m_lineNumberArea;
    SyntaxHighlighter *m_highlighter;
    CodeFolding *m_folding;
    QString m_fileExtension;
};

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    CodeEditor *m_codeEditor;
};

#endif // CODE_EDITOR_H
