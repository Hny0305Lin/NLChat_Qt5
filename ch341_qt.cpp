#include "ch341_qt.h"

CH341Qt::CH341Qt(QObject *parent) : QObject(parent) {
    m_serialPort = new QSerialPort(this);
    m_portCheckTimer = new QTimer(this);
    m_receiveBuffer.reserve(BUFFER_SIZE);
    
    connect(m_serialPort, &QSerialPort::readyRead, 
            this, &CH341Qt::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred,
            this, &CH341Qt::handleError);
    connect(m_portCheckTimer, &QTimer::timeout,
            this, &CH341Qt::checkPorts);
            
    m_portCheckTimer->start(2000); // 每2秒检查一次
    m_lastPorts = availablePorts();
}

CH341Qt::~CH341Qt() {
    closeDevice();
    delete m_serialPort;
}

bool CH341Qt::openDevice(const QString& portName) {
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
    
    // 清空接收缓冲区
    m_receiveBuffer.clear();
    
    // 添加延时，等待设备准备就绪
    QThread::msleep(100);
    
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

void CH341Qt::closeDevice() {
    if(m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

bool CH341Qt::writeData(const QByteArray& data) {
    if(!m_serialPort->isOpen()) {
        emit errorOccurred(tr("设备未打开"));
        return false;
    }
    
    // 确保发送的数据以换行符结束
    QByteArray sendData = data;
    if(!sendData.endsWith('\n')) {
        sendData.append('\n');
    }
    
    qint64 bytesWritten = m_serialPort->write(sendData);
    if(bytesWritten != sendData.size()) {
        // 等待数据写入完成
        if(!m_serialPort->waitForBytesWritten(3000)) {
            emit errorOccurred(tr("数据写入超时"));
            return false;
        }
    }
    
    return true;
}

bool CH341Qt::isOpen() const {
    return m_serialPort->isOpen();
}

QStringList CH341Qt::availablePorts() {
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

void CH341Qt::handleReadyRead() {
    // 读取所有可用数据
    QByteArray newData = m_serialPort->readAll();
    
    // 将新数据添加到缓冲区
    m_receiveBuffer.append(newData);
    
    // 如果缓冲区超过大小限制，清理旧数据
    if(m_receiveBuffer.size() > BUFFER_SIZE) {
        m_receiveBuffer = m_receiveBuffer.right(BUFFER_SIZE / 2);
    }
    
    // 处理完整的数据包
    processBuffer();
}

void CH341Qt::handleError(QSerialPort::SerialPortError error) {
    if(error == QSerialPort::NoError) {
        return;
    }
    
    // 只处理重要的错误
    switch(error) {
        case QSerialPort::DeviceNotFoundError:
        case QSerialPort::PermissionError:
        case QSerialPort::OpenError:
            emit errorOccurred(tr("串口错误: %1")
                          .arg(m_serialPort->errorString()));
            break;
            
        case QSerialPort::ResourceError:
            // 设备被拔出
            closeDevice();
            emit errorOccurred(tr("设备已断开连接"));
            break;
            
        default:
            // 忽略其他类型的错误
            break;
    }
}

void CH341Qt::checkPorts() {
    QStringList currentPorts = availablePorts();
    if(currentPorts != m_lastPorts) {
        m_lastPorts = currentPorts;
        emit portsChanged();
    }
}

void CH341Qt::processBuffer() {
    // 等待接收完整的数据包
    static const int MIN_PACKET_SIZE = 1;  // 最小数据包大小
    
    while(m_receiveBuffer.size() >= MIN_PACKET_SIZE) {
        // 查找数据包结束标记
        int endPos = m_receiveBuffer.indexOf('\n');
        if(endPos < 0) {
            // 没有找到完整的数据包，继续等待
            break;
        }
        
        // 提取一个完整的数据包
        QByteArray packet = m_receiveBuffer.left(endPos);
        m_receiveBuffer = m_receiveBuffer.mid(endPos + 1);
        
        // 移除可能的回车符
        packet = packet.trimmed();
        
        if(!packet.isEmpty()) {
            // 确保数据是UTF-8编码
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            QTextCodec::ConverterState state;
            QString text = codec->toUnicode(packet.constData(), packet.size(), &state);
            
            if(state.invalidChars == 0) {
                // 数据是有效的UTF-8
                emit dataReceived(packet);
            } else {
                // 数据不是UTF-8，尝试转换
                text = QString::fromLocal8Bit(packet);
                emit dataReceived(text.toUtf8());
            }
        }
    }
}

void CH341Qt::setBaudRate(qint32 baudRate)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setBaudRate(baudRate);
    }
}

void CH341Qt::setDataBits(QSerialPort::DataBits dataBits)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setDataBits(dataBits);
    }
}

void CH341Qt::setStopBits(QSerialPort::StopBits stopBits)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setStopBits(stopBits);
    }
}

void CH341Qt::setParity(QSerialPort::Parity parity)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setParity(parity);
    }
}

void CH341Qt::setFlowControl(QSerialPort::FlowControl flowControl)
{
    if(m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setFlowControl(flowControl);
    }
}

void CH341Qt::setReadBufferSize(qint64 size)
{
    if(m_serialPort) {
        m_serialPort->setReadBufferSize(size);
    }
} 