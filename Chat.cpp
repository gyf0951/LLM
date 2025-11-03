#include "Chat.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>

Chat::Chat(QObject *parent)
    : QObject{parent},
    manager(new QNetworkAccessManager(this))
    ,currentMessage("")
    ,appId("")
    ,chatId("")
{

    // 初始化默认配置
    m_currentModelConfig.id = "default";
    m_currentModelConfig.apiKey = "application-4a5b57a1f8e72c81b7fe26194a1243d0"; // 默认API Key

    //API key和url
    apiKey  = "application-4a5b57a1f8e72c81b7fe26194a1243d0"; //"application-d0eb3d363ce0e5e805e9d7fb29dc09c0";
    baseUrl = "http://172.10.32.226:8080";   //http://211.93.8.150:18082  http://172.10.32.226:8080

    profileUrl = baseUrl + "/api/application/profile";
    //baseUrl = "https://ark.cn-beijing.volces.com/api/v3/chat/completions"; //豆包
    //apiKey  =  "d7146afd-8d2c-40ff-a6ac-b233ee313f1e";  //豆包

    connect(this, &Chat::appIdReceived, this, &Chat::getChatId, Qt::UniqueConnection);

    qDebug() << "Chat 实例创建:" << this;
}


// 设置模型配置
void Chat::setModelConfig(const ModelConfig& config)
{
    m_currentModelConfig = config;

    qDebug() << "Chat 模型配置已更新:" << m_currentModelConfig.name;
    qDebug() << "API Key:" << m_currentModelConfig.apiKey;

    appId.clear();
    chatId.clear();

    getProfile();
}


// 获取当前模型配置
ModelConfig Chat::getCurrentModelConfig() const
{
    return m_currentModelConfig;
}


void Chat::sendMessage(const QString &userMessage)
{
    if (chatId.isEmpty()) {
        qDebug() << "chatId 为空，不能发送消息";
        return;
    }

    qDebug() << "sendMessage() this=" << this << " chatId=" << chatId;

    //请求地址
    QString url = QString("%1/api/application/chat_message/%2").arg(baseUrl,chatId);
    qDebug() << "请求地址:" << url;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //qDebug() << "API Key:" << m_currentModelConfig.apiKey;
    request.setRawHeader("AUTHORIZATION", m_currentModelConfig.apiKey.toUtf8());
    request.setRawHeader("accept", "application/json");


    // 构造 JSON 请求体
    QJsonObject root;
    root["message"] = userMessage;
    root["re_chat"] = false;
    root["image_list"] = QJsonArray();
    root["document_list"] = QJsonArray();
    root["audio_list"] = QJsonArray();
    root["video_list"] = QJsonArray();
    root["form_data"] = QJsonObject();

    QJsonDocument doc(root);
    QByteArray data = doc.toJson();

    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, data);

    // 处理返回
    connect(reply, &QIODevice::readyRead, this, [=]() {
        QByteArray chunk = reply->readAll();
        buffer.append(chunk); // 累积到 buffer

        // 按 \n 拆分
        int index = 0;
        while ((index = buffer.indexOf("\n")) != -1) {
            QByteArray line = buffer.left(index).trimmed();
            buffer.remove(0, index + 1);

            if (line.startsWith("data:")) {
                QByteArray jsonPart = line.mid(5).trimmed();
                if (!jsonPart.isEmpty()) {
                    QJsonParseError err;
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonPart, &err);
                    if (err.error == QJsonParseError::NoError && respDoc.isObject()) {
                        QJsonObject obj = respDoc.object();
                        if (obj.contains("content")) {
                            QString content = obj["content"].toString();
                            if (!content.isEmpty()) {
                                // 发出流式片段信号
                                emit messageReceived(content);
                            }
                        }
                    } else {
                        qDebug() << "JSON 解析失败:" << err.errorString() << jsonPart;
                    }
                }
            }
        }
    });

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "send Message请求错误:" << reply->errorString();
            emit errorOccurred(reply->errorString());
        } else {
            qDebug() << "流式消息传输完成";
        }
        reply->deleteLater();
    });

}

