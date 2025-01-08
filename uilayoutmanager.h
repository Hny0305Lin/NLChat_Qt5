#ifndef UILAYOUTMANAGER_H
#define UILAYOUTMANAGER_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QWidget>
#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include "chatbubblewidget.h"
#include "messagelistwindow.h"

class UILayoutManager
{
public:
    static void setupMainWindowLayout(QMainWindow* mainWindow,
                                    QComboBox* portList,
                                    QPushButton* connectButton,
                                    QPushButton* refreshButton,
                                    QPushButton* settingsButton,
                                    ChatBubbleWidget* chatDisplay,
                                    QTextEdit* messageInput,
                                    QPushButton* sendButton,
                                    MessageListWindow* messageList);
    
    static void applyStyleSheet(QMainWindow* mainWindow);
    
    static void setupSerialSettingsDialog(QDialog* dialog,
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
                                        QPushButton* cancelButton);
                                        
    static void applySerialSettingsStyle(QDialog* dialog);
    
    static QWidget* createTitleBar(QMainWindow* mainWindow, const QString& title);
    
private:
    static QWidget* createToolbar(QComboBox* portList,
                                QPushButton* connectButton,
                                QPushButton* refreshButton,
                                QPushButton* settingsButton);
                                
    static QWidget* createInputArea(QTextEdit* messageInput,
                                  QPushButton* sendButton);
                                  
    static QWidget* createChatArea(QWidget* toolbar,
                                 ChatBubbleWidget* chatDisplay,
                                 QWidget* inputArea);
                                 
    static QWidget* createSettingsArea(QComboBox* baudRateBox,
                                     QComboBox* dataBitsBox,
                                     QComboBox* stopBitsBox,
                                     QComboBox* parityBox,
                                     QComboBox* flowControlBox,
                                     QSpinBox* bufferSizeBox,
                                     QSpinBox* packageDelayBox,
                                     QCheckBox* autoScrollBox);
                                     
    static QWidget* createSettingsButtons(QPushButton* defaultButton,
                                        QPushButton* okButton,
                                        QPushButton* cancelButton);
    
    static void setupWindowButtons(QWidget* titleBar, QMainWindow* mainWindow);
};

#endif // UILAYOUTMANAGER_H 