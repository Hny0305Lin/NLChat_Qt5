#include "nlchatwindow.h"
#include "ui_nlchatwindow.h"
#include "chatbubblewidget.h"
#include "messagelistwindow.h"
#include "uilayoutmanager.h"

NLChatWindow::NLChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NLChatWindow)
{
    ui->setupUi(this);
    setupUi();
    initializeSerialManager();
}

NLChatWindow::~NLChatWindow()
{
    if(m_serialManager) {
        m_serialManager->closePort();
        delete m_serialManager;
    }
    delete ui;
}

void NLChatWindow::setupUi()
{
    // 设置全局字体
    QFont font("HarmonyOS Sans SC", 10);
    QApplication::setFont(font);
    
    // 创建控件
    m_settingsButton = new QPushButton(tr("设置"), this);
    m_settingsButton->setObjectName("settingsButton");
    m_portList = new QComboBox(this);
    m_portList->setMinimumWidth(180);
    m_connectButton = new QPushButton(tr("连接"), this);
    m_refreshButton = new QPushButton(tr("刷新"), this);
    
    m_chatDisplay = new ChatBubbleWidget(this);
    m_chatDisplay->setObjectName("chatDisplay");
    
    m_messageInput = new QTextEdit(this);
    m_messageInput->setMaximumHeight(50);
    m_messageInput->setMinimumHeight(50);
    m_messageInput->setPlaceholderText(tr("请先连接设备..."));
    m_messageInput->setObjectName("messageInput");
    m_messageInput->setEnabled(false);
    m_messageInput->installEventFilter(this);
    
    m_sendButton = new QPushButton(tr("发送"), this);
    m_sendButton->setFixedSize(80, 50);
    m_sendButton->setObjectName("sendButton");
    m_sendButton->setEnabled(false);
    
    m_messageList = new MessageListWindow(this);
    m_messageList->hide();
    
    // 使用布局管理器设置界面
    UILayoutManager::setupMainWindowLayout(
        this, m_portList, m_connectButton, m_refreshButton,
        m_settingsButton, m_chatDisplay, m_messageInput,
        m_sendButton, m_messageList
    );
    
    // 设置窗口属性
    setMinimumSize(900, 650);     // 最小尺寸限制
    resize(1280, 960);            // 设置默认尺寸为1280*960
    
    // 连接信号槽
    connect(m_connectButton, &QPushButton::clicked, this, &NLChatWindow::handleConnectButton);
    connect(m_refreshButton, &QPushButton::clicked, this, &NLChatWindow::refreshPortList);
    connect(m_sendButton, &QPushButton::clicked, this, &NLChatWindow::handleSendButton);
    connect(m_messageInput, &QTextEdit::textChanged, this, [this]() {
        if(m_messageInput->document()->size().height() > 70) {
            m_messageInput->setMaximumHeight(70);
        }
    });
    connect(m_settingsButton, &QPushButton::clicked, this, &NLChatWindow::handleSettingsButton);
}

void NLChatWindow::initializeSerialManager()
{
    m_serialManager = new SerialManager(this);
    
    connect(m_serialManager, &SerialManager::messageReceived, 
            this, &NLChatWindow::handleMessage);
    connect(m_serialManager, &SerialManager::errorOccurred,
            this, &NLChatWindow::handleError);
    connect(m_serialManager, &SerialManager::portsChanged,
            this, &NLChatWindow::handlePortsChanged);
    connect(m_serialManager, &SerialManager::connectionStatusChanged,
            this, &NLChatWindow::handleConnectionStatus);
            
    refreshPortList();
}

void NLChatWindow::handleConnectButton()
{
    if(!m_serialManager->isOpen()) {
        QString selectedPort = m_portList->currentText();
        if(selectedPort.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), 
                               tr("未选择端口"));
            return;
        }
        
        if(m_serialManager->openPort(selectedPort)) {
            appendSystemMessage(tr("已连接到 %1").arg(selectedPort));
            m_messageList->setCurrentPort(selectedPort);
            m_messageList->show();  // 连接成功时显示消息列表
        }
    } else {
        m_serialManager->closePort();
        appendSystemMessage(tr("已断开连接"));
        m_messageList->hide();  // 断开连接时隐藏消息列表
    }
}

