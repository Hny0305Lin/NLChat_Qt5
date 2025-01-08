#include "uilayoutmanager.h"

void UILayoutManager::setupMainWindowLayout(QMainWindow* mainWindow,
                                          QComboBox* portList,
                                          QPushButton* connectButton,
                                          QPushButton* refreshButton,
                                          QPushButton* settingsButton,
                                          ChatBubbleWidget* chatDisplay,
                                          QTextEdit* messageInput,
                                          QPushButton* sendButton,
                                          MessageListWindow* messageList)
{
    // 设置无边框窗口
    mainWindow->setWindowFlags(Qt::FramelessWindowHint);
    
    // 创建主窗口布局
    QWidget* centralWidget = new QWidget(mainWindow);
    QVBoxLayout* mainVLayout = new QVBoxLayout(centralWidget);
    mainVLayout->setSpacing(0);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建标题栏
    QWidget* titleBar = createTitleBar(mainWindow, "星闪聊天Qt Version");
    mainVLayout->addWidget(titleBar);
    
    // 创建内容区域
    QWidget* contentWidget = new QWidget(mainWindow);
    QHBoxLayout* contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建左侧聊天区域容器
    QWidget* chatWidget = new QWidget(mainWindow);
    QVBoxLayout* chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setSpacing(0);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建各个区域
    QWidget* toolbar = createToolbar(portList, connectButton, refreshButton, settingsButton);
    QWidget* inputArea = createInputArea(messageInput, sendButton);
    QWidget* chatArea = createChatArea(toolbar, chatDisplay, inputArea);
    
    chatLayout->addWidget(chatArea);
    
    contentLayout->addWidget(chatWidget);
    contentLayout->addWidget(messageList);
    
    mainVLayout->addWidget(contentWidget);
    mainWindow->setCentralWidget(centralWidget);
    
    // 应用样式
    applyStyleSheet(mainWindow);
}

QWidget* UILayoutManager::createToolbar(QComboBox* portList,
                                      QPushButton* connectButton,
                                      QPushButton* refreshButton,
                                      QPushButton* settingsButton)
{
    QWidget* toolbar = new QWidget;
    toolbar->setObjectName("toolbarWidget");
    
    QHBoxLayout* layout = new QHBoxLayout(toolbar);
    layout->addWidget(settingsButton);
    layout->addWidget(portList);
    layout->addWidget(connectButton);
    layout->addWidget(refreshButton);
    layout->addStretch();
    layout->setContentsMargins(20, 10, 20, 10);
    
    return toolbar;
}

QWidget* UILayoutManager::createInputArea(QTextEdit* messageInput,
                                        QPushButton* sendButton)
{
    QWidget* inputContainer = new QWidget;
    inputContainer->setObjectName("inputContainer");
    
    QHBoxLayout* layout = new QHBoxLayout(inputContainer);
    layout->addWidget(messageInput);
    layout->addWidget(sendButton);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    
    return inputContainer;
}

QWidget* UILayoutManager::createChatArea(QWidget* toolbar,
                                       ChatBubbleWidget* chatDisplay,
                                       QWidget* inputArea)
{
    QWidget* chatContainer = new QWidget;
    chatContainer->setObjectName("chatContainer");
    
    QVBoxLayout* layout = new QVBoxLayout(chatContainer);
    layout->addWidget(toolbar);
    layout->addWidget(chatDisplay);
    layout->addWidget(inputArea);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    
    return chatContainer;
}

QWidget* UILayoutManager::createTitleBar(QMainWindow* mainWindow, const QString& title)
{
    QWidget* titleBar = new QWidget;
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(40);
    
    QHBoxLayout* layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(10, 0, 10, 0);
    
    // 添加图标（如果有的话）
    // QLabel* iconLabel = new QLabel;
    // iconLabel->setPixmap(QPixmap(":/icon.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // layout->addWidget(iconLabel);
    
    // 添加标题
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setObjectName("titleLabel");
    layout->addWidget(titleLabel);
    
    // 添加窗口按钮
    setupWindowButtons(titleBar, mainWindow);
    
    // 使标题栏可拖动
    titleBar->installEventFilter(mainWindow);
    
    return titleBar;
}

