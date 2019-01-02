#ifndef CSPLINE_H
#define CSPLINE_H
#include<QPainter>

//Cardinal Spline Class
class CSpline
{
public:

    //x[],y[]控制点坐标, n 为控制点数量,
    //grain为控制点之间的插值平滑点, tension为通过控制点位值的曲线平滑程度
    CSpline(double x[100], double y[100], int grain, double tension, int n);
    //插值点数组
    QPoint mSpline[2056];
    //产生插值点
    void CubicSpline(int n, QPoint *knots, int grain, double tension);
    double * getSlope();//获得斜率
    double * getLen();//获得两个插值点之间距离
    virtual ~CSpline();

private:
    QPoint *mKnots;//控制点数组
    int n;//控制点数目
    double m[16];//M矩阵
    double* slopes;//每个插值点的斜率数组
    int totalNum;//插值点总数
    double * lens;//两个插值点之间的距离数组

    //计算插值点坐标
    double Matrix(double a, double b, double c, double d, double u);
    //求每个点的斜率
    void setSlope(int n, double tension, int grain);
    void setLen();
    //计算M矩阵
    void GetCardinalMatrix(double t);
};

#endif // CSPLINE_H

