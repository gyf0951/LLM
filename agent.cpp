#include "agent.h"
#include "ui_agent.h"
#include <QPointer>
#include <cstdio>
#include<QVBoxLayout>
#include<QTextBrowser>
#include<QScrollBar>
#include<QTableWidget>
#include <QRegularExpression>
#include<cmark-gfm.h>
#include<cmark-gfm_export.h>
#include<QTimer>

Agent::Agent(QWidget *parent)
    : QWidget(parent),
    manager(new QNetworkAccessManager(this))
    , ui(new Ui::Agent)
{
    ui->setupUi(this);

    ui->toolButton_9->setVisible(false);

    apiKey  = "application-abc742f2c161f6e18b8919734b5d9f5a";

    baseUrl = "http://172.10.32.226:8080";   //http://211.93.8.150:18082

    profileUrl = baseUrl + "/api/application/profile";

    m_apiKey = "application-38e4e9f014001410c75d06e0119cc7fe";

    // getProfile();
    // connect(this,&Agent::appIdReceived,this,&Agent::getChatId);

    // Meeting_getProfile();
    // connect(this,&Agent::m_appIdReceived,this,&Agent::Meeting_getChatId);
}

Agent::~Agent()
{
    delete ui;
}

void Agent::on_toolButton_8_clicked()
{
    ui->widget_3->hide();

    ui->toolButton_9->setVisible(true);
}





void Agent::on_toolButton_9_clicked()
{
    ui->widget_3->show();

    ui->toolButton_9->setVisible(false);
}

// AI语音会议纪要
void Agent::on_toolButton_7_clicked()
{
    // if (chatId.isEmpty()) {
    //     QMessageBox::warning(this, "提示", "请等待获取 chatId 后再上传");
    //     return;
    // }

    // QString filePath = QFileDialog::getOpenFileName(this, "选择音频", "", "音频文件 (*.mp3 *.wav *.m4a *.flac)");
    // if (filePath.isEmpty()) return;

    // uploadAudio(filePath);

    // connect(this,&Agent::messageReceived,this,[this](){
    //     Meeting_sendSummaryRequests(finalText);
    // });

    if(!m_recordDlg)
    {
        m_recordDlg = new MeetingAssistantDialog(this);
    }
    m_recordDlg->open();

}

//调用profie接口
void Agent::getProfile()
{
    qDebug() << "准备请求 profile 接口，profileUrl=" << profileUrl;

    QNetworkRequest request;
    request.setUrl(QUrl(profileUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QByteArray(apiKey.toUtf8()));
    request.setRawHeader("Accept", "application/json");



    qDebug() << "发送请求...";

    QNetworkReply *reply = manager->get(request);
    QByteArray response = reply->readAll();
    qDebug() << response ;

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
                    qDebug() << "获取到音频转文字应用 id:" << appId;

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
void Agent::getChatId()
{
    if (appId.isEmpty()) {
        qDebug() << "appId 为空，请先调用 getProfile() 获取";
        return;
    }
    QString url = QString("%1/api/application/%2/chat/open").arg(baseUrl).arg(appId);
    qDebug() << "准备请求 chatId 接口，url=" << url;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QByteArray(apiKey.toUtf8()));
    request.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = manager->get(request);

    connect(reply,&QNetworkReply::finished,this,[this, reply]()
            {
                qDebug() << "chat/open finished 信号触发";

                if (reply->error() != QNetworkReply::NoError) {
                    qDebug() << "请求错误:" << reply->error() << reply->errorString();
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
                            qDebug() << "获取到音频转文字 chatId:" << chatId;

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

//上传音频
void Agent::uploadAudio(const QString &filePath)
{

    // 拼接新的上传 URL
    QString uploadUrl = QString("%1/api/application/%2/chat/%3/upload_file")
                            .arg(baseUrl,appId, chatId);

    QNetworkRequest req;
    req.setUrl(QUrl(uploadUrl));
    req.setRawHeader("AUTHORIZATION", apiKey.toUtf8());

    QFileInfo info(filePath);
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件！");
        return;
    }

    QByteArray audioData = file->readAll();
    file->close();

    QHttpMultiPart *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart audioPart;
    audioPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file\"; filename=\"audio.mp3\""));
    audioPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("audio/mpeg"));
    audioPart.setBody(audioData);

    multi->append(audioPart);

    QNetworkReply *reply = manager->post(req, multi);
    multi->setParent(reply); // 自动释放

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray resp = reply->readAll();
        qDebug() << " 上传响应:" << resp;

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(resp);
            if (!doc.isObject()) {
                emit errorOccurred("返回格式错误");
                return;
            }
            QJsonObject obj = doc.object();
            if (obj["code"].toInt() != 200) {
                emit errorOccurred(obj["message"].toString());
                return;
            }

            QJsonArray dataArray = obj["data"].toArray();
            if (dataArray.isEmpty()) {
                emit errorOccurred("data 为空");
                return;
            }

            QJsonObject fileObj = dataArray.first().toObject();
            QString fileId = fileObj["file_id"].toString();
            QString fileUrl = fileObj["url"].toString();
            qDebug() << "✅ 上传成功 FileId:" << fileId << " Url:" << fileUrl;
            if (!fileId.isEmpty())
            {


                    m_processingBox = new QMessageBox(this);
                    m_processingBox->setWindowTitle("处理中");
                    m_processingBox->setText("正在处理音频文件，请稍候...");
                    m_processingBox->setModal(true); // 设置为模态对话框
                    m_processingBox->show();
                    sendTranscriptionRequest(fileId, fileUrl, info.fileName(),filePath);

            }
        }
        reply->deleteLater();
    });
}


