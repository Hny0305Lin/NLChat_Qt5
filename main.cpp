#include "nlchatwindow.h"
#include "serialcli.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    // 首先创建命令行解析器
    QCommandLineParser parser;
    parser.addOption({{"g", "gui"}, "启动图形界面模式"});
    
    // 检查是否有 -g 或 --gui 参数
    bool hasGuiArguments = false;
    for(int i = 1; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if(arg == "--gui" || arg == "-g") {
            hasGuiArguments = true;
            break;
        }
    }
    
    // 根据参数选择应用程序类型
    QCoreApplication* app;
    if(hasGuiArguments) {
        app = new QApplication(argc, argv);
        
        // 加载鸿蒙字体
        QFontDatabase::addApplicationFont(":/fonts/fonts/HarmonyOS_Sans_SC_Regular.ttf");
        QFontDatabase::addApplicationFont(":/fonts/fonts/HarmonyOS_Sans_SC_Medium.ttf");
        QFontDatabase::addApplicationFont(":/fonts/fonts/HarmonyOS_Sans_SC_Bold.ttf");
        
        // 设置默认字体
        QFont defaultFont("HarmonyOS Sans SC", 10);
        QApplication::setFont(defaultFont);
    } else {
        app = new QCoreApplication(argc, argv);
    }
    
    app->setApplicationName("NLChatQt");
    app->setApplicationVersion("1.0.0");
    
    // 设置命令行选项
    SerialCLI cli;
    cli.setupOptions(parser);
    
    // 处理命令行参数
    parser.process(*app);
    
    if(hasGuiArguments) {
        // GUI模式
        NLChatWindow w;
        w.show();
        return app->exec();
    } else {
        // CLI模式
        if(cli.handleCommands(parser)) {
            return 0;  // 命令已执行完毕
        }
        return app->exec();  // 持续运行（如读取模式）
    }
}
