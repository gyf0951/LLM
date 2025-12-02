#include "widget.h"
#include "ui_widget.h"

#include <QScrollBar>
#include "HistoryUploader.h"
#include <QDateTime>

Widget::Widget(Chat *chatInstance,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , chat(chatInstance)
{
    ui->setupUi(this);

    ui->toolButton_9->setVisible(false);

    ui->chatEdit->setFont(QFont("Microsoft YaHei", 14));
    ui->listWidget->setFont(QFont("Microsoft YaHei", 14));
    //chat->getProfile();
    //connect(chat,&Chat::appIdReceived,chat,&Chat::getChatId);


    QTimer::singleShot(0, this, [this]() {
        // 在事件循环启动后调用，保证控件布局正确
        resizeEvent(nullptr);
    });




    //设置过滤器
    ui->chatEdit->installEventFilter(this);
    ui->chatEdit->setFocus();
    //发送按钮使能
    ui->pushButton->setEnabled(false);
    ui->pushButton->setIcon(QIcon());
    ui->pushButton->setStyleSheet(
        "QPushButton {"
        "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 72@2x.png\");"
        "    border-radius: 19px;"
        "}"
        );
    connect(ui->chatEdit, &QTextEdit::textChanged, this, [this] {
        if(ui->chatEdit->document()->isEmpty()){
            ui->pushButton->setEnabled(false);
            ui->pushButton->setStyleSheet(
                "QPushButton {"
                "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 72@2x.png\");"
                "    border-radius: 19px;"
                "}"
                );
        }
        else{
            ui->pushButton->setEnabled(true);
            ui->pushButton->setStyleSheet(
                "QPushButton {"
                "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 77@2x.png\");"
                "    border-radius: 19px;"
                "}"
                );
        }
    });

    connect(chat, &Chat::messageReceived, this, [=](const QString &reply){
        ui->listWidget->appendToLastMessage(reply, false); // false = AI 消息

        currentAnswerBuffer.append(reply); // 拼接完整回答

        // 保存流式回复（拼接到同一条消息）
        for (ChatSession &s : sessions) {
            if (s.id == currentSessionId) {
                // ... (你保存历史记录的逻辑保持不变)
                if (s.messages.isEmpty() || s.messages.last().startsWith("User:")) {
                    s.messages.append(reply);
                } else {
                    s.messages.last().append(reply);
                }
                break;
            }
        }

        //qDebug() << "AI 回复:" << reply;



    });

    connect(chat, &Chat::finished, this, [=](){
        qDebug() << "完整回答为：" << currentAnswerBuffer;

        // 历史记录上传到数据库
            QString userId = "13299501181";
            QString modelType = "jxkJxkChat";
            QString question = w_msg;
            QString answer = currentAnswerBuffer;
            QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            uploader->uploadRecord(userId, modelType, question, answer, before, now);


        currentAnswerBuffer.clear();
    });

    connect(chat, &Chat::errorOccurred, this, [=](const QString &err){
        ui->listWidget->addMessage("❌ 请求失败: " + err, false);
    });

    //摁键发送
    connect(ui->pushButton, &QPushButton::clicked, this,&Widget::pushButton_clicked);
   // connect(ui->pushButton,SIGNAL(returnPressed),this,SLOT(pushButton_clicked()));

    //附件上传
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Widget::on_pushButton_3_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Widget::on_pushButton_2_clicked);


    //新建对话
    connect(ui->newChatBtn, &QPushButton::clicked, this, &Widget::onNewChatClicked);
    connect(ui->sessionList, &QListWidget::itemClicked, this, &Widget::onSessionSelected);


    //设置listwidget的滚轮
    ui->listWidget->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical { width:12px; background:#f5f5f5; margin:0; }"
        "QScrollBar::handle:vertical { background:#c0c0c0; border-radius:6px; min-height:20px; }"
        "QScrollBar::handle:vertical:hover { background:#a0a0a0; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
        );



    uploader = new HistoryUploader(this);
    connect(uploader, &HistoryUploader::uploadSuccess, this, [](){
        qDebug() << "历史记录上传成功 ";
    });
    connect(uploader, &HistoryUploader::uploadFailed, this, [](const QString &err){
        qDebug() << "历史记录上传失败 " << err;
    });



}


//设置按钮位置
void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // 先调用父类的处理

    int margin = 10; // 发送按钮距离边界的间隔
    ui->pushButton->move(
        ui->chatEdit->x() + ui->chatEdit->width() - ui->pushButton->width() - margin,
        ui->chatEdit->y() + ui->chatEdit->height() - ui->pushButton->height() - margin
        );

    int margin_2 = 20; //按钮2与发送按钮间隔
    ui->pushButton_2->move(
        ui->pushButton->x()  - ui->pushButton_2->width() - margin_2,
        ui->pushButton->y()
        );

    int margin_3 = 11;  //按钮3与发送按钮间隔
    ui->pushButton_3->move(
        ui->pushButton_2->x()  - ui->pushButton_3->width() - margin_3,
        ui->pushButton_2->y()
        );

    //设置frame线条位置
    int  spacing = 15;
    ui->frame->move(
        ui->pushButton->geometry().center().x() - ui->frame->width() - spacing,
        ui->pushButton->geometry().center().y() - ui->frame->height()/2
        );
}


