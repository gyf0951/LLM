#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QTimer>
#include <QClipboard>
#include <QMimeData>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>


namespace Ui {
class welcomepage;
}

class welcomepage : public QWidget
{
    Q_OBJECT

public:
    explicit welcomepage(QWidget *parent = nullptr);
    ~welcomepage();



protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;



signals:
    void sendMessageToChatPage(const QString &msg);



private slots:
    void welcome_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_toggleBtn_clicked();

    void on_toggleBtn_2_clicked();

    void on_toolButton_2_clicked();

   // void on_toolButton_3_clicked();

private:
    Ui::welcomepage *ui;





};

#endif // WELCOMEPAGE_H
