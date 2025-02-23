#include "ch34x_qt.h"
#include <QDateTime>

/**
 * @brief 构造函数实现
 * 初始化串口对象、定时器和缓冲区
 * 设置定时器用于检测设备热插拔
 */
CH34xQt::CH34xQt(QObject *parent) : QObject(parent) {
    // 初始化基本组件
    m_serialPort = new QSerialPort(this);
    m_portCheckTimer = new QTimer(this);
    m_reconnectTimer = new QTimer(this);
    m_packageTimer = new QTimer(this);
    m_receiveBuffer.reserve(BUFFER_SIZE);
    
    // 初始化统计信息
    resetStatistics();
    
    // 初始化配置
    m_config.baudRate = QSerialPort::Baud115200;
    m_config.dataBits = QSerialPort::Data8;
    m_config.stopBits = QSerialPort::OneStop;
    m_config.parity = QSerialPort::NoParity;
    m_config.flowControl = QSerialPort::NoFlowControl;
    m_config.readBufferSize = BUFFER_SIZE;
    m_config.writeBufferSize = BUFFER_SIZE;
    m_config.autoReconnect = false;
    m_config.reconnectInterval = 5000;
    m_config.maxReconnectAttempts = 3;
    m_config.packageTimeout = 1000;
    m_config.usePackageMode = false;
    m_config.packageEnd = "\n";
    
    // 连接信号槽
    connect(m_serialPort, &QSerialPort::readyRead, 
            this, &CH34xQt::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred,
            this, &CH34xQt::handleError);
    connect(m_portCheckTimer, &QTimer::timeout,
            this, &CH34xQt::checkPorts);
    connect(m_reconnectTimer, &QTimer::timeout,
            this, &CH34xQt::tryReconnect);
    connect(m_packageTimer, &QTimer::timeout,
            this, [this]() { emit packageTimeout(); });
            
    m_portCheckTimer->start(2000);
    m_lastPorts = availablePorts();
}

/**
 * @brief 析构函数实现
 * 确保设备正确关闭并释放资源
 */
CH34xQt::~CH34xQt() {
    closeDevice();
    delete m_serialPort;
}

/**
 * @brief 打开指定串口设备的实现
 * 
 * @details
 * 1. 如果已有打开的设备，先关闭
 * 2. 设置串口参数（默认115200 8N1）
 * 3. 清空接收缓冲区
 * 4. 等待设备就绪
 * 5. 尝试打开设备
 * 
 * @param portName 要打开的串口名称
 * @return 是否成功打开设备
 */
bool CH34xQt::openDevice(const QString& portName) {
    if(m_serialPort->isOpen()) {
        closeDevice();
    }
    
    m_serialPort->setPortName(portName);
    
    // 设置默认的串口参数
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    m_receiveBuffer.clear();  // 清空接收缓冲区
    
    QThread::msleep(100);  // 等待设备准备就绪
    
    if(!m_serialPort->open(QIODevice::ReadWrite)) {
        QString errorMsg = tr("无法打开端口 %1: %2\n"
                            "请检查以下可能的原因：\n"
                            "1. 端口是否被其他程序占用\n"
                            "2. 是否有权限访问该端口\n"
                            "3. 设备是否正确连接")
                            .arg(portName)
                            .arg(m_serialPort->errorString());
        emit errorOccurred(errorMsg);
        return false;
    }
    
    return true;
}

/**
 * @brief 关闭当前打开的设备
 * 如果设备已打开则关闭它
 */