//调用profie接口
void Chat::getProfile()
{
    qDebug() << "准备请求 profile 接口，profileUrl=" << profileUrl;

    QNetworkRequest request;
    request.setUrl(QUrl(profileUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", m_currentModelConfig.apiKey.toUtf8());
    request.setRawHeader("Accept", "application/json");

    // 关闭 SSL 校验
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    qDebug() << "发送请求...";

    QNetworkReply *reply = manager->get(request);

    // 等待 finished 信号再去读取数据
    connect(reply, &QNetworkReply::finished, this, [reply, this]() {
        qDebug() << "finished 信号触发";

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "请求错误:" << reply->error() << reply->errorString();
            emit errorOccurred(reply->errorString());
        } else {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "请求成功，HTTP 状态码:" << statusCode;

            QByteArray response = reply->readAll();
            //qDebug() << "Profile 返回:" << response;

            QJsonDocument doc = QJsonDocument::fromJson(response);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.contains("data") && obj["data"].isObject()) {
                    QJsonObject dataObj = obj["data"].toObject();
                    appId = dataObj["id"].toString();
                    qDebug() << "获取到应用 id:" << appId;

                    emit appIdReceived(appId);
                } else {
                    emit errorOccurred("Profile 返回数据格式错误");
                }
            } else {
                emit errorOccurred("Profile 返回不是有效 JSON");
            }
        }
        reply->deleteLater();
    });

    // 捕获 SSL 错误
    connect(reply, QOverload<const QList<QSslError>&>::of(&QNetworkReply::sslErrors),
            this, [=](const QList<QSslError> &errors){
                for (auto &err : errors)
                    qDebug() << "SSL 错误:" << err.errorString();
                reply->ignoreSslErrors();
            });

    // 捕获网络错误
    connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code){
        qDebug() << "网络错误 code:" << code << " errorString:" << reply->errorString();
    });
}


//调用chatid
void Chat::getChatId()
{
    if (appId.isEmpty()) {
        qDebug() << "appId 为空，请先调用 getProfile() 获取";
        return;
    }

    qDebug() << "getChatId() this=" << this;

    QString url = QString("%1/api/application/%2/chat/open").arg(baseUrl,appId);
    qDebug() << "准备请求 chatId 接口，url=" << url;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", m_currentModelConfig.apiKey.toUtf8());
    request.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = manager->get(request);

    connect(reply,&QNetworkReply::finished,this,[this, reply]()
            {
                qDebug() << "chat/open finished 信号触发";

                if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "getChatId 请求错误:" << reply->error() << reply->errorString();
                emit errorOccurred(reply->errorString());
                }
                else{
                    qDebug() << "请求成功，HTTP 状态码:"
                             << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                    QByteArray response = reply->readAll();
                    //qDebug() << "chat/open 返回:" << response;

                    QJsonDocument doc = QJsonDocument::fromJson(response);
                    if(doc.isObject()){
                        QJsonObject obj = doc.object();
                        if (obj.contains("data")) {
                            chatId = obj["data"].toString();
                            qDebug() << "获取到 chatId:" << chatId;

                            emit chatIdReceived(chatId);  //发送chatid信号

                        }else{
                             emit errorOccurred("chatid 返回数据格式错误");
                        }
                    }else{
                        emit errorOccurred("chatid 返回不是有效 JSON");
                    }
                }
                reply->deleteLater();
            }
            );
    connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code){
        qDebug() << "chat/open 网络错误 code:" << code << " errorString:" << reply->errorString();
    });
}


void Chat::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();

        // 转 JSON
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString message = obj["choices"][0]["message"]["content"].toString();
            emit messageReceived(message);
        } else {
            emit errorOccurred("返回数据不是有效的 JSON");
        }
    } else {
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
}
