#ifndef CH34X_QT_H
#define CH34X_QT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QTextCodec>
#include <QThread>

/**
 * @brief 浩瀚银河开源CH34x系列USB转串口芯片的Qt封装类
 *
 * 依赖Qt自有的强大的QSerialPort实现, 封装库只需要保证好正常调用即可
 * 
 * 该类提供了对沁恒厂商CH340/CH341/CH343等系列芯片的基本操作：
 * - 设备的打开与关闭
 * - 数据的收发
 * - 串口参数设置
 * - 设备热插拔检测
 * - 自动缓冲区管理
 * 
 * 高级功能：
 * - 自动重连机制
 * - 数据包分割与合并
 * - 编码自动识别转换
 * - 错误重试机制
 * - 数据统计功能
 */
class CH34xQt : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief 串口配置结构体
     */
    struct SerialConfig {
        QString portName;                    ///< 串口名称
        qint32 baudRate;                    ///< 波特率
        QSerialPort::DataBits dataBits;     ///< 数据位
        QSerialPort::StopBits stopBits;     ///< 停止位
        QSerialPort::Parity parity;         ///< 校验位
        QSerialPort::FlowControl flowControl;///< 流控制
        int readBufferSize;                 ///< 读取缓冲区大小
        int writeBufferSize;                ///< 写入缓冲区大小
        bool autoReconnect;                 ///< 自动重连
        int reconnectInterval;              ///< 重连间隔(ms)
        int maxReconnectAttempts;           ///< 最大重连次数
        int packageTimeout;                 ///< 数据包超时时间(ms)
        bool usePackageMode;                ///< 是否使用数据包模式
        QString packageStart;               ///< 数据包起始标记
        QString packageEnd;                 ///< 数据包结束标记
    };
    
    /**
     * @brief 串口统计信息结构体
     */
    struct Statistics {
        qint64 bytesReceived;              ///< 接收字节数
        qint64 bytesSent;                  ///< 发送字节数
        qint64 packetsReceived;            ///< 接收数据包数
        qint64 packetsSent;                ///< 发送数据包数
        qint64 errors;                     ///< 错误次数
        qint64 reconnects;                 ///< 重连次数
        QDateTime startTime;               ///< 开始时间
        QDateTime lastReceiveTime;         ///< 最后接收时间
        QDateTime lastSendTime;            ///< 最后发送时间
    };

    /**
     * @brief 构造函数，初始化串口对象和定时器
     * @param parent 父对象指针
     */
    explicit CH34xQt(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数，确保关闭设备并释放资源
     */
    ~CH34xQt();
    
    /**
     * @brief 打开指定串口设备
     * @param portName 串口名称（如COM1）
     * @return 是否成功打开设备
     */
    bool openDevice(const QString& portName);
    
    /**
     * @brief 关闭当前打开的设备
     */
    void closeDevice();
    
    /**
     * @brief 发送数据到串口设备
     * @param data 要发送的数据
     * @return 是否成功发送
     */
    bool writeData(const QByteArray& data);
    
    /**
     * @brief 检查设备是否打开
     * @return 设备是否打开
     */
    bool isOpen() const;
    
    /**
     * @brief 获取系统中所有可用的CH34x设备列表
     * @return 可用设备的串口名称列表
     */
    static QStringList availablePorts();
    
    // 串口参数设置函数组
    /**
     * @brief 设置波特率
     * @param baudRate 波特率值
     */
    void setBaudRate(qint32 baudRate);
    
    /**
     * @brief 设置数据位
     * @param dataBits 数据位（5-8位）
     */
    void setDataBits(QSerialPort::DataBits dataBits);
    
    /**
     * @brief 设置停止位
     * @param stopBits 停止位（1、1.5或2位）
     */
    void setStopBits(QSerialPort::StopBits stopBits);
    
    /**
     * @brief 设置校验位
     * @param parity 校验方式（无、奇、偶、标记、空格）
     */
    void setParity(QSerialPort::Parity parity);
    
    /**
     * @brief 设置流控制
     * @param flowControl 流控制方式（无、硬件、软件）
     */
    void setFlowControl(QSerialPort::FlowControl flowControl);
    
    /**
     * @brief 设置读取缓冲区大小
     * @param size 缓冲区大小（字节）
     */
    void setReadBufferSize(qint64 size);
    
    /**
     * @brief 应用串口配置
     * @param config 串口配置结构体
     * @return 是否成功应用配置
     */
    bool applyConfig(const SerialConfig& config);
    
    /**
     * @brief 获取当前配置
     * @return 当前串口配置
     */
    SerialConfig currentConfig() const;
    
    /**
     * @brief 获取统计信息
     * @return 串口统计信息
     */
    Statistics getStatistics() const;
    
    /**
     * @brief 重置统计信息
     */
    void resetStatistics();
    
    /**
     * @brief 设置自动重连
     * @param enable 是否启用
     * @param interval 重连间隔(ms)
     * @param maxAttempts 最大重试次数
     */
    void setAutoReconnect(bool enable, int interval = 5000, int maxAttempts = 3);
    
    /**
     * @brief 设置数据包模式
     * @param enable 是否启用
     * @param start 起始标记
     * @param end 结束标记
     * @param timeout 超时时间(ms)
     */
    void setPackageMode(bool enable, const QString& start = "", 
                       const QString& end = "\n", int timeout = 1000);

signals:
    /**
     * @brief 收到数据信号
     * @param data 接收到的数据
     */
    void dataReceived(const QByteArray& data);
    
    /**
     * @brief 发生错误信号
     * @param error 错误信息
     */
    void errorOccurred(const QString& error);
    
    /**
     * @brief 设备列表变化信号
     * 当插入或移除设备时触发
     */
    void portsChanged();
    
    /**
     * @brief 统计信息更新信号
     * @param stats 最新的统计信息
     */
    void statisticsUpdated(const Statistics& stats);
    
    /**
     * @brief 重连状态信号
     * @param attempt 当前重试次数
     * @param maxAttempts 最大重试次数
     */
    void reconnecting(int attempt, int maxAttempts);
    
    /**
     * @brief 数据包接收超时信号
     */
    void packageTimeout();

private slots:
    /**
     * @brief 处理串口数据可读事件
     * 当有新数据到达时被调用
     */
    void handleReadyRead();
    
    /**
     * @brief 处理串口错误事件
     * @param error 错误类型
     */
    void handleError(QSerialPort::SerialPortError error);
    
    /**
     * @brief 定期检查设备列表变化
     * 由定时器触发，检查设备热插拔
     */
    void checkPorts();
    
private:
    QSerialPort* m_serialPort;      ///< 串口对象指针
    QTimer* m_portCheckTimer;       ///< 端口检查定时器
    QStringList m_lastPorts;        ///< 上次检测到的端口列表
    
    static const int BUFFER_SIZE = 1024 * 1024 * 10;  ///< 接收缓冲区大小（10MB）
    QByteArray m_receiveBuffer;     ///< 数据接收缓冲区
    
    /**
     * @brief 处理接收缓冲区数据
     * 解析和处理接收到的数据包
     */
    void processBuffer();
    
    // CH34x设备识别常量
    static const quint16 CH341_VID = 0x1a86;      ///< WCH厂商ID
    static const quint16 CH341_PID_1 = 0x7523;    ///< CH340芯片产品ID
    static const quint16 CH341_PID_2 = 0x7522;    ///< CH340K/CH343芯片产品ID
    static const quint16 CH341_PID_3 = 0x5523;    ///< CH341芯片产品ID
    static const quint16 CH341_PID_4 = 0xe523;    ///< CH330芯片产品ID
    
    SerialConfig m_config;                ///< 当前配置
    Statistics m_statistics;              ///< 统计信息
    QTimer* m_reconnectTimer;            ///< 重连定时器
    QTimer* m_packageTimer;              ///< 数据包超时定时器
    int m_reconnectAttempts;             ///< 当前重连次数
    
    /**
     * @brief 更新统计信息
     * @param bytesRx 接收字节数
     * @param bytesTx 发送字节数
     * @param packetsRx 接收包数
     * @param packetsTx 发送包数
     */
    void updateStatistics(qint64 bytesRx = 0, qint64 bytesTx = 0,
                         qint64 packetsRx = 0, qint64 packetsTx = 0);
                         
    /**
     * @brief 尝试重新连接
     */
    void tryReconnect();
};

#endif // CH34X_QT_H 
