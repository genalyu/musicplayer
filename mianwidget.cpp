#include "mianwidget.h"
#include "./ui_mianwidget.h"

MianWidget::MianWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MianWidget)
    , player(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);

    //禁止窗口尺寸改变
    this->setFixedSize(this->geometry().size());

    //去掉标题栏
    this->setWindowFlag(Qt::FramelessWindowHint);

    // 判断数据库是否存在，存在就连接，否则添加至链接
    if(QSqlDatabase::contains("sql_default_connection"))
    {
        db=QSqlDatabase::database("sql_default_connection");
    }
    else
    {
        db=QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("mp3listdatabase.db");
    }

    // 打开数据库
    if(!db.open())
    {
        QMessageBox::critical(0,QObject::tr("Open Data Error"),db.lastError().text());
    }
    else
    {
        // query赋值为打开的数据库
        QSqlQuery query;
        QString sql="create table if not exists searchlist(num,songname text,singername text,id)";

        if(!query.exec(sql))
        {
            QMessageBox::critical(0,QObject::tr("create searchlist Error."),query.lastError().text());
        }


        // 歌曲痕迹数据表
        sql="create table if not exists historysong(num,songname text,singername text,id)";

        if(!query.exec(sql))
        {
            QMessageBox::critical(0,QObject::tr("create historysong Error."),query.lastError().text());
        }

        // 查询历史记录
        sql="select *from historysong;";
        if(!query.exec(sql))
        {
            QMessageBox::critical(0,QObject::tr("select historysong Error."),query.lastError().text());
        }

        while(query.next())
        {
            QString songname,singername;
            QSqlRecord rec = query.record();
            int ablumkey = rec.indexOf("songname");
            int hashkey = rec.indexOf("singername");
            songname = query.value(ablumkey).toString();
            singername = query.value(hashkey).toString();

            QString strshow = songname + "--" +singername;
            QListWidgetItem *item = new QListWidgetItem(strshow);
            ui->listWidget_History->addItem(item);

        }

    }
    // 播放操作
    QMediaPlayer *player = new QMediaPlayer;
    QAudioOutput *audioOutput = new QAudioOutput;
//  QMediaPlaylist *playlist = new QMediaPlaylist;

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(updateDuration(qint64)));
    connect(this,SIGNAL(lyricShow(QString)),this,SLOT(lyricTextShow(QString)));
    connect(ui->listWidget_Search,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(playSearchMusic()));
    connect(ui->listWidget_History,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(playHistoryMusic()));
}

void MianWidget::clearHistory()
{
    QSqlQuery query;
    QString sql = "DELETE FROM historysong;";

    if (!query.exec(sql))
    {
        QMessageBox::critical(0, QObject::tr("Delete historysong Error."), query.lastError().text());
    }
    else
    {
        // 成功清空记录后，可以清空 UI 列表
        ui->listWidget_History->clear();
    }
}

MianWidget::~MianWidget()
{
    delete ui;
}

void MianWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0,0,width(),height(),QPixmap(":/new/prefix1/images/batman.jpg"));
}

void MianWidget::on_pushButton_Close_clicked()
{
    //关闭窗口
    close();
}


void MianWidget::on_pushButton_Skin_clicked() // 换皮肤
{

}


void MianWidget::on_pushButton_About_clicked()
{

}


void MianWidget::on_pushButton_Search_clicked() // 搜索音乐
{
    ui->listWidget_Search->clear();

    QSqlQuery query;
    query.exec("DELETE FROM searchlist");
    QString sql = "DELETE FROM searchlist";

    //发起请求
    QString url = kugouSearchApi + QString("%1&type=1&offset=0&total=true&limit=20").arg(ui->lineEdit_Search->text());

    httpAccess(url);

    QByteArray JsonData;
    QEventLoop loop;

    auto c = connect(this,MianWidget::finish,[&](const QByteArray &data)
    {
        JsonData = data;
        loop.exit(1);
    });

    loop.exec();// 等待loop.exit(1)的发出
    disconnect(c);

    // 解析返回数据
    hashJsonAnalysis(JsonData);
}