void CH34xQt::closeDevice() {
    if(m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

/**
 * @brief 发送数据的实现
 * 
 * @details
 * 1. 检查设备是否打开
 * 2. 确保数据以换行符结束
 * 3. 写入数据并等待完成
 * 
 * @param data 要发送的数据
 * @return 是否成功发送
 */
bool CH34xQt::writeData(const QByteArray& data) {
    if(!m_serialPort->isOpen()) {
        emit errorOccurred(tr("设备未打开"));
        return false;
    }
    
    QByteArray sendData = data;
    if(m_config.usePackageMode) {
        if(!m_config.packageStart.isEmpty()) {
            sendData.prepend(m_config.packageStart.toUtf8());
        }
        if(!m_config.packageEnd.isEmpty()) {
            sendData.append(m_config.packageEnd.toUtf8());
        }
    } else if(!sendData.endsWith('\n')) {
        sendData.append('\n');
    }
    
    qint64 bytesWritten = m_serialPort->write(sendData);
    if(bytesWritten != sendData.size()) {
        if(!m_serialPort->waitForBytesWritten(3000)) {
            emit errorOccurred(tr("数据写入超时"));
            return false;
        }
    }
    
    updateStatistics(0, bytesWritten, 0, 1);
    return true;
}

/**
 * @brief 检查设备是否打开
 * @return 设备打开状态
 */
bool CH34xQt::isOpen() const {
    return m_serialPort->isOpen();
}

/**
 * @brief 获取可用的CH34x设备列表
 * 
 * @details
 * 遍历系统所有串口，根据VID和PID识别CH34x系列设备
 * 支持的设备：
 * - CH340 (0x7523)
 * - CH340K/CH343 (0x7522)
 * - CH341 (0x5523)
 * - CH330 (0xe523)
 * 
 * @return 可用的CH34x设备串口名称列表
 */
QStringList CH34xQt::availablePorts() {
    QStringList result;
    
    for(const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        if(info.hasVendorIdentifier() && info.hasProductIdentifier()) {
            if(info.vendorIdentifier() == CH341_VID && 
               (info.productIdentifier() == CH341_PID_1 ||
                info.productIdentifier() == CH341_PID_2 ||
                info.productIdentifier() == CH341_PID_3 ||
                info.productIdentifier() == CH341_PID_4)) {
                result << info.portName();
            }
        }
    }
    
    return result;
}

/**
 * @brief 处理串口数据可读事件
 * 
 * @details
 * 1. 读取所有可用数据
 * 2. 添加到接收缓冲区
 * 3. 如果缓冲区过大，清理旧数据
 * 4. 处理接收到的数据包
 */
void CH34xQt::handleReadyRead() {
    QByteArray newData = m_serialPort->readAll();
    updateStatistics(newData.size(), 0, 0, 0);
    
    m_receiveBuffer.append(newData);
    
    if(m_receiveBuffer.size() > BUFFER_SIZE) {
        m_receiveBuffer = m_receiveBuffer.right(BUFFER_SIZE / 2);
    }
    
    if(m_config.usePackageMode) {
        m_packageTimer->start(); // 重置超时定时器
    }
    
    processBuffer();
}

/**
 * @brief 处理串口错误事件
 * 
 * @details
 * 处理以下关键错误：
 * - 设备未找到
 * - 权限错误
 * - 打开错误
 * - 设备资源错误（如设备拔出）
 * 
 * @param error 错误类型
 */
void CH34xQt::handleError(QSerialPort::SerialPortError error) {
    if(error == QSerialPort::NoError) {
        return;
    }
    
    switch(error) {
        case QSerialPort::DeviceNotFoundError:
        case QSerialPort::PermissionError:
        case QSerialPort::OpenError:
            emit errorOccurred(tr("串口错误: %1")
                          .arg(m_serialPort->errorString()));
            break;
            
        case QSerialPort::ResourceError:
            closeDevice();
            emit errorOccurred(tr("设备已断开连接"));
            break;
            
        default:
            break;
    }
}

/**
 * @brief 检查设备列表变化
 * 
 * @details
 * 定期检查可用设备列表，如有变化则发出信号
 * 用于实现设备热插拔检测
 */
void CH34xQt::checkPorts() {
    QStringList currentPorts = availablePorts();
    if(currentPorts != m_lastPorts) {
        m_lastPorts = currentPorts;
        emit portsChanged();
    }
}

/**
 * @brief 处理接收缓冲区数据
 * 
 * @details
 * 1. 查找完整的数据包（以换行符分隔）
 * 2. 处理UTF-8编码
 * 3. 发出数据接收信号
 * 
 * 数据包格式：
 * - 以换行符分隔
 * - 支持UTF-8编码
 * - 如果不是UTF-8则尝试本地编码
 */
void CH34xQt::processBuffer() {
    if(m_config.usePackageMode) {
        // 数据包模式处理
        while(!m_receiveBuffer.isEmpty()) {
            int startPos = 0;
            if(!m_config.packageStart.isEmpty()) {
                startPos = m_receiveBuffer.indexOf(m_config.packageStart.toUtf8());
                if(startPos < 0) break;
                startPos += m_config.packageStart.length();
            }
            
            int endPos = m_receiveBuffer.indexOf(m_config.packageEnd.toUtf8(), startPos);
            if(endPos < 0) break;
            
            QByteArray packet = m_receiveBuffer.mid(startPos, endPos - startPos);
            m_receiveBuffer = m_receiveBuffer.mid(endPos + m_config.packageEnd.length());
            
            if(!packet.isEmpty()) {
                emit dataReceived(packet);
                updateStatistics(0, 0, 1, 0);
            }
        }
    } else {
        // 原有的处理逻辑
        static const int MIN_PACKET_SIZE = 1;
        while(m_receiveBuffer.size() >= MIN_PACKET_SIZE) {
            int endPos = m_receiveBuffer.indexOf('\n');
            if(endPos < 0) break;
            
            QByteArray packet = m_receiveBuffer.left(endPos);
            m_receiveBuffer = m_receiveBuffer.mid(endPos + 1);
            packet = packet.trimmed();
            
            if(!packet.isEmpty()) {
                QTextCodec *codec = QTextCodec::codecForName("UTF-8");
                QTextCodec::ConverterState state;
                QString text = codec->toUnicode(packet.constData(), packet.size(), &state);
                
                if(state.invalidChars == 0) {
                    emit dataReceived(packet);
                } else {
                    text = QString::fromLocal8Bit(packet);
                    emit dataReceived(text.toUtf8());
                }
                updateStatistics(0, 0, 1, 0);
            }
        }
    }
}

// 串口参数设置函数组实现
void CH34xQt::setBaudRate(qint32 baudRate)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setBaudRate(baudRate);
    }
}

