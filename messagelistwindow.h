#ifndef MESSAGELISTWINDOW_H
#define MESSAGELISTWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

class MessageListItem : public QWidget
{
    Q_OBJECT
public:
    explicit MessageListItem(const QString& name, const QString& message, 
                           const QDateTime& time, QWidget* parent = nullptr);
private:
    QLabel* m_nameLabel;
    QLabel* m_messageLabel;
    QLabel* m_timeLabel;
};

class MessageListWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MessageListWindow(QWidget* parent = nullptr);
    
    void addMessage(const QString& portName, const QString& message, bool isFromMe);
    void setCurrentPort(const QString& portName);
    
private:
    QListWidget* m_listWidget;
    QString m_currentPort;
    QLabel* m_titleLabel;
    
    void setupUi();
    void applyStyle();
};

#endif // MESSAGELISTWINDOW_H 