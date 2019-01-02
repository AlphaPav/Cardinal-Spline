#include "cspline.h"
#include<qDebug>
#include <QtCore/qmath.h>
CSpline::CSpline(double x[100], double y[100], int grain, double tension, int n)
{

    int np= n;
    QPoint jd[100];
    QPoint *knots;
    for (int i= 1; i<=np; i++)
    {
        jd[i].setX(x[i-1]);
        jd[i].setY(y[i-1]);
    }
    jd[0].setX(x[0]); jd[0].setY(y[0]);
    jd[np+1].setX(x[np-1]); jd[np+1].setY(y[np-1]);
    np+=2;
    knots = jd;
    mKnots= knots;
    CubicSpline(np,knots,grain, tension);
    setSlope(n,tension, grain);
    setLen();
}

void CSpline::CubicSpline(int n, QPoint *knots, int grain, double tension)
{
    QPoint * s, *km1, *k0, * k1, *k2;
    int i,j;
    float u[50];
    GetCardinalMatrix(tension); //根据tension算出矩阵M
    for(i=0; i<  grain; i++) u[i]= ((float)i)/grain; //根据grain分段值，产生u[]
    s=mSpline;//插值点数组
    totalNum=0;

    //konts是传入的控制点
    km1=knots;//p_i-1
    k0=knots+1;//p_i
    k1=knots+2;//p_i+1
    k2=knots+3;//p_i+2
    for (i=1;i<=n-3;i++){
        for(j=0;j<grain;j++)
        {
            s->setX(Matrix(km1->x(),k0->x(),k1->x(),k2->x(),u[j]));
            s->setY(Matrix(km1->y(),k0->y(),k1->y(),k2->y(),u[j]));
            s++;
            totalNum++;
        }
        km1++;k0++;k1++;k2++;
    }
    s->setX(k0->x());s->setY(k0->y());
}
void CSpline::GetCardinalMatrix(double t)
{
  //  printf("%lf",t);
    m[0]= -t; m[1]=2-t; m[2]=-2+t;  m[3]=t;
    m[4]=2*t; m[5]=-3+t; m[6]=3-2*t; m[7]=-t;
    m[8]=-t; m[9]=0; m[10]=t; m[11]=0;
    m[12]=0; m[13]=1; m[14]=0; m[15]=0;
}

double CSpline::Matrix(double a, double b, double c, double d, double u)
{
    double p0,p1,p2,p3;
    p0= m[0]*a + m[1]*b+ m[2]*c+ m[3]*d;
    p1= m[4]*a + m[5]*b+ m[6]*c+ m[7]*d;
    p2= m[8]*a + m[9]*b+ m[10]*c+ m[11]*d;
    p3= m[12]*a + m[13]*b+ m[14]*c+ m[15]*d;

    return (p3 + u*(p2 + u*(p1 + u*p0)));
}

void CSpline::setSlope(int n, double tension, int grain)
{
    int m =(n-1)*grain;

    slopes= (double * )malloc(sizeof(double)* m);
    for (int i=0;i< m- 1;i ++){
        slopes[i] = tension *
                (double)(mSpline[i+1].y()-mSpline[i-1].y())
                / (double)(mSpline[i+1].x()- mSpline[i-1].x());
        if(mSpline[i+1].x()< mSpline[i-1].x())//向反方向运动，特殊处理
        {
            slopes[i]= -1* slopes[i];
        }
    }
    //p0'=  1/2* (2p1-p0-p2)
     slopes[0]= 0.5*(2*mSpline[1].y()- mSpline[0].y()- mSpline[2].y())/
                (2*mSpline[1].x()- mSpline[0].x()-mSpline[2].x());
     slopes[m-1] =slopes[m-2];
}



void CSpline::setLen()
{
    lens =(double *) malloc(sizeof (double) * totalNum);

    lens[0]=0;
    for(int i=1 ; i< totalNum;i++){
        lens[i]= lens[i-1] + sqrt((mSpline[i].y()- mSpline[i-1].y())*(mSpline[i].y()- mSpline[i-1].y())
                +(mSpline[i].x()- mSpline[i-1].x())*(mSpline[i].x()- mSpline[i-1].x()));
    }
}

double * CSpline::getSlope()
{

    return slopes;
}
double * CSpline::getLen()
{
    return lens;
}
CSpline::~CSpline()
{
    delete mKnots;
    delete slopes;
    delete lens;
}
