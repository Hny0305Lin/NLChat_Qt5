#ifndef SERIALCLI_H
#define SERIALCLI_H

#include <QObject>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "ch34x_qt.h"

/**
 * @brief 串口命令行接口类
 * 
 * 提供命令行方式操作CH34x设备的功能：
 * - 列出可用设备
 * - 打开/关闭设备
 * - 发送/接收数据
 * - 配置串口参数
 * - 查看设备状态
 */
class SerialCLI : public QObject
{
    Q_OBJECT
public:
    explicit SerialCLI(QObject *parent = nullptr);
    
    /**
     * @brief 处理命令行参数
     * @param parser 命令行解析器
     * @return 是否需要退出程序
     */
    bool handleCommands(QCommandLineParser& parser);
    
    /**
     * @brief 设置命令行选项
     * @param parser 命令行解析器
     */
    void setupOptions(QCommandLineParser& parser);
    
private:
    CH34xQt* m_device;
    
    /**
     * @brief 列出可用设备
     */
    void listPorts();
    
    /**
     * @brief 打开设备
     * @param portName 端口名
     * @param config 配置参数
     */
    void openPort(const QString& portName, const CH34xQt::SerialConfig& config);
    
    /**
     * @brief 发送数据
     * @param data 要发送的数据
     */
    void sendData(const QString& data);
    
    /**
     * @brief 显示设备状态
     */
    void showStatus();
    
private slots:
    void handleReceived(const QByteArray& data);
    void handleError(const QString& error);
};

#endif // SERIALCLI_H 