void MianWidget::on_VopSlider_valueChanged(int value) // 改变声音大小
{

}



void MianWidget::on_progressSlider_valueChanged(int value) // 改变播放进度
{

}


void MianWidget::on_progressSlider_sliderPressed()
{

}


void MianWidget::on_progressSlider_sliderReleased()
{

}


void MianWidget::on_pushButton_Front_clicked()
{

}


void MianWidget::on_pushButton_Playerpause_clicked()
{

}


void MianWidget::on_pushButton_Next_clicked()
{

}


void MianWidget::on_pushButton_Loopplay_clicked()
{

}
// 更新播放的进度
void MianWidget::updateDuration(qint64 value)
{
    ui->progressSlider->setRange(0,player->duration());
    ui->progressSlider->setValue(value);
}

// 读取网络数据
void MianWidget::netReply(QNetworkReply *reply)
{
    QVariant status_code=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);// 状态码

    reply->attribute(QNetworkRequest::RedirectionTargetAttribute);// 遇到重定向时更新url

    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray data=reply->readAll();
        emit finish(data);
    }
    else
    {
        qDebug()<<reply->errorString();
    }
    reply->deleteLater();// 完成后清除
}

// 显示歌词
void MianWidget::lyricTextShow(QString lycris)
{
    ui->textBrowser_Lyric->setFont(QFont("宋体",10,QFont::Bold));
    ui->textBrowser_Lyric->setTextColor(Qt::white);
    ui->textBrowser_Lyric->setText(lycris);
}
// 音乐下载和播放
void MianWidget::downloadPlayer(int id)
{
    QString url = kugouDownloadApi + QString("%1").arg(id);
    httpAccess(url);
    QByteArray JsonData;
    QEventLoop loop;
    auto d = connect(this,&MianWidget::finish,[&](const QByteArray &data){
        JsonData = data;
        loop.exit(1);
    });
    loop.exec();
    disconnect(d);
    QString music = musicJsonAnalysis(JsonData);
    qDebug()<<music;
    player->setSource(QUrl::fromLocalFile(music));


    // 设置音量
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0);

    // 设置滑动条
    ui->VopSlider->setValue(100);

    player->play();


}

// 访问http
void MianWidget::httpAccess(QString url)
{
    request = new QNetworkRequest;
    manager = new QNetworkAccessManager;
    request->setUrl(url);
    manager->get(*request);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(netReply(QNetworkReply*)));
}

// 音乐的hash和ablum_id值的解析，变成json
void MianWidget::hashJsonAnalysis(QByteArray JsonData)
{
    QJsonDocument document = QJsonDocument::fromJson(JsonData);
    if(document.isObject())
    {
        QJsonObject data = document.object(); // 对json的处理

        if(data.contains("result"))
        {
            //QJsonObject objectInfo = data.value("result").toObject();
            QJsonObject result = data["result"].toObject();

            QJsonArray objectHash = result.value("songs").toArray();
            for(int i = 0;i<objectHash.count();i++)
            {
                QString songname,singername;
                int id;
                QJsonObject album = objectHash.at(i).toObject();

                if(album.contains("artists"))
                {
                    QJsonArray singer = album.value("artists").toArray();
                    int j = 0;
                    do{
                        QJsonObject singernamex = singer.at(j).toObject();
                        if(j != 0)
                        {
                            singername = singername + "、";
                        }
                        singername = singername + singernamex.value("name").toString();
                        j++;
                    }
                    while(singer.at(j).isObject());
                }

                if(album.contains("name"))
                {
                    songname = album.value("name").toString();
                }

                if(album.contains("id"))
                {
                    id = album.value("id").toDouble();
                }

                QSqlQuery query;

                QString sql = QString("INSERT INTO searchlist (num,songname, singername, id) "
                                      "VALUES (%1, '%2', '%3',%4)")
                                    .arg(i)
                                    .arg(songname)
                                    .arg(singername)
                                    .arg(id);

                if(!query.exec(sql))
                {
                    QMessageBox::critical(0,QObject::tr("insert searchlist Error."),query.lastError().text());
                }

                // 将解析的音乐名称存入listWidget_Search
                QString show = songname + "--" + singername;
                QListWidgetItem *item = new QListWidgetItem(show);
                ui->listWidget_Search->addItem(item);

            }

        }
    }
    if(document.isArray())
    {
        qDebug()<<"Array";
    }
}
// 解析json
QString MianWidget::musicJsonAnalysis(QByteArray JsonData)
{
    QJsonDocument document = QJsonDocument::fromJson(JsonData);

    if (document.isObject())
    {
        QJsonObject data = document.object();

        QJsonObject data1 = data["data"].toObject();

        if (data1.contains("lrc"))
        {
            emit lyricShow(data1.value("lrc").toString());
        }

        if (data1.contains("url"))
        {
            return data1.value("url").toString();
        }
    }

    // 如果 document 是数组，处理数组
    if (document.isArray())
    {
        qDebug() << "Array.";
    }

    // 如果未找到 play_url，返回空字符串
    return QString();
}


