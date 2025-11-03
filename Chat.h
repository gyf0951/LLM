#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include "welcomepage.h"


class Chat : public QObject
{
    Q_OBJECT
public:
    explicit Chat(QObject *parent = nullptr);
    void sendMessage(const QString &userMessage);
    void getProfile(); //获取应用id
    void getChatId();  //获取会话id

    // 设置模型配置
    void setModelConfig(const ModelConfig& config);

    // 获取当前模型配置
    ModelConfig getCurrentModelConfig() const;

private slots:
    // 发送消息

    //流式处理
    //void onReplyReadyRead();
    void onReplyFinished();




private:
    QNetworkAccessManager *manager;
    QString baseUrl;
    QString apiKey;
    QString profileUrl;
    QString currentMessage; //流式消息
    QString appId;  //应用id
    QString chatId;  //会话id
    QByteArray buffer; // 用来拼接流式分片

    ModelConfig m_currentModelConfig;

signals:
    // 当收到模型回复时
    void messageReceived(const QString &reply);

    // 错误处理
    void errorOccurred(const QString &errorMsg);

    //收到appid的信号
    void appIdReceived(const QString &appId);

    //收到chatid的信号
     void chatIdReceived(const QString &chatId);

};

#endif // CHAT_H
