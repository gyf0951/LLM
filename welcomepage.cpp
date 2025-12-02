#include "welcomepage.h"
#include "ui_welcomepage.h"
#include <QGraphicsDropShadowEffect>
#include <QAction>


welcomepage::welcomepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::welcomepage)
    ,historyUploader(new HistoryUploader(this))
{
    ui->setupUi(this);




    ui->toggleBtn_2->setVisible(false);

    ui->textEdit->setFont(QFont("Microsoft YaHei", 14));

    QTimer::singleShot(0, this, [this]() {
        // 在事件循环启动后调用，保证控件布局正确
        resizeEvent(nullptr);
    });

    //发送按钮使能
    ui->pushButton->setEnabled(false);
    ui->pushButton->setIcon(QIcon());
    ui->pushButton->setStyleSheet(
        "QPushButton {"
        "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 72@2x.png\");"
        "    border-radius: 19px;"
        "}"
        );
    connect(ui->textEdit, &QTextEdit::textChanged, this, [this] {
        if(ui->textEdit->document()->isEmpty()){
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

    ui->textEdit->installEventFilter(this);
    ui->textEdit->setFocus();

    //摁键发送
    connect(ui->pushButton, &QPushButton::clicked, this,&welcomepage::welcome_pushButton_clicked);
    //connect(ui->pushButton,SIGNAL(returnPressed),this,SLOT(welcome_pushButton_clicked()));

    //附件按钮
    connect(ui->pushButton_3, &QPushButton::clicked, this, &welcomepage::on_pushButton_3_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &welcomepage::on_pushButton_2_clicked);




    //增加搜索按钮
    QAction *searchAction = new QAction(this);
    searchAction->setIcon(QIcon(":/pic/img/66@2x.png"));
    ui->lineEdit->addAction(searchAction, QLineEdit::LeadingPosition);


    // ===== 初始化历史记录控件 =====
    setupHistoryListWidget();

    // 连接历史记录信号
    connect(historyUploader, &HistoryUploader::recordsFetched,
            this, &welcomepage::onRecordsFetched);
    connect(historyUploader, &HistoryUploader::fetchFailed,
            this, &welcomepage::onFetchFailed);

    // 当用户点击历史记录列表项时，调用onHistoryItemClicked处理
    connect(ui->historyListWidget, &QListWidget::itemClicked,
            this, &welcomepage::onHistoryItemClicked);

}

//设置按钮位置
void welcomepage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // 先调用父类的处理

    int margin = 10; // 发送按钮距离边界的间隔
    ui->pushButton->move(
        ui->textEdit->x() + ui->textEdit->width() - ui->pushButton->width() - margin,
        ui->textEdit->y() + ui->textEdit->height() - ui->pushButton->height() - margin
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

   /* ui->pushButton_4->move(
        ui->pushButton_3->x()  - ui->pushButton_4->width() - margin_3,
        ui->pushButton_3->y()
        ); */

    //设置frame线条位置
    int  spacing = 15;
    ui->frame->move(
        ui->pushButton->geometry().center().x() - ui->frame->width() - spacing,
        ui->pushButton->geometry().center().y() - ui->frame->height()/2
        );
}

welcomepage::~welcomepage()
{
    delete ui;
}


bool welcomepage::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == ui->textEdit && ev->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent *>(ev);
        int key = ke->key();
        Qt::KeyboardModifiers mods = ke->modifiers();

        // 普通 Enter / Return 且没有 Shift
        if ((key == Qt::Key_Return || key == Qt::Key_Enter) &&
            !(mods & Qt::ShiftModifier))
        {
            welcome_pushButton_clicked();
            return true;
        }

        //统一复制过来的格式
        if  (ke->matches(QKeySequence::Paste))
        {
            const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
            if (mime && mime->hasText())
            {
                QTextCursor cursor = ui->textEdit->textCursor();
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

void welcomepage::welcome_pushButton_clicked()
{
    QString msg = ui->textEdit->toPlainText().trimmed();
    if (msg.isEmpty()) return;

    emit sendMessageToChatPage(msg);  // 发射信号给聊天页
    ui->textEdit->clear();
}

//上传文件功能未完善
void welcomepage::on_pushButton_3_clicked()
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

//上传图片功能未完善
void welcomepage::on_pushButton_2_clicked()
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


void welcomepage::on_toggleBtn_clicked()
{
    ui->widget_3->hide();

    ui->toggleBtn_2->setVisible(true);

    ui->horizontalLayout->setStretch(0, 1); // frame2
    ui->horizontalLayout->setStretch(1, 2); // widget2
    ui->horizontalLayout->setStretch(2, 0); // widget3 隐藏

    if (auto *vlay = qobject_cast<QVBoxLayout*>(ui->widget_2->layout())) {
        vlay->setAlignment(ui->widget_4, Qt::AlignHCenter | Qt::AlignVCenter);
        vlay->invalidate();
        vlay->activate();
    }

    // 刷新布局
    ui->horizontalLayout->invalidate();
    ui->horizontalLayout->activate();
    this->updateGeometry();

}


void welcomepage::on_toggleBtn_2_clicked()
{
    ui->widget_3->show();

    ui->toggleBtn_2->setVisible(false);

    if (auto *vlay = qobject_cast<QVBoxLayout*>(ui->widget_2->layout())) {
        vlay->setAlignment(ui->widget_4, Qt::AlignHCenter | Qt::AlignVCenter);
        vlay->invalidate();
        vlay->activate();
    }

    ui->horizontalLayout->invalidate();
    ui->horizontalLayout->activate();
    this->updateGeometry();
}


void welcomepage::on_toolButton_2_clicked()
{

}

// 初始化历史记录列表控件样式
void welcomepage::setupHistoryListWidget()
{
    ui->historyListWidget->setStyleSheet(
        // 列表整体样式
        "QListWidget {"
        "    background-color: #f8f9fa;"      // 浅灰色背景
        "    border: none;"                   // 无边框
        "    border-radius: 8px;"             // 圆角
        "    padding: 5px;"                   // 内边距
        "    font-family: 'Microsoft YaHei';" // 字体
        "    font-size: 13px;"                // 字体大小
        "}"
        // 列表项（每条历史记录）的默认样式
        "QListWidget::item {"
        "    background-color: white;"        // 白色背景
        "    border-radius: 6px;"             // 圆角
        "    padding: 12px;"                  // 内边距（上下左右12像素）
        "    margin: 4px 2px;"                // 外边距（上下4px，左右2px）
        "    border: 1px solid #e0e0e0;"      // 浅灰色边框
        "}"
        // 鼠标悬停时的样式
        "QListWidget::item:hover {"
        "    background-color: #e3f2fd;"      // 浅蓝色背景
        "    border-color: #2196f3;"          // 蓝色边框
        "}"
        // 选中状态的样式
        "QListWidget::item:selected {"
        "    background-color: #bbdefb;"      // 深蓝色背景
        "    border-color: #1976d2;"          // 深蓝色边框
        "}"
        // ===== 滚动条样式（垂直方向） =====
        "QScrollBar:vertical {"
        "    background-color: #f8f9fa;"      // 滚动条背景色
        "    width: 10px;"                    // 滚动条宽度
        "    border-radius: 5px;"             // 圆角
        "    margin: 0px;"                    // 无边距
        "}"
        // 滚动条滑块样式
        "QScrollBar::handle:vertical {"
        "    background-color: #c0c0c0;"      // 滑块颜色（灰色）
        "    border-radius: 5px;"             // 圆角
        "    min-height: 20px;"               // 最小高度
        "}"
        // 滚动条滑块悬停样式
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #a0a0a0;"      // 悬停时变深灰
        "}"
        // 隐藏滚动条的上下箭头按钮
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"                    // 高度为0，即隐藏
        "}"
        // 滚动条空白区域（滑块上下的空白部分）
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"               // 透明背景
        "}"
        );
    // 设置其他属性
    ui->historyListWidget->setSpacing(2); // 列表项之间的间距
    ui->historyListWidget->setAlternatingRowColors(false); // 不使用交替行颜色
    ui->historyListWidget->setSelectionMode(QAbstractItemView::SingleSelection);  // 单选模式


    // ----- 设置滚动行为 -----
    ui->historyListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);  // 平滑滚动（按像素）
    ui->historyListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);      // 禁用水平滚动条
}

// 加载历史记录
void welcomepage::loadHistoryRecords(const QString &userId)
{
    currentUserId = userId;
    ui->historyListWidget->clear();

    // 显示加载提示
    QListWidgetItem *loadingItem = new QListWidgetItem("⏳ 正在加载历史记录...");
    loadingItem->setTextAlignment(Qt::AlignCenter);
    loadingItem->setFlags(Qt::NoItemFlags); // 不可选中
    loadingItem->setForeground(QColor("#666666"));
    ui->historyListWidget->addItem(loadingItem);

    // 请求历史记录
    historyUploader->fetchRecords(userId);

    qDebug() << "📡 正在请求用户历史记录:" << userId;
}

// 处理获取到的历史记录
void welcomepage::onRecordsFetched(const QList<QVariantMap> &records)
{
    ui->historyListWidget->clear();

    if (records.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem("💬 暂无聊天记录");
        emptyItem->setTextAlignment(Qt::AlignCenter);
        emptyItem->setForeground(QColor("#999999"));
        emptyItem->setFlags(Qt::NoItemFlags);
        ui->historyListWidget->addItem(emptyItem);
        qDebug() << "📭 历史记录为空";
        return;
    }

    // 按时间倒序排列（最新的在上面）
    QList<QVariantMap> sortedRecords = records;
    std::sort(sortedRecords.begin(), sortedRecords.end(),
              [](const QVariantMap &a, const QVariantMap &b) {
                  return a["question_time"].toString() > b["question_time"].toString();
              });

    // 添加历史记录项
    for (const QVariantMap &record : sortedRecords) {
        addHistoryItem(record);
    }

    qDebug() << "✅ 成功加载" << records.size() << "条历史记录";
}

// 添加单条历史记录到列表
void welcomepage::addHistoryItem(const QVariantMap &record)
{
    QString question = record["question"].toString();
    QString questionTime = record["question_time"].toString();
    QString modelType = record["model_type"].toString();

    // 限制显示长度
    QString displayQuestion = question;
    if (displayQuestion.length() > 45) {
        displayQuestion = displayQuestion.left(45) + "...";
    }

    // 格式化时间
    QDateTime dt = QDateTime::fromString(questionTime, Qt::ISODate);
    QString timeStr;

    // 判断是否是今天
    QDate today = QDate::currentDate();
    if (dt.date() == today) {
        timeStr = "今天 " + dt.toString("hh:mm");
    } else if (dt.date() == today.addDays(-1)) {
        timeStr = "昨天 " + dt.toString("hh:mm");
    } else {
        timeStr = dt.toString("MM-dd hh:mm");
    }

    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem();

    // 显示格式: 问题预览 + 时间 + 模型
    QString displayText = QString("%1\n%2 · %3")
                              .arg(displayQuestion)
                              .arg(timeStr);
                              //.arg(modelType.isEmpty() ? "AI助手" : modelType);

    item->setText(displayText);
    item->setData(Qt::UserRole, record); // 存储完整数据
    item->setToolTip(question); // 鼠标悬停显示完整问题

    ui->historyListWidget->addItem(item);
}


// 处理历史记录获取失败
void welcomepage::onFetchFailed(const QString &error)
{
    ui->historyListWidget->clear();

    QListWidgetItem *errorItem = new QListWidgetItem("❌ 加载失败\n" + error);
    errorItem->setTextAlignment(Qt::AlignCenter);
    errorItem->setForeground(QColor("#f44336"));
    errorItem->setFlags(Qt::NoItemFlags);
    ui->historyListWidget->addItem(errorItem);

    qDebug() << "❌ 获取历史记录失败:" << error;
}


// 用户点击历史记录列表项
void welcomepage::onHistoryItemClicked(QListWidgetItem *item)
{
    // ----- 从item中取出存储的完整记录数据 -----
    // 现在用data(Qt::UserRole)取出来
    QVariantMap record = item->data(Qt::UserRole).toMap();

    // 检查数据是否有效（某些提示项如"加载中"没有数据）
    if (record.isEmpty()) {
        return;  // 无数据则直接返回
    }

    // ----- 发射信号，通知其他组件用户点击了历史记录 -----
    emit historyItemClicked(record);  // 传递完整的记录数据

    QString question = record["question"].toString();
    ui->textEdit->setPlainText(question);  // 显示在文本框中，用户可以再次编辑发送

    // 输出调试信息
    qDebug() << "📌 点击历史记录 ID:" << record["id"].toString();
    qDebug() << "   问题:" << question;
}
