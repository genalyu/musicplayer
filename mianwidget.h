#ifndef MIANWIDGET_H
#define MIANWIDGET_H

#include <QWidget>

#include <QPainter> // 实现窗口绘制

#include <QtNetwork/QNetworkRequest> //HTTP的URL请求管理
#include <QtNetwork/QNetworkAccessManager> //URL的上传管理
#include <QtNetwork/QNetworkReply> //网页回复数据触发信号的类
#include <QEventLoop> //进入和离开事件循环
#include <QJsonArray> //封装json数组
#include <QJsonObject>
#include <QJsonDocument>

#include <QtMultimedia/QtMultimedia>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include <QMessageBox>
#include <QTime>
#include <math.h>

#include <QMouseEvent>

#include <QtMultimedia/QAudioOutput>


static QString kugouSearchApi = "https://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s=";
static QString kugouDownloadApi = "https://api.gumengya.com/Api/Netease?format=json&id=";



QT_BEGIN_NAMESPACE
namespace Ui {
class MianWidget;
}
QT_END_NAMESPACE

class MianWidget : public QWidget
{
    Q_OBJECT

public:
    MianWidget(QWidget *parent = nullptr);
    ~MianWidget();

    // 处理窗口ui图片
    void paintEvent(QPaintEvent *event);

signals:
    void finish(QByteArray);
    void lyricShow(QString);


private slots:
    void on_pushButton_Close_clicked();//关闭窗口
    void on_pushButton_Skin_clicked();//更换皮肤
    void on_pushButton_About_clicked();//应用程序关于操作
    void on_pushButton_Search_clicked();//搜搜音乐
    void on_VopSlider_valueChanged(int value);//音量调节



    void on_progressSlider_valueChanged(int value);//播放进度
    void on_progressSlider_sliderPressed();
    void on_progressSlider_sliderReleased();

    void on_pushButton_Front_clicked();
    void on_pushButton_Playerpause_clicked();
    void on_pushButton_Next_clicked();
    void on_pushButton_Loopplay_clicked();

    void clearHistory();

    // 更新播放的进度
    void updateDuration(qint64);

    // 读取网络数据
    void netReply(QNetworkReply *);

    // 显示歌词
    void lyricTextShow(QString lycris);

    // 音乐播放
    void playSearchMusic(); // 双击搜搜列表播放音乐
    void playHistoryMusic(); // 双击历史列表播放音乐

    void on_clearHistory_clicked();

protected:
    // 音乐下载和播放
    void downloadPlayer(int id);

    // 访问http
    void httpAccess(QString url);

    // 音乐的hash和ablum_id值的解析，使用json
    void hashJsonAnalysis(QByteArray);

    // 解析json
    QString musicJsonAnalysis(QByteArray);


private:
    Ui::MianWidget *ui;

    // 网络 播放器 sqlite数据库
    QNetworkRequest *request;
    QNetworkAccessManager *manager;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QSqlDatabase db;
    int row;

    // 处理鼠标拖动窗口
private:
    QPoint m_mousePoint;
    QPoint movePoint;
    bool mousePress;
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
};
#endif // MIANWIDGET_H
