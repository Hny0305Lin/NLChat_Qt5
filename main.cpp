#include "nlchatwindow.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 加载鸿蒙字体
    QFontDatabase::addApplicationFont(":/fonts/fonts/HarmonyOS_Sans_SC_Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/HarmonyOS_Sans_SC_Medium.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/HarmonyOS_Sans_SC_Bold.ttf");
    
    // 设置默认字体
    QFont defaultFont("HarmonyOS Sans SC", 10);
    QApplication::setFont(defaultFont);
    
    NLChatWindow w;
    w.show();
    return a.exec();
}