void CH34xQt::setDataBits(QSerialPort::DataBits dataBits)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setDataBits(dataBits);
    }
}

void CH34xQt::setStopBits(QSerialPort::StopBits stopBits)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setStopBits(stopBits);
    }
}

void CH34xQt::setParity(QSerialPort::Parity parity)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setParity(parity);
    }
}

void CH34xQt::setFlowControl(QSerialPort::FlowControl flowControl)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setFlowControl(flowControl);
    }
}

void CH34xQt::setReadBufferSize(qint64 size)
{
    if(m_serialPort) {
        m_serialPort->setReadBufferSize(size);
    }
}

bool CH34xQt::applyConfig(const SerialConfig& config)
{
    if(m_serialPort->isOpen()) {
        closeDevice();
    }
    
    m_config = config;
    
    if(!config.portName.isEmpty()) {
        m_serialPort->setPortName(config.portName);
        m_serialPort->setBaudRate(config.baudRate);
        m_serialPort->setDataBits(config.dataBits);
        m_serialPort->setStopBits(config.stopBits);
        m_serialPort->setParity(config.parity);
        m_serialPort->setFlowControl(config.flowControl);
        m_serialPort->setReadBufferSize(config.readBufferSize);
        
        setAutoReconnect(config.autoReconnect, 
                        config.reconnectInterval,
                        config.maxReconnectAttempts);
                        
        setPackageMode(config.usePackageMode,
                      config.packageStart,
                      config.packageEnd,
                      config.packageTimeout);
        
        return openDevice(config.portName);
    }
    
    return false;
}

CH34xQt::SerialConfig CH34xQt::currentConfig() const
{
    return m_config;
}

CH34xQt::Statistics CH34xQt::getStatistics() const
{
    return m_statistics;
}

void CH34xQt::resetStatistics()
{
    m_statistics = Statistics();
    m_statistics.startTime = QDateTime::currentDateTime();
    emit statisticsUpdated(m_statistics);
}

void CH34xQt::setAutoReconnect(bool enable, int interval, int maxAttempts)
{
    m_config.autoReconnect = enable;
    m_config.reconnectInterval = interval;
    m_config.maxReconnectAttempts = maxAttempts;
    m_reconnectAttempts = 0;
    
    if(!enable) {
        m_reconnectTimer->stop();
    }
}

void CH34xQt::setPackageMode(bool enable, const QString& start, 
                            const QString& end, int timeout)
{
    m_config.usePackageMode = enable;
    m_config.packageStart = start;
    m_config.packageEnd = end;
    m_config.packageTimeout = timeout;
    
    if(enable) {
        m_packageTimer->setInterval(timeout);
    } else {
        m_packageTimer->stop();
    }
}

void CH34xQt::updateStatistics(qint64 bytesRx, qint64 bytesTx,
                              qint64 packetsRx, qint64 packetsTx)
{
    m_statistics.bytesReceived += bytesRx;
    m_statistics.bytesSent += bytesTx;
    m_statistics.packetsReceived += packetsRx;
    m_statistics.packetsSent += packetsTx;
    
    if(bytesRx > 0) {
        m_statistics.lastReceiveTime = QDateTime::currentDateTime();
    }
    if(bytesTx > 0) {
        m_statistics.lastSendTime = QDateTime::currentDateTime();
    }
    
    emit statisticsUpdated(m_statistics);
}

void CH34xQt::tryReconnect()
{
    if(!m_config.autoReconnect || 
       m_reconnectAttempts >= m_config.maxReconnectAttempts) {
        m_reconnectTimer->stop();
        return;
    }
    
    m_reconnectAttempts++;
    m_statistics.reconnects++;
    
    emit reconnecting(m_reconnectAttempts, m_config.maxReconnectAttempts);
    
    if(openDevice(m_config.portName)) {
        m_reconnectTimer->stop();
        m_reconnectAttempts = 0;
    }
} 
