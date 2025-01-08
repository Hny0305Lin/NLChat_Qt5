#ifndef NLCHATWINDOW_H
#define NLCHATWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTime>
#include <QFont>
#include <QApplication>
#include <QScrollBar>
#include <QKeyEvent>
#include "serialmanager.h"
#include "serialsettingsdialog.h"
#include "chatbubblewidget.h"
#include "messagelistwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class NLChatWindow; }
QT_END_NAMESPACE

class NLChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    NLChatWindow(QWidget *parent = nullptr);
    ~NLChatWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void handleConnectButton();
    void handleSendButton();
    void handleMessage(const QString& message);
    void handleError(const QString& error);
    void handlePortsChanged();
    void handleConnectionStatus(bool connected);
    void refreshPortList();
    void handleSettingsButton();

private:
    Ui::NLChatWindow *ui;
    SerialManager* m_serialManager;
    
    // UI Elements
    QComboBox* m_portList;
    QPushButton* m_connectButton;
    QPushButton* m_refreshButton;
    ChatBubbleWidget* m_chatDisplay;
    QTextEdit* m_messageInput;
    QPushButton* m_sendButton;
    QPushButton* m_settingsButton;
    SerialSettingsDialog::Settings m_serialSettings;
    MessageListWindow* m_messageList;
    QPoint m_dragPosition;

    void setupUi();
    void initializeSerialManager();
    void appendMessage(const QString& message, bool isFromMe);
    void appendSystemMessage(const QString& message);
};

#endif // NLCHATWINDOW_H
