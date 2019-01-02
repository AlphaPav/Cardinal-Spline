#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    flag=0;
    n=0;
    mCSpline=NULL;
    timer1= new QTimer();
    connect(timer1, SIGNAL(timeout()),this,SLOT(timer1Rotate()));
    timer2= new QTimer();
    connect(timer2, SIGNAL(timeout()),this,SLOT(timer2Rotate()));
    connect(ui->Draw_pushButton,&QPushButton::clicked,this,&MainWindow::on_Draw_pushButton_clicked);
    connect(ui->Clear_pushButton,&QPushButton::clicked,this,&MainWindow::on_Clear_pushButton_clicked);
    connect(ui->Points_pushButton,&QPushButton::clicked,this,&MainWindow::on_Points_pushButton_clicked);
    connect(ui->Play_pushButton,&QPushButton::clicked,this,&MainWindow::on_Play_pushButton_clicked);
    ui->Points_pushButton->setEnabled(false);
    ui->Play_pushButton->setEnabled(false);
    ui->Play_pushButton->setText("Play");

    //设置obj控件来承载图片
    obj =new QLabel(this);
    obj->setFixedSize(140,140);
    obj->hide();

    //img对象为png图片
    isImage=true;//初始化的默认选择是png图片
    QImage * OriImg= new QImage(":/carpng.png");
    img = new QImage();
    *img= OriImg->scaled(120,120,Qt::IgnoreAspectRatio);
    if(img==NULL) printf("imgnull");

    //mov对象是gif动态图片
    mov= new QMovie();
    mov->setFileName(":/cargif.gif");
    if(mov==NULL) printf("imgnull");
    mov->setScaledSize(QSize(120,120));
    mov->start();

    this->setWindowTitle("Cardinal Spline Demo");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *ev)
{
    QFile qssfile(":/base.qss");//为UI控件设置样式
    qssfile.open(QFile::ReadOnly);
    QString qss;
    qss = qssfile.readAll();
    this->setStyleSheet(qss);

    QPainter p(this);
    //在界面的上部分设置背景为白色，表示可接受鼠标点击的绘画区域
    p.setClipRect(0,0,size().width(),size().height()-200);
    p.fillRect(QRectF(0, 0, size().width(), size().height()), Qt::white);

    p.setPen(QPen(QColor(200, 0 ,0), 6));
    p.setRenderHint(QPainter::Antialiasing, true);
    for (int i=0 ;i<mPoints.size();i++)
    {
        p.drawPoint(mPoints.at(i)); //绘制关键点
    }
    p.setPen(QPen(QColor(100,100,100), 2));
    for (int i=0 ;i<mPoints.size()-1;i++)
    {
        p.drawLine(mPoints.at(i), mPoints.at(i+1));//绘制关键点之间的曲线
    }
    if(flag==1)//绘制插值曲线
    {   p.setPen(QPen(QColor(71,156,194),2,Qt::SolidLine,Qt::RoundCap));
        for(int j=0;j<(n-1)*grain;j++)
        {
            p.drawLine(mCSpline->mSpline[j].x(),mCSpline->mSpline[j].y(),
                       mCSpline->mSpline[j+1].x(),mCSpline->mSpline[j+1].y());
        }
    }else if(flag==2)//绘制插值点
    {
        p.setPen(QPen(QColor(71,156,194),4,Qt::SolidLine,Qt::RoundCap));
        for(int j=0;j<(n-1)*grain;j++)
        {
           p.drawPoint(mCSpline->mSpline[j].x(),mCSpline->mSpline[j].y());
        }
    }
}


void MainWindow::mousePressEvent(QMouseEvent *ev)
{
   if(flag==0)//可接受点击画点的模式
   {
       if((ev->pos().x() <size().width() )&& (ev->pos().x() >0 )
               && (ev->pos().y() < (size().height()-200)) &&  (ev->pos().y() >0))
       {
           mPoints.append(ev->pos());
           n=mPoints.size();
           px[n-1]=ev->pos().x();
           py[n-1]=ev->pos().y();
           update();
       }
   }
}

//处理点击绘制按钮
void MainWindow::on_Draw_pushButton_clicked()
{
    if(n>0)
    {
        flag=1;//绘制模式，此时不接受用户点击画点
        //从界面获得grain和tensiton的值
        grain =ui ->Grain_spinBox->value();
        tension= ui->Tension_doubleSpinBox->value();

        //为了处理用户连续多次点击绘制按钮而不清除画布的情况，先清空mCSpline
        if(mCSpline!= NULL)
        {
            delete mCSpline;
        }
        mCSpline= new CSpline(px,py,grain,tension,n);

        ui->Points_pushButton->setEnabled(true);
        ui->Play_pushButton->setEnabled(true);
        update();
    }
}

//处理点击清除按钮
void MainWindow::on_Clear_pushButton_clicked()
{
    delete mCSpline;
    mCSpline=NULL;
    QVector<QPoint>().swap(mPoints);

    flag=0;
    n=0;
    obj->hide();//隐藏小车图片的label
    ui->Points_pushButton->setEnabled(false);
    ui->Play_pushButton->setEnabled(false);
    ui->Play_pushButton->setText("Play");
    update();
}


void MainWindow::on_Points_pushButton_clicked()
{
    flag=2;
    update();
}