void UILayoutManager::setupWindowButtons(QWidget* titleBar, QMainWindow* mainWindow)
{
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(titleBar->layout());
    
    // 最小化按钮
    QPushButton* minButton = new QPushButton("─");
    minButton->setObjectName("minButton");
    minButton->setFixedSize(45, 30);
    QObject::connect(minButton, &QPushButton::clicked, mainWindow, &QMainWindow::showMinimized);
    
    // 最大化按钮
    QPushButton* maxButton = new QPushButton("□");
    maxButton->setObjectName("maxButton");
    maxButton->setFixedSize(45, 30);
    QObject::connect(maxButton, &QPushButton::clicked, [mainWindow]() {
        if(mainWindow->isMaximized()) {
            mainWindow->showNormal();
        } else {
            mainWindow->showMaximized();
        }
    });
    
    // 关闭按钮
    QPushButton* closeButton = new QPushButton("×");
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(45, 30);
    QObject::connect(closeButton, &QPushButton::clicked, mainWindow, &QMainWindow::close);
    
    layout->addWidget(minButton);
    layout->addWidget(maxButton);
    layout->addWidget(closeButton);
}

void UILayoutManager::applyStyleSheet(QMainWindow* mainWindow)
{
    QString styleSheet = R"(
        QWidget {
            font-family: "HarmonyOS Sans SC";
            font-size: 10pt;
            background-color: #f0f2f5;
        }
        
        #toolbarWidget {
            background-color: #001529;
            min-height: 60px;
        }
        
        #chatContainer {
            background-color: #f0f2f5;
        }
        
        #chatDisplay {
            border: none;
            background-color: white;
            border-radius: 4px;
            padding: 20px;
            font-family: "HarmonyOS Sans SC";
            font-size: 11pt;
        }
        
        #inputContainer {
            background-color: white;
            border-radius: 4px;
            padding: 0px;
            max-height: 70px;
        }
        
        #messageInput {
            border: none;
            background-color: #f0f2f5;
            border-radius: 4px;
            padding: 8px;
            font-family: "HarmonyOS Sans SC";
            font-size: 11pt;
            max-height: 50px;
        }
        
        QPushButton {
            font-family: "HarmonyOS Sans SC Medium";
            background-color: #1890ff;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: 500;
            min-width: 80px;
        }
        
        QPushButton:hover {
            background-color: #40a9ff;
        }
        
        QPushButton:pressed {
            background-color: #096dd9;
        }
        
        #sendButton {
            background-color: #52c41a;
            font-size: 11pt;
            padding: 0;
            margin: 0;
        }
        
        #sendButton:hover {
            background-color: #73d13d;
        }
        
        QComboBox {
            background-color: rgba(255, 255, 255, 0.1);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 4px;
            padding: 8px 15px;
            min-width: 180px;
        }
        
        QComboBox:hover {
            background-color: rgba(255, 255, 255, 0.2);
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            width: 12px;
            height: 12px;
            margin-right: 10px;
        }
        
        QComboBox QAbstractItemView {
            background-color: #001529;
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.1);
            selection-background-color: rgba(255, 255, 255, 0.1);
        }
        
        #titleBar {
            background-color: #001529;
            border: none;
            min-height: 40px;
        }
        
        #titleLabel {
            color: white;
            font-size: 12pt;
            font-weight: bold;
            padding-left: 10px;
            background-color: transparent;
        }
        
        #minButton, #maxButton, #closeButton {
            background: transparent;
            border: none;
            color: white;
            font-family: "Segoe MDL2 Assets";
            font-size: 10pt;
            padding: 0;
            margin: 0;
            min-width: 45px;
            min-height: 30px;
        }
        
        #minButton:hover, #maxButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
        }
        
        #closeButton:hover {
            background-color: #e81123;
        }
    )";
    
    mainWindow->setStyleSheet(styleSheet);
}

