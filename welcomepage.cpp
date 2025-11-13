#include "welcomepage.h"
#include "ui_welcomepage.h"
#include <QGraphicsDropShadowEffect>
#include <QAction>


welcomepage::welcomepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::welcomepage)
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