// 事件过滤器
bool Widget::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == ui->chatEdit && ev->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent *>(ev);
        int key = ke->key();
        Qt::KeyboardModifiers mods = ke->modifiers();

        // 普通 Enter / Return 且没有 Shift
         if ((key == Qt::Key_Return || key == Qt::Key_Enter) &&
            !(mods & Qt::ShiftModifier))
        {
            pushButton_clicked();
            return true;
        }

        //统一复制过来的格式
        if  (ke->matches(QKeySequence::Paste))
        {
            const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
            if (mime && mime->hasText())
            {
                QTextCursor cursor = ui->chatEdit->textCursor();
                QTextCharFormat fmt;
                fmt.setFontFamilies({ "Microsoft YaHei" });
                fmt.setFontPointSize(9);
                cursor.insertText(mime->text(), fmt);
                return true;
            }
        }

    }





    return QWidget::eventFilter(obj, ev);
}


Widget::~Widget()
{
    delete ui;
}




void Widget::pushButton_clicked()
{
    QString msg= ui->chatEdit->toPlainText().trimmed(); // 去掉前后空格和换行
    if (msg.isEmpty())
    {

        ui->chatEdit->clear();
        return;
    }
    ui->listWidget->addMessage(msg,true);
    ui->chatEdit->clear();
    ui->chatEdit->toPlainText();

    chat->sendMessage(msg);

    // 保存到当前会话
    for (ChatSession &s : sessions) {
        if (s.id == currentSessionId) {
            s.messages.append(msg);

            // 如果是前几条，就更新会话标题
            if (s.title == "新对话" && !msg.isEmpty()) {
                s.title = msg.left(15); // 前 15 个字作为标题
                // 同步到 UI
                for (int i=0; i<ui->sessionList->count(); i++) {
                    QListWidgetItem *it = ui->sessionList->item(i);
                    if (it->data(Qt::UserRole).toString() == s.id) {
                        it->setText(s.title);
                        break;
                    }
                }
            }
            break;
        }
    }
}

//接受从welcomepage的消息
void Widget::receiveMessageFromWelcome(const QString &msg)
{
    if (msg.isEmpty()) return;

    ui->listWidget->addMessage(msg,true);
    ui->chatEdit->clear();
    ui->chatEdit->toPlainText();
    chat->sendMessage(msg);

    //消息同步到历史记录里
    w_msg = msg;
    before = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

}


void Widget::receiveMessageFromLLM(const QString &msg)
{
    if (msg.isEmpty()) return;

    ui->listWidget->addMessage(msg,true);
    ui->chatEdit->clear();
    ui->chatEdit->toPlainText();
    chat->sendMessage(msg);
}


//上传图片（功能待完善）
void Widget::on_pushButton_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开图片"),
        QDir::homePath(),
        tr("图片文件 (*.jpg *.jpeg *.png *.bmp *.gif *.tiff *.webp);;"
           "JPEG文件 (*.jpg *.jpeg);;"
           "PNG文件 (*.png);;"
           "所有文件 (*)")
        );
    if (!filePath.isEmpty()) {
        // 使用系统默认程序打开图片
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}


void Widget::on_pushButton_3_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开文件"),
        QDir::homePath(),
        tr("Office文件 (*.doc *.docx *.pdf *.xls *.xlsx);;Word文件 (*.doc *.docx);;PDF文件 (*.pdf);;Excel文件 (*.xls *.xlsx);;所有文件 (*)")
        );
    if (!filePath.isEmpty()) {
        // 使用系统默认程序打开文件
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}


void Widget::on_toolButton_8_clicked()
{
    ui->widget_3->hide();

    ui->toolButton_9->setVisible(true);
}


void Widget::on_toolButton_9_clicked()
{
    ui->widget_3->show();

    ui->toolButton_9->setVisible(false);
}


void Widget::onNewChatClicked()
{
    // 创建新会话
    ChatSession session;
    session.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    session.title = "新对话";
    sessions.append(session);

    // 更新 UI（添加到列表）
    QListWidgetItem *item = new QListWidgetItem(session.title);
    item->setData(Qt::UserRole, session.id);
    ui->sessionList->addItem(item);

    // 切换到这个会话
    currentSessionId = session.id;
    ui->listWidget->clear(); // 清空聊天窗口
}


void Widget::onSessionSelected(QListWidgetItem *item)
{
    QString sessionId = item->data(Qt::UserRole).toString();
    currentSessionId = sessionId;

    // 找到对应会话
    for (const ChatSession &s : sessions) {
        if (s.id == sessionId) {
            // 清空并加载历史消息
            ui->listWidget->clear();
            for (const QString &msg : s.messages) {
                ui->listWidget->addMessage(msg, true); // 简化处理，先都当用户消息
            }
            break;
        }
    }
}