void MianWidget::playSearchMusic() // 双击搜索列表播放音乐
{
    // 获取对应索取
    int row = ui->listWidget_Search->currentRow();
    qDebug()<<"row-->"<<row;

    // 查询音乐数据信息
    QSqlQuery query;

    QString sql = QString("select * from searchlist where num=%1;").arg(row);

    if(!query.exec(sql)) // 先执行再判断
    {
        QMessageBox::critical(0,QObject::tr("select searchlist table Error."),query.lastError().text());
    }

    // 将选中的音乐数据信息存入历史记录
    QString songname,singername;
    int num;
    int id;
    while(query.next())
    {
        QSqlRecord recd = query.record();
        int songkey = recd.indexOf("songname");
        int singerkey = recd.indexOf("singername");
        num = recd.indexOf("num");
        id = recd.indexOf("id");

        songname = query.value(songkey).toString();
        singername = query.value(singerkey).toString();
        num = query.value(num).toInt();
        id = query.value(id).toDouble();

        sql = QString("select id from historysong where id='%1';").arg(id);

        if(!query.exec(sql))
        {
            QMessageBox::critical(0,QObject::tr("select id Error."),query.lastError().text());
        }

        if(query.next()==NULL)
        {
            sql = QString("insert into historysong values(NULL,%1,'%2','%3',%4)").arg(num).arg(songname).arg(singername).arg(id);
            if(!query.exec(sql))
            {
                QMessageBox::critical(0,QObject::tr("insert history Error."),query.lastError().text());
            }

            // 将解析的音乐名称保存在历史记录
            QString show = songname + "--" +singername;
            QListWidgetItem *item = new QListWidgetItem(show);
            ui->listWidget_History->addItem(item);
        }
    }



    downloadPlayer(id);
}

void MianWidget::playHistoryMusic() // 双击历史列表播放音乐
{
    // 获取双击对象的索引
    int row = ui->listWidget_History->currentRow();
    qDebug()<<"row-->"<<row;

    // 查询搜索数据库中数据表的历史记录
    QSqlQuery query;
    QString sql = QString("select *from historysong where num=%1;").arg(row+1);
    if(!query.exec(sql))
    {
        QMessageBox::critical(0,QObject::tr("select historysong Error."),query.lastError().text());
    }

    int id;
    while(query.next())
    {
        QSqlRecord recd = query.record();
        int id = recd.indexOf("id");
    }


    downloadPlayer(id);
}

void MianWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePress)
    {
        QPoint movepos=event->globalPos();
        move(movepos-movePoint);
    }
}
void MianWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    mousePress=false;

}
void MianWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        mousePress=true;
    }

    movePoint=event->globalPos()-pos();
}

void MianWidget::on_clearHistory_clicked()
{
    clearHistory();
}

