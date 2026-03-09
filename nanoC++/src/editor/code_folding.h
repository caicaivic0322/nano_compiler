#ifndef CODE_FOLDING_H
#define CODE_FOLDING_H

#include <QObject>
#include <QTextBlock>
#include <QVector>
#include <QMap>

class CodeEditor;

struct FoldRegion {
    int startLine;
    int endLine;
    bool collapsed;
    QString placeholder;
    
    FoldRegion(int start = 0, int end = 0) 
        : startLine(start), endLine(end), collapsed(false) {}
    
    bool contains(int line) const {
        return line >= startLine && line <= endLine;
    }
    
    bool isValid() const {
        return startLine < endLine;
    }
};

class CodeFolding : public QObject
{
    Q_OBJECT

public:
    explicit CodeFolding(CodeEditor *editor);
    ~CodeFolding();

    // 分析代码并更新折叠区域
    void analyzeDocument();
    
    // 获取指定行的折叠区域
    FoldRegion* getFoldRegion(int line);
    
    // 切换折叠状态
    void toggleFold(int line);
    
    // 折叠/展开所有
    void collapseAll();
    void expandAll();
    
    // 检查行是否被折叠
    bool isLineVisible(int line) const;
    
    // 获取折叠区域列表
    QVector<FoldRegion> getFoldRegions() const { return m_regions; }
    
    // 获取折叠指示器宽度
    int foldIndicatorWidth() const { return 15; }
    
    // 检查某行是否可以折叠
    bool canFold(int line) const;
    bool canFoldLine(int line) const { return canFold(line); }
    
    // 获取行对应的折叠状态
    bool isFolded(int line) const;

signals:
    void foldingChanged();

private:
    // 检测 C/C++ 代码块
    void detectCppBlocks();
    
    // 检测代码块边界
    void findBlockBoundaries();
    
    // 更新文本显示
    void updateTextDisplay();
    
    // 创建折叠占位符
    QString createPlaceholder(const FoldRegion &region);

    CodeEditor *m_editor;
    QVector<FoldRegion> m_regions;
    QMap<int, int> m_lineToRegion; // 行号到区域索引的映射
    bool m_updating;
};

#endif // CODE_FOLDING_H