void NLChatWindow::handleConnectionStatus(bool connected)
{
    m_connectButton->setText(connected ? tr("断开") : tr("连接"));
    m_portList->setEnabled(!connected);
    m_refreshButton->setEnabled(!connected);
    
    // 添加输入框和发送按钮的启用/禁用控制
    m_messageInput->setEnabled(connected);
    m_sendButton->setEnabled(connected);
    
    // 设置输入框的提示文本
    m_messageInput->setPlaceholderText(connected ? 
        tr("请输入消息...") : 
        tr("请先连接设备..."));
}

void NLChatWindow::handleSendButton()
{
    if(!m_serialManager->isOpen()) {
        QMessageBox::warning(this, tr("错误"),
                           tr("设备未连接"));
        return;
    }
    
    QString message = m_messageInput->toPlainText().trimmed();
    if(message.isEmpty()) {
        return;
    }
    
    if(m_serialManager->sendData(message)) {
        appendMessage(message, true);
        m_messageInput->clear();
    }
}

void NLChatWindow::handleMessage(const QString& message)
{
    appendMessage(message, false);
}

void NLChatWindow::handleError(const QString& error)
{
    QMessageBox::critical(this, tr("串口错误"), error);
}

void NLChatWindow::refreshPortList()
{
    m_portList->clear();
    m_portList->addItems(m_serialManager->getAvailablePorts());
}

void NLChatWindow::appendMessage(const QString& message, bool isFromMe)
{
    m_chatDisplay->addMessage(message, isFromMe);
    m_messageList->addMessage(m_portList->currentText(), message, isFromMe);
}

void NLChatWindow::handlePortsChanged()
{
    QString currentPort = m_portList->currentText();
    refreshPortList();
    
    // 保持之前选择的端口
    int index = m_portList->findText(currentPort);
    if(index >= 0) {
        m_portList->setCurrentIndex(index);
    }
}

void NLChatWindow::handleSettingsButton()
{
    SerialSettingsDialog dialog(this);
    dialog.setSettings(m_serialSettings);
    
    if(dialog.exec() == QDialog::Accepted) {
        m_serialSettings = dialog.getSettings();
        // 应用自动滚动设置
        m_chatDisplay->setAutoScroll(m_serialSettings.autoScroll);
        // 如果串口已经打开，应用新设置
        if(m_serialManager->isOpen()) {
            m_serialManager->applySettings(m_serialSettings);
        }
    }
}

void NLChatWindow::appendSystemMessage(const QString& message)
{
    m_chatDisplay->addSystemMessage(message);
}

// 添加事件过滤器处理函数
bool NLChatWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 处理输入框事件
    if (obj == m_messageInput) {
        if (event->type() == QEvent::MouseButtonPress && !m_messageInput->isEnabled()) {
            // 如果输入框被禁用且用户点击了输入框
            QMessageBox::information(this, tr("提示"),
                                   tr("请先连接设备"));
            return true;
        }
        
        if (event->type() == QEvent::KeyPress) {
            if (!m_messageInput->isEnabled()) {
                // 如果输入框被禁用且用户试图输入
                QMessageBox::information(this, tr("提示"),
                                       tr("请先连接设备"));
                return true;
            }
            
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                // 如果按下的是回车键且没有按住Shift键
                if (!(keyEvent->modifiers() & Qt::ShiftModifier)) {
                    QString message = m_messageInput->toPlainText().trimmed();
                    if (message.isEmpty()) {
                        QMessageBox::information(this, tr("提示"),
                                              tr("您没有填写好噢"));
                    } else {
                        handleSendButton();  // 发送消息
                    }
                    return true;  // 事件已处理
                }
            }
        }
    }
    
    // 处理标题栏拖动
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragPosition = mouseEvent->globalPos() - frameGeometry().topLeft();
            event->accept();
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->buttons() & Qt::LeftButton) {
            move(mouseEvent->globalPos() - m_dragPosition);
            event->accept();
            return true;
        }
    }
    
    return QMainWindow::eventFilter(obj, event);
}