void UILayoutManager::setupSerialSettingsDialog(QDialog* dialog,
                                              QComboBox* baudRateBox,
                                              QComboBox* dataBitsBox,
                                              QComboBox* stopBitsBox,
                                              QComboBox* parityBox,
                                              QComboBox* flowControlBox,
                                              QSpinBox* bufferSizeBox,
                                              QSpinBox* packageDelayBox,
                                              QCheckBox* autoScrollBox,
                                              QPushButton* defaultButton,
                                              QPushButton* okButton,
                                              QPushButton* cancelButton)
{
    // 设置无边框
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建标题栏
    QWidget* titleBar = new QWidget(dialog);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(40);
    
    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);
    
    QLabel* titleLabel = new QLabel(QObject::tr("串口设置"), titleBar);
    titleLabel->setObjectName("titleLabel");
    
    QPushButton* closeButton = new QPushButton("×", titleBar);
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(45, 30);
    QObject::connect(closeButton, &QPushButton::clicked, dialog, &QDialog::reject);
    
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeButton);
    
    // 创建内容区域
    QWidget* contentWidget = new QWidget(dialog);
    contentWidget->setObjectName("contentWidget");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    // 创建设置区域
    QWidget* settingsArea = createSettingsArea(baudRateBox, dataBitsBox, stopBitsBox,
                                             parityBox, flowControlBox, bufferSizeBox,
                                             packageDelayBox, autoScrollBox);
    
    // 创建按钮区域
    QWidget* buttonArea = createSettingsButtons(defaultButton, okButton, cancelButton);
    
    contentLayout->addWidget(settingsArea);
    contentLayout->addWidget(buttonArea);
    
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(contentWidget);
    
    // 使标题栏可拖动
    titleBar->installEventFilter(dialog);
    
    // 应用样式
    applySerialSettingsStyle(dialog);
}

QWidget* UILayoutManager::createSettingsArea(QComboBox* baudRateBox,
                                           QComboBox* dataBitsBox,
                                           QComboBox* stopBitsBox,
                                           QComboBox* parityBox,
                                           QComboBox* flowControlBox,
                                           QSpinBox* bufferSizeBox,
                                           QSpinBox* packageDelayBox,
                                           QCheckBox* autoScrollBox)
{
    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout(widget);
    
    int row = 0;
    auto addRow = [&](const QString& label, QWidget* widget) {
        QLabel* lbl = new QLabel(label, widget);
        lbl->setStyleSheet("color: black;");
        layout->addWidget(lbl, row, 0);
        layout->addWidget(widget, row++, 1);
    };
    
    addRow(QObject::tr("波特率:"), baudRateBox);
    addRow(QObject::tr("数据位:"), dataBitsBox);
    addRow(QObject::tr("停止位:"), stopBitsBox);
    addRow(QObject::tr("校验位:"), parityBox);
    addRow(QObject::tr("流控制:"), flowControlBox);
    addRow(QObject::tr("缓冲区大小:"), bufferSizeBox);
    addRow(QObject::tr("合包延迟:"), packageDelayBox);
    layout->addWidget(autoScrollBox, row++, 0, 1, 2);
    
    return widget;
}

QWidget* UILayoutManager::createSettingsButtons(QPushButton* defaultButton,
                                              QPushButton* okButton,
                                              QPushButton* cancelButton)
{
    QWidget* widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(widget);
    
    layout->addWidget(defaultButton);
    layout->addStretch();
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);
    
    return widget;
}

void UILayoutManager::applySerialSettingsStyle(QDialog* dialog)
{
    dialog->setStyleSheet(R"(
        QDialog {
            background-color: #f0f2f5;
            border: 1px solid #d9d9d9;
        }
        
        #titleBar {
            background-color: #001529;
            border: none;
        }
        
        #titleLabel {
            color: white;
            font-size: 12pt;
            font-weight: bold;
            padding-left: 10px;
        }
        
        #closeButton {
            background: transparent;
            border: none;
            color: white;
            font-family: "Segoe MDL2 Assets";
            font-size: 10pt;
            padding: 0;
            margin: 0;
            min-width: 45px;
        }
        
        #closeButton:hover {
            background-color: #e81123;
        }
        
        #contentWidget {
            background-color: #f0f2f5;
        }
        
        QLabel {
            color: black;
            font-size: 10pt;
        }
        
        /* ... 其他样式保持不变 ... */
    )");
} 