void MainWindow::on_Play_pushButton_clicked()
{
    ui->Play_pushButton->setText("RePlay");
    if(ui->Image_radioButton->isChecked() == true)
    {
         QPixmapCache::clear();
         obj->setPixmap(QPixmap::fromImage(*img));
         isImage= true;
    }else if(ui->Gif_radioButton->isChecked()==true)
    {
        isImage=false;
        QPixmapCache::clear();
        obj->setMovie(mov);

    }
    obj->show();

    if(ui->Para_checkBox->isChecked()==false)
    {   int k1= 1000;
        speed= ui->Speed_horizontalSlider->value()*1.0;
        speed = speed/3;
        ani= new QPropertyAnimation(obj,"pos");
        ani->setDuration(k1*(n-1)*grain* 1.0 /speed);
        ani->setStartValue(QPoint(mCSpline->mSpline[0].x()-70, mCSpline->mSpline[0].y()-120));
        for( int i =0 ;i < (n-1)*grain-1;i++)
        {
           ani->setKeyValueAt((i+1)*1.0 /((n-1)*grain), QPoint(mCSpline->mSpline[i].x()-70, mCSpline->mSpline[i].y()-120));
        }
        ani->setEndValue(QPoint(mCSpline->mSpline[(n-1)*grain-1].x()-70, mCSpline->mSpline[(n-1)*grain-1].y()-120));
        ani->start();
        timer2->stop();
        timer1->setInterval(k1/speed);//基于插值点的计时器
        timer1->start();
    }else{
        speed= ui->Speed_horizontalSlider->value()*1.0;
        speed = speed/10;
        double * lens= mCSpline->getLen();
        ani2= new QPropertyAnimation(obj,"pos");
        ani2->setDuration(10*(lens[(n-1)*grain-1] *1.0/speed) );
        ani2->setStartValue(QPoint(mCSpline->mSpline[0].x()-70, mCSpline->mSpline[0].y()-120));
        int tickcount=0;
        while(tickcount < (lens[(n-1)*grain-1] *1.0/speed))
        {
            int mu;
            double s;
             s= speed* tickcount;
            for(int i=0;i <(n-1)*grain -1 ;i++ )
            {
                if( s>=lens[i]&& s<= lens[i+1]) mu=i+1;
            }
            if(mu< (n-1)*grain )
            {
                ani2->setKeyValueAt((tickcount)*1.0 /(lens[(n-1)*grain-1] *1.0/speed),
                            QPoint(mCSpline->mSpline[mu].x()-70, mCSpline->mSpline[mu].y()-120));
            }
            tickcount+= 1;
        }
        ani2->setEndValue(QPoint(mCSpline->mSpline[(n-1)*grain-1].x()-70, mCSpline->mSpline[(n-1)*grain-1].y()-120));
        ani2->start();
        timer1->stop();
        timer2->setInterval(10);//基于弧长的匀速运动的计时器
        timer2->start();
    }
    ticks=1;
}
void MainWindow::timer1Rotate()
{
    double * slopes= mCSpline->getSlope();
    if(ticks>= ((n-1)*grain))
    {
        timer1->stop();
    }else{
        if (isImage){//png图片
            double angle= atan(slopes[ticks])/M_PI * 180;
            QMatrix matrix;
            matrix.rotate(angle);
            QImage imgRotate= img->transformed(matrix,Qt::FastTransformation);
            if( (ticks<(n-1)*grain-2)&& (mCSpline->mSpline[ticks+2].x() < mCSpline->mSpline[ticks].x()))
            {
                imgRotate= imgRotate.mirrored(true,false);
            }else if((ticks>= (n-1)*grain-2)&& (mCSpline->mSpline[ticks].x() < mCSpline->mSpline[ticks-2].x()))
            {
                imgRotate= imgRotate.mirrored(true,false);
            }
            QPixmapCache::clear();
            obj->setPixmap(QPixmap::fromImage(imgRotate));
        }else{//gif图片无法旋转，故不作处理
        }
        ticks= ticks+1;
    }
}

void MainWindow::timer2Rotate(){
    double * slopes= mCSpline->getSlope();
    double * lens= mCSpline->getLen();
    if(ticks < (lens[(n-1)*grain-1] *1.0/speed))
    {
        int mu;
        double s;
         s= speed* ticks;
        for(int i=0;i <(n-1)*grain -1 ;i++ )
        {
            if( s>=lens[i]&& s<= lens[i+1]) mu=i+1;
        }
        if(mu< (n-1)*grain )
        {
            if(isImage)
            {
                double angle= atan(slopes[mu])/M_PI * 180;
                QMatrix matrix;
                matrix.rotate(angle);
                QImage imgRotate= img->transformed(matrix,Qt::FastTransformation);
                if( (mu<(n-1)*grain-2 )&&(mCSpline->mSpline[mu+2].x() < mCSpline->mSpline[mu].x()))
                {
                    imgRotate= imgRotate.mirrored(true,false);//反方向运动时镜面反转
                }else if ((mu >= (n-1)*grain-2 )&&(mCSpline->mSpline[mu].x() < mCSpline->mSpline[mu-2].x())){
                    imgRotate= imgRotate.mirrored(true,false);
                }
                QPixmapCache::clear();
                obj->setPixmap(QPixmap::fromImage(imgRotate));
            }
            ticks++;
        }else{
             ticks++;
        }
    }else{
        if(isImage)
        {
            double angle= atan(slopes[(n-1)*grain-1])/M_PI * 180;

            QMatrix matrix;
            matrix.rotate(angle);
            QImage imgRotate= img->transformed(matrix,Qt::FastTransformation);
            if(mCSpline->mSpline[(n-1)*grain-1 ].x() < mCSpline->mSpline[(n-1)*grain-3].x())
            {
                imgRotate= imgRotate.mirrored(true,false);
            }
            QPixmapCache::clear();
            obj->setPixmap(QPixmap::fromImage(imgRotate));

        }
       // obj->move( mCSpline->mSpline[(n-1)*grain-1].x()-70, mCSpline->mSpline[(n-1)*grain-1].y()-100);
        timer2->stop();
    }
    obj->show();

}
