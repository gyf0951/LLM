#include "historyuploader.h"
#include <QNetworkRequest>
#include <QDebug>
#include <QUuid>
#include <QJsonArray>

HistoryUploader::HistoryUploader(QObject *parent)
    : QObject{parent}
    ,manager(new QNetworkAccessManager(this))
{

}

// 生成32位无符号UUID
QString HistoryUploader::generateUUID()
{
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    uuid.remove('-');  // 删除短横线

    return uuid;
}


//历史记录上传
void HistoryUploader::uploadRecord(const QString &userId,
                                   const QString &modelType,
                                   const QString &question,
                                   const QString &answer,
                                   const QString &questionTime,
                                   const QString &answerTime)
{
    QString recordId = generateUUID();

    QUrl url("http://127.0.0.1:5000/api/add_record");  //  Flask 接口
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 构造 JSON 数据
    QJsonObject json;
    json["id"] = recordId;
    json["user_id"] = userId;
    json["model_type"] = modelType;
    json["question"] = question;
    json["answer"] = answer;
    json["question_time"] = questionTime;
    json["answer_time"] = answerTime;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "📤 上传聊天记录到 Flask:" << url.toString();
   // qDebug() << "🧾 数据:" << data;

    QNetworkReply *reply = manager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray resp = reply->readAll();

        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "✅ 上传成功:" << resp;
            emit uploadSuccess();
        } else {
            qDebug() << "❌ 上传失败:" << reply->errorString() << resp;
            emit uploadFailed(reply->errorString());
        }
        reply->deleteLater();
    });

}

// 查询接口实现
void HistoryUploader::fetchRecords(const QString &userId)
{
    QString url = QString("http://127.0.0.1:5000/api/get_records?user_id=%1").arg(userId);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

     QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit fetchFailed(reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (!doc.isObject()) {
            emit fetchFailed("响应不是有效JSON");
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();
        if (!obj.contains("data") || !obj["data"].isArray()) {
            emit fetchFailed("响应格式错误，没有data数组");
            reply->deleteLater();
            return;
        }

        QList<QVariantMap> recordList;
        for (const QJsonValue &v : obj["data"].toArray()) {
            if (v.isObject())
                recordList.append(v.toObject().toVariantMap());
        }

        emit recordsFetched(recordList);
        for (const QVariantMap &rec : recordList) {
            qDebug() << rec;
        }
        reply->deleteLater();
        });
}
