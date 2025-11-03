#ifndef AGENT_H
#define AGENT_H

#include"meetingassistantdialog.h"

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioSink>
#include <QWidget>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QMessageBox>
#include <QFileInfo>



namespace Ui {
class Agent;
}

class Agent : public QWidget
{
    Q_OBJECT

public:
    explicit Agent(QWidget *parent = nullptr);
    ~Agent();
    void getProfile(); //获取应用id
    void getChatId();  //获取会话id
    void uploadAudio(const QString &filePath); // 上传音频文件
    //音频转录
    void sendTranscriptionRequest(const QString &fileId, const QString &fileUrl, const QString &fileName,const QString &filePath);


    void Meeting_getProfile(); //获取会议纪要应用id
    void Meeting_getChatId();  //获取会议纪要会话id
    void Meeting_sendSummaryRequests(const QString &finalText); //调用获取summary等
    void sendMindRequest(const QString &summaryText, const QMap<QString, QString> &results); //调用mind

private slots:
    void on_toolButton_8_clicked();

    void on_toolButton_9_clicked();

    void on_toolButton_7_clicked();

private:
    Ui::Agent *ui;
    QNetworkAccessManager *manager;
    QString baseUrl;
    QString apiKey;
    QString profileUrl;
    QString currentMessage; //流式消息
    QString appId;  //应用id
    QString chatId;  //会话id
    QByteArray buffer; // 用来拼接流式分片

    QString finalText;  //记录音频转录结果

    QString m_apiKey;
    QString m_appId;  //会议纪要应用id
    QString m_chatId;  //会议纪要会话id

    QMessageBox *m_processingBox = nullptr; //保存消息框指针

    MeetingAssistantDialog *m_recordDlg = nullptr;

signals:
    // 当收到模型回复时
    void messageReceived(const QString &reply);

    // 错误处理
    void errorOccurred(const QString &errorMsg);

    //收到appid的信号
    void appIdReceived(const QString &appId);

    //收到chatid的信号
    void chatIdReceived(const QString &chatId);

    //收到会议纪要appid的信号
    void m_appIdReceived(const QString &appId);

    //收到会议纪要chatid的信号
    void m_chatIdReceived(const QString &chatId);
};

#endif // AGENT_H
