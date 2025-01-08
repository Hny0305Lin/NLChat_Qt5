#ifndef CH341_QT_H
#define CH341_QT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QTextCodec>
#include <QThread>

class CH341Qt : public QObject {
    Q_OBJECT
    
public:
    explicit CH341Qt(QObject *parent = nullptr);
    ~CH341Qt();
    
    bool openDevice(const QString& portName);
    void closeDevice();
    bool writeData(const QByteArray& data);
    bool isOpen() const;
    
    static QStringList availablePorts();
    
    void setBaudRate(qint32 baudRate);
    void setDataBits(QSerialPort::DataBits dataBits);
    void setStopBits(QSerialPort::StopBits stopBits);
    void setParity(QSerialPort::Parity parity);
    void setFlowControl(QSerialPort::FlowControl flowControl);
    void setReadBufferSize(qint64 size);
    
signals:
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& error);
    void portsChanged();
    
private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void checkPorts();
    
private:
    QSerialPort* m_serialPort;
    QTimer* m_portCheckTimer;
    QStringList m_lastPorts;
    
    static const int BUFFER_SIZE = 1024 * 1024 * 10;  // 10MB缓冲区
    QByteArray m_receiveBuffer;
    
    void processBuffer();
    
    // CH341设备识别常量
    static const quint16 CH341_VID = 0x1a86;
    static const quint16 CH341_PID_1 = 0x7523;  // CH340
    static const quint16 CH341_PID_2 = 0x7522;  // CH340K/CH343
    static const quint16 CH341_PID_3 = 0x5523;  // CH341
    static const quint16 CH341_PID_4 = 0xe523;  // CH330
};

#endif // CH341_QT_H 