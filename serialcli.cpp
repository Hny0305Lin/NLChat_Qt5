#include "serialcli.h"
#include <QCoreApplication>
#include <QTextStream>

SerialCLI::SerialCLI(QObject *parent) : QObject(parent)
{
    m_device = new CH34xQt(this);
    
    connect(m_device, &CH34xQt::dataReceived,
            this, &SerialCLI::handleReceived);
    connect(m_device, &CH34xQt::errorOccurred,
            this, &SerialCLI::handleError);
}

void SerialCLI::setupOptions(QCommandLineParser& parser)
{
    parser.setApplicationDescription("NLChat星闪聊天");
    
    // 添加命令行选项
    parser.addHelpOption();
    parser.addVersionOption();
    
    parser.addOptions({
        {{"l", "list"}, "列出可用的CH34x设备"},
        {{"p", "port"}, "指定要操作的串口", "portname"},
        {{"b", "baud"}, "设置波特率", "baudrate", "115200"},
        {{"d", "data"}, "设置数据位 (5-8)", "databits", "8"},
        {{"s", "stop"}, "设置停止位 (1,2)", "stopbits", "1"},
        {{"y", "parity"}, "设置校验位 (none,odd,even)", "parity", "none"},
        {{"f", "flow"}, "设置流控 (none,hard,soft)", "flow", "none"},
        {{"w", "write"}, "发送数据", "data"},
        {{"r", "read"}, "持续读取数据"},
        {"status", "显示设备状态"}
    });
}

bool SerialCLI::handleCommands(QCommandLineParser& parser)
{
    // 列出设备
    if(parser.isSet("list")) {
        listPorts();
        return true;
    }
    
    // 打开设备
    if(parser.isSet("port")) {
        CH34xQt::SerialConfig config;
        config.portName = parser.value("port");
        config.baudRate = parser.value("baud").toInt();
        
        // 设置数据位
        int dataBits = parser.value("data").toInt();
        switch(dataBits) {
            case 5: config.dataBits = QSerialPort::Data5; break;
            case 6: config.dataBits = QSerialPort::Data6; break;
            case 7: config.dataBits = QSerialPort::Data7; break;
            default: config.dataBits = QSerialPort::Data8; break;
        }
        
        // 设置停止位
        if(parser.value("stop") == "2") {
            config.stopBits = QSerialPort::TwoStop;
        } else {
            config.stopBits = QSerialPort::OneStop;
        }
        
        // 设置校验位
        QString parity = parser.value("parity").toLower();
        if(parity == "odd") {
            config.parity = QSerialPort::OddParity;
        } else if(parity == "even") {
            config.parity = QSerialPort::EvenParity;
        } else {
            config.parity = QSerialPort::NoParity;
        }
        
        // 设置流控制
        QString flow = parser.value("flow").toLower();
        if(flow == "hard") {
            config.flowControl = QSerialPort::HardwareControl;
        } else if(flow == "soft") {
            config.flowControl = QSerialPort::SoftwareControl;
        } else {
            config.flowControl = QSerialPort::NoFlowControl;
        }
        
        openPort(config.portName, config);
        
        // 发送数据
        if(parser.isSet("write")) {
            sendData(parser.value("write"));
            return true;
        }
        
        // 持续读取
        if(parser.isSet("read")) {
            return false;  // 保持程序运行
        }
    }
    
    // 显示状态
    if(parser.isSet("status")) {
        showStatus();
        return true;
    }
    
    parser.showHelp();
    return true;
}

void SerialCLI::listPorts()
{
    QTextStream out(stdout);
    QStringList ports = CH34xQt::availablePorts();
    
    if(ports.isEmpty()) {
        out << "未找到CH34x设备\n";
        return;
    }
    
    out << "可用的CH34x设备:\n";
    for(const QString& port : ports) {
        out << "  " << port << "\n";
    }
}

void SerialCLI::openPort(const QString& portName, const CH34xQt::SerialConfig& config)
{
    QTextStream out(stdout);
    
    if(m_device->applyConfig(config)) {
        out << "成功打开设备 " << portName << "\n";
    } else {
        out << "无法打开设备 " << portName << "\n";
        QCoreApplication::exit(1);
    }
}

void SerialCLI::sendData(const QString& data)
{
    QTextStream out(stdout);
    
    if(m_device->writeData(data.toUtf8())) {
        out << "数据发送成功\n";
    } else {
        out << "数据发送失败\n";
    }
}

void SerialCLI::showStatus()
{
    QTextStream out(stdout);
    
    if(!m_device->isOpen()) {
        out << "设备未打开\n";
        return;
    }
    
    CH34xQt::SerialConfig config = m_device->currentConfig();
    CH34xQt::Statistics stats = m_device->getStatistics();
    
    out << "设备状态:\n"
        << "  端口: " << config.portName << "\n"
        << "  波特率: " << config.baudRate << "\n"
        << "  数据位: " << config.dataBits << "\n"
        << "  停止位: " << config.stopBits << "\n"
        << "  校验位: " << config.parity << "\n"
        << "  流控制: " << config.flowControl << "\n"
        << "  已接收: " << stats.bytesReceived << " 字节\n"
        << "  已发送: " << stats.bytesSent << " 字节\n"
        << "  错误数: " << stats.errors << "\n";
}

void SerialCLI::handleReceived(const QByteArray& data)
{
    QTextStream out(stdout);
    out << QString::fromUtf8(data);
    out.flush();
}

void SerialCLI::handleError(const QString& error)
{
    QTextStream err(stderr);
    err << "错误: " << error << "\n";
    err.flush();
} 
