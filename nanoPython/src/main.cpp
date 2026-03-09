#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QTextStream>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setApplicationName("nanoPython");
    app.setApplicationDisplayName("nanoPython");
    app.setOrganizationName("nanoCompiler");
    app.setApplicationVersion("1.0.0");
    
    // 设置应用样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 加载样式表
    QFile styleFile(":/styles/default.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QTextStream stream(&styleFile);
        app.setStyleSheet(stream.readAll());
        styleFile.close();
    }
    
    // 创建并显示主窗口
    MainWindow window;
    window.show();
    
    return app.exec();
}