//发送音频
void Agent::sendTranscriptionRequest(const QString &fileId, const QString &fileUrl, const QString &fileName,const QString &filePath)
{
    QString url = QString("%1/api/application/chat_message/%2").arg(baseUrl,chatId);
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("AUTHORIZATION", apiKey.toUtf8());

    QJsonObject audioObj;
    audioObj["name"] = fileName;
    audioObj["percentage"] = 0;
    audioObj["status"] = "ready";
    audioObj["size"] = QFileInfo(filePath).size();

    QJsonObject raw;
    qint64 uid = QDateTime::currentMSecsSinceEpoch();
    raw["uid"] = uid;
    audioObj["raw"] = raw;
    audioObj["uid"] = uid;
    audioObj["url"] = fileUrl;
    audioObj["file_id"] = fileId;

    QJsonObject body;
    body["message"] = "请开始转录";
    body["re_chat"] = false;
    body["image_list"] = QJsonArray();
    body["document_list"] = QJsonArray();
    body["audio_list"] = QJsonArray{audioObj};
    body["video_list"] = QJsonArray();
    body["form_data"] = QJsonObject();

    QByteArray data = QJsonDocument(body).toJson();

    QNetworkReply *reply = manager->post(req, data);


    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray resp = reply->readAll();
        //  qDebug() << "转录请求响应:" << resp;
        QList<QByteArray> lines = resp.split('\n');


        for (QByteArray line : lines) {
            line = line.trimmed();
            if (line.startsWith("data:")) {
                line = line.mid(5).trimmed(); // 去掉 "data:" 前缀
                if (line.isEmpty()) continue;

                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(line, &err);
                if (err.error != QJsonParseError::NoError)
                    continue;

                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    QString content = obj.value("content").toString();
                    if (!content.isEmpty())
                        finalText += content;
                }
            }
        }

        if (!finalText.isEmpty()) {
            qDebug() << "📝 最终转录结果:" << finalText;
            // QMessageBox::information(this, "转录成功", finalText);
            emit messageReceived("转录成功");
        } else {
            QMessageBox::information(this, "提示", "没有检测到有效转录内容。");
        }

        reply->deleteLater();

    });
}


// 调用会议纪要智能体 ----------------------------

