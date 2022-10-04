#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <cmath>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

struct point
{
    long double x, y;
    point operator-(point obj) {return point(this->x - obj.x, this->y - obj.y);}
    bool operator==(point obj) {return (this->x == obj.x) and (this->y == obj.y);}
    point (long double a, long double b) : x(a), y(b) {}
    static long double dist(point a, point b) {return sqrt(pow(a.x-b.x, 2) + pow(a.y-b.y, 2)); }
};

struct triplet
{
    long double x1, x2, x3, y1, y2, y3;
    triplet (point a, point b, point c)
    {
        x1 = a.x;
        y1 = a.y;
        x2 = b.x;
        y2 = b.y;
        x3 = c.x;
        y3 = c.y;
    }
};

struct duplet
{
    point centre;
    long double radius;
    duplet(point a, long double b) : centre(a), radius(b) {}
};

enum block
{
    left = -1, right = 1, None = 0
};

struct edge
{
    point a, b;
    long double vect_x, vect_y;
    block status;
    edge (point left, point right, block v_block = None) : a(left), b(right), status(v_block)
    {
        auto buf = b-a;
        this->vect_x = buf.x;
        this->vect_y = buf.y;
    }
    long double operator*(edge obj) {return this->vect_x*obj.vect_x + this->vect_y*obj.vect_y;}
    long double norm() {return point::dist(this->a, this->b);}
    bool operator==(edge obj) {return (this->a == obj.a) and (this->b == obj.b);}
    long double edge_vect(edge target, block flag)
    {
        auto buf = sgn(this->vect_x*target.vect_y - this->vect_y*target.vect_x);
        if (buf == 0) return 0;
        if (flag == None) return 1;
        else
            if (flag == buf) return 0;
                else return 1;
    }
    long double edge_pol(point target, block flag)
    {
        auto target_buf_a = edge(this->a, target);
        auto target_buf_b = edge(this->b, target);
        if ((target_buf_a.norm() == 0) or (target_buf_b.norm() == 0))
            return 0;
        if ((target == this->a) or (target== this->b))
            return 0;
        return this->edge_vect(target_buf_a, flag)*acos((pow(target_buf_a.norm(), 2)+pow(target_buf_b.norm(), 2)-pow(this->norm(),2))
                                                      /(2*target_buf_a.norm()*target_buf_b.norm()));
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    virtual void mousePressEvent(QMouseEvent* pe);
    virtual void paintEvent(QPaintEvent *event);
private slots:
    void on_calcButton_clicked();

    void on_clearButton_clicked();

private:
private:
    std::vector <point> data;
    std::vector <edge> triang_result;
    std::vector <triplet> triplet_base;
    std::vector <duplet> circle_base;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