//获取会议纪要应用id
void Agent::Meeting_getProfile()
{
    qDebug() << "准备请求 profile 接口，profileUrl=" << profileUrl;

    QNetworkRequest request;
    request.setUrl(QUrl(profileUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QByteArray(m_apiKey.toUtf8()));
    request.setRawHeader("Accept", "application/json");



    qDebug() << "发送请求...";

    QNetworkReply *reply = manager->get(request);
    QByteArray response = reply->readAll();
    qDebug() << response ;

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
                    m_appId = dataObj["id"].toString();
                    qDebug() << "获取到会议纪要应用 id:" << m_appId;

                    emit m_appIdReceived(m_appId);
                } else {
                    emit errorOccurred("Profile 返回数据格式错误");
                }
            } else {
                emit errorOccurred("Profile 返回不是有效 JSON");
            }
        }
        reply->deleteLater();
    });
}



//获取会议纪要会话id
void Agent::Meeting_getChatId()
{
    if (m_appId.isEmpty()) {
        qDebug() << "m_appId 为空，请先调用 Meeting_getProfile() 获取";
        return;
    }

    QString url = QString("%1/api/application/%2/chat/open").arg(baseUrl,m_appId);
    qDebug() << "准备请求 chatId 接口，url=" << url;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QByteArray(m_apiKey.toUtf8()));
    request.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = manager->get(request);

    connect(reply,&QNetworkReply::finished,this,[this, reply]()
            {
                qDebug() << "chat/open finished 信号触发";

                if (reply->error() != QNetworkReply::NoError) {
                    qDebug() << "请求错误:" << reply->error() << reply->errorString();
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
                            m_chatId = obj["data"].toString();
                            qDebug() << "获取到会议纪要 chatId:" << m_chatId;

                            emit m_chatIdReceived(m_chatId);  //发送chatid信号

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


void Agent::Meeting_sendSummaryRequests(const QString &finalText)
{
    if (m_chatId.isEmpty() || m_appId.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先获取会议纪要应用 ID 和 Chat ID！");
        return;
    }

    const QStringList types = {"summary", "insights", "highlights"};
    QNetworkAccessManager *mgr = manager;

    auto results = std::make_shared<QMap<QString, QString>>();
    auto completedCount = std::make_shared<int>(0);

    // ====== 全部完成 → 弹出 Markdown 窗口 ======
    auto checkAllCompleted = [this, types, results, completedCount]() {
        if (*completedCount < types.size()) return;

        qDebug().noquote() << "\n✅ 所有任务完成";

        if (m_processingBox)
        {
            m_processingBox->close();
            delete m_processingBox;
            m_processingBox = nullptr;
        }

        QWidget *mdWindow = new QWidget;
        mdWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdWindow->setWindowTitle("会议纪要结果");
        mdWindow->resize(950, 750);

        QVBoxLayout *mainLayout = new QVBoxLayout(mdWindow);

        QTabWidget *tabWidget = new QTabWidget(mdWindow);
        tabWidget->setStyleSheet(
            "QTabBar::tab { font-family:'Microsoft YaHei'; font-size:16px; padding:10px 20px; "
            "background:#f0f0f0; border:1px solid #d0d0d0; border-bottom:none; }"
            "QTabBar::tab:selected { background:#ffffff; font-weight:bold; color:#0078D7; }"
            "QTabWidget::pane { border:1px solid #d0d0d0; top:-1px; }"
            );

        // ✅ 使用 cmark-gfm 渲染 Markdown 的函数
        auto renderWithCmark = [](const QString &markdownText) -> QString {
            if (markdownText.isEmpty()) {
                return "<p>无内容</p>";
            }

            // 预处理：修复常见的格式问题
            QString cleanText = markdownText;

            // 基础清理
            cleanText.replace("\\n", "\n");
            cleanText.replace("\\t", "    ");
            cleanText.replace("\\r", "");

            // 修复列表格式
            cleanText.replace(QRegularExpression(R"((\d+)\.(\S))"), "\\1. \\2");

            // 修复粗体格式
            cleanText.replace(QRegularExpression(R"(\*\*([^*\n]+?)\*\*)"), "**\\1**");

            // 修复标题格式
            cleanText.replace(QRegularExpression(R"(^#+([^#\s]))", QRegularExpression::MultilineOption), "# \\1");

            // 转换为 UTF-8
            QByteArray utf8Data = cleanText.toUtf8();

            // 使用 cmark-gfm 解析 Markdown
            cmark_node *doc = cmark_parse_document(
                utf8Data.constData(),
                utf8Data.length(),
                CMARK_OPT_DEFAULT | CMARK_OPT_FOOTNOTES | CMARK_OPT_TABLE_PREFER_STYLE_ATTRIBUTES
                );

            if (!doc) {
                qDebug() << "Error: Failed to parse Markdown with cmark-gfm";
                return "<p>Error: Failed to parse Markdown</p>";
            }

            // 渲染为 HTML
            char *html = cmark_render_html(doc, CMARK_OPT_DEFAULT | CMARK_OPT_TABLE_PREFER_STYLE_ATTRIBUTES,nullptr);
            QString result = QString::fromUtf8(html);

            // 释放内存
            free(html);
            cmark_node_free(doc);

            return result;
        };

        // ✅ 创建使用 cmark-gfm 的浏览器
        auto createMarkdownBrowser = [renderWithCmark](const QString &rawText, const QString &type = "") -> QTextBrowser* {
            QTextBrowser *browser = new QTextBrowser;
            browser->setOpenExternalLinks(true);
            browser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            // 滚动条样式
            browser->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical { width:12px; background:#f5f5f5; margin:0; }"
                "QScrollBar::handle:vertical { background:#c0c0c0; border-radius:6px; min-height:20px; }"
                "QScrollBar::handle:vertical:hover { background:#a0a0a0; }"
                "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
                );

            // 设置字体
            QFont font("Microsoft YaHei", 11);
            browser->setFont(font);

            // 使用 cmark-gfm 渲染为 HTML
            QString htmlContent = renderWithCmark(rawText);

            // 添加自定义 CSS 样式
            QString styledHtml = QString(
                                     "<!DOCTYPE html>"
                                     "<html>"
                                     "<head>"
                                     "<meta charset=\"UTF-8\">"
                                     "<style>"
                                     "body { "
                                     "    font-family: 'Microsoft YaHei', sans-serif; "
                                     "    font-size: 11pt; "
                                     "    line-height: 1.6; "
                                     "    margin: 15px; "
                                     "    color: #333; "
                                     "    background-color: #ffffff;"
                                     "}"
                                     "h1, h2, h3, h4, h5, h6 { "
                                     "    margin-top: 1.2em; "
                                     "    margin-bottom: 0.6em; "
                                     "    color: #2c3e50; "
                                     "    font-weight: 600;"
                                     "}"
                                     "h1 { "
                                     "    font-size: 1.8em; "
                                     "    border-bottom: 2px solid #eee; "
                                     "    padding-bottom: 0.3em;"
                                     "}"
                                     "h2 { "
                                     "    font-size: 1.5em; "
                                     "    border-bottom: 1px solid #eee; "
                                     "    padding-bottom: 0.2em;"
                                     "}"
                                     "h3 { font-size: 1.3em; }"
                                     "h4 { font-size: 1.1em; }"
                                     "ul, ol { "
                                     "    padding-left: 2em; "
                                     "    margin-bottom: 1em;"
                                     "}"
                                     "li { "
                                     "    margin-bottom: 0.5em; "
                                     "    line-height: 1.6;"
                                     "}"
                                     "p { "
                                     "    margin-bottom: 1em; "
                                     "    line-height: 1.6;"
                                     "}"
                                     "blockquote { "
                                     "    border-left: 4px solid #3498db; "
                                     "    margin: 1em 0; "
                                     "    padding-left: 1.5em; "
                                     "    color: #555; "
                                     "    background-color: #f8f9fa;"
                                     "}"
                                     "code { "
                                     "    background-color: #f1f3f4; "
                                     "    padding: 2px 6px; "
                                     "    border-radius: 3px; "
                                     "    font-family: 'Consolas', 'Monaco', monospace; "
                                     "    font-size: 0.9em;"
                                     "}"
                                     "pre { "
                                     "    background-color: #f8f9fa; "
                                     "    padding: 1em; "
                                     "    border-radius: 5px; "
                                     "    overflow-x: auto; "
                                     "    border: 1px solid #e1e4e8;"
                                     "}"
                                     "pre code { "
                                     "    background: none; "
                                     "    padding: 0;"
                                     "}"
                                     "table { "
                                     "    border-collapse: collapse; "
                                     "    width: 100%; "
                                     "    margin: 1em 0; "
                                     "    box-shadow: 0 1px 3px rgba(0,0,0,0.1);"
                                     "}"
                                     "th, td { "
                                     "    border: 1px solid #ddd; "
                                     "    padding: 12px; "
                                     "    text-align: left;"
                                     "}"
                                     "th { "
                                     "    background-color: #f2f6fc; "
                                     "    font-weight: 600;"
                                     "}"
                                     "tr:nth-child(even) { "
                                     "    background-color: #fafbfc;"
                                     "}"
                                     "strong, b { "
                                     "    color: #2c3e50; "
                                     "    font-weight: 600;"
                                     "}"
                                     "em, i { "
                                     "    color: #555;"
                                     "}"
                                     "a { "
                                     "    color: #3498db; "
                                     "    text-decoration: none;"
                                     "}"
                                     "a:hover { "
                                     "    text-decoration: underline;"
                                     "}"
                                     ".highlight { "
                                     "    background-color: #fff3cd; "
                                     "    padding: 2px 4px; "
                                     "    border-radius: 3px;"
                                     "}"
                                     "</style>"
                                     "</head>"
                                     "<body>%1</body>"
                                     "</html>"
                                     ).arg(htmlContent);

            browser->setHtml(styledHtml);
            return browser;
        };

        // 获取三个类型的内容
        QString summary = results->value("summary");
        QString insights = results->value("insights");
        QString highlights = results->value("highlights");

        // 调试输出原始内容
        qDebug().noquote() << "=== 重点原始内容 ===";
        qDebug().noquote() << highlights;
        qDebug().noquote() << "==================";

        // 创建三个 Tab
        QTextBrowser *sumBrowser = createMarkdownBrowser(summary, "summary");
        QTextBrowser *insBrowser = createMarkdownBrowser(insights, "insights");
        QTextBrowser *highBrowser = createMarkdownBrowser(highlights, "highlights");

        tabWidget->addTab(sumBrowser, "📝 摘要");
        tabWidget->addTab(insBrowser, "💡 洞察");
        tabWidget->addTab(highBrowser, "📌 重点");

        mainLayout->addWidget(tabWidget);
        mdWindow->setLayout(mainLayout);
        mdWindow->show();
    };

    // --- 循环请求 ---
    for (const QString &type : types) {
        QUrl url(QString("%1/api/application/chat_message/%2").arg(baseUrl,m_chatId));
        QNetworkRequest req(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        req.setRawHeader("Authorization", m_apiKey.toUtf8());

        QString taskMessage;
        if (type == "summary") taskMessage = "请生成会议摘要。";
        else if (type == "insights") taskMessage = "请提炼会议中的关键观点与洞察。";
        else if (type == "highlights") taskMessage = "请总结会议重点内容。";

        QJsonObject body;
        body["message"] = finalText + "\n" + taskMessage;
        body["re_chat"] = false;
        body["image_list"] = QJsonArray();
        body["document_list"] = QJsonArray();
        body["audio_list"] = QJsonArray();
        body["video_list"] = QJsonArray();

        QJsonObject form_data;
        form_data["type"] = type;
        body["form_data"] = form_data;

        QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);
        QNetworkReply *reply = mgr->post(req, data);
        QPointer<QNetworkReply> safeReply(reply);

        auto contentBuffer = std::make_shared<QString>();
        auto hasEnded = std::make_shared<bool>(false);

        connect(reply, &QNetworkReply::readyRead, this,
                [safeReply, type, contentBuffer, hasEnded, results, completedCount, checkAllCompleted]() {
                    if (!safeReply || *hasEnded) return;

                    QByteArray chunk = safeReply->readAll();
                    QList<QByteArray> lines = chunk.split('\n');

                    for (QByteArray line : lines) {
                        line = line.trimmed();
                        if (!line.startsWith("data:")) continue;

                        QByteArray jsonData = line.mid(5).trimmed();
                        if (jsonData.isEmpty()) continue;

                        QJsonParseError err;
                        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &err);
                        if (err.error != QJsonParseError::NoError || !doc.isObject()) continue;

                        QJsonObject obj = doc.object();
                        QString content = obj.value("content").toString();
                        bool isEnd = obj.value("is_end").toBool(false);

                        if (!content.isEmpty()) {
                            *contentBuffer += content;
                        }

                        if (isEnd && !*hasEnded) {
                            *hasEnded = true;
                            results->insert(type, *contentBuffer);
                            (*completedCount)++;

                            qDebug().noquote() << QString("✅ [%1] 完成，内容长度: %2").arg(type).arg(contentBuffer->length());

                            checkAllCompleted();

                            if (safeReply) {
                                safeReply->deleteLater();
                            }
                        }
                    }
                });

        connect(reply, &QNetworkReply::errorOccurred, this,
                [safeReply, type, results, completedCount, checkAllCompleted](QNetworkReply::NetworkError err) {
                    if (results->contains(type)) return;
                    QString errMsg = safeReply ? safeReply->errorString() : QString("Network error %1").arg((int)err);
                    results->insert(type, QString("请求失败: %1").arg(errMsg));
                    (*completedCount)++;
                    qDebug().noquote() << QString("❌ [%1] 请求出错: %2").arg(type, errMsg);
                    if (safeReply) safeReply->deleteLater();
                    checkAllCompleted();
                });

        // 添加 finished 信号处理
        connect(reply, &QNetworkReply::finished, this,
                [safeReply, type, contentBuffer, hasEnded, results, completedCount, checkAllCompleted]() {
                    if (!safeReply || *hasEnded || results->contains(type)) return;

                    *hasEnded = true;
                    results->insert(type, *contentBuffer);
                    (*completedCount)++;

                    qDebug().noquote() << QString("📋 [%1] 请求完成，内容长度: %2").arg(type).arg(contentBuffer->length());

                    checkAllCompleted();

                    if (safeReply) {
                        safeReply->deleteLater();
                    }
                });
    }
}


void Agent::sendMindRequest(const QString &summaryText, const QMap<QString, QString> &results)
{
    QString url = QString("%1/api/application/chat_message/%1").arg(baseUrl,m_chatId);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", m_apiKey.toUtf8());

    QJsonObject body;
    body["message"] = summaryText;
    body["re_chat"] = false;
    body["image_list"] = QJsonArray();
    body["document_list"] = QJsonArray();
    body["audio_list"] = QJsonArray();
    body["video_list"] = QJsonArray();

    QJsonObject form_data;
    form_data["type"] = "mind";
    body["form_data"] = form_data;

    QByteArray data = QJsonDocument(body).toJson();
    QNetworkReply *reply = manager->post(req, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray resp = reply->readAll();
        qDebug() << "✅ mind 返回原文:" << resp;

        QString contentText;
        QList<QByteArray> lines = resp.split('\n');
        for (QByteArray line : lines) {
            line = line.trimmed();
            if (line.startsWith("data:")) {
                line = line.mid(5).trimmed();
                if (line.isEmpty()) continue;

                QJsonDocument doc = QJsonDocument::fromJson(line);
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    QString content = obj.value("content").toString();
                    if (!content.isEmpty()) contentText += content;
                }
            }
        }

        if (!contentText.isEmpty()) {
            qDebug() << "🧠 mind 最终结果:" << contentText;
            QMessageBox::information(nullptr, "会议纪要生成完成",
                                     QString("Summary:\n%1\n\nInsights:\n%2\n\nHighlights:\n%3\n\nMind:\n%4")
                                         .arg(results["summary"])
                                         .arg(results["insights"])
                                         .arg(results["highlights"])
                                         .arg(contentText));
        } else {
            QMessageBox::warning(nullptr, "提示", "mind 未检测到有效内容。");
        }

        reply->deleteLater();
    });
}
