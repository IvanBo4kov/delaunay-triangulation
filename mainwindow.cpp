#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <vector>
#include <utility>
#include <algorithm>
#include <queue>
#include <set>
#include <unistd.h>

using std::vector;
using std::queue;

const int rad = 10;


long double pol_angle (point a, point b)
{
    return acos((b.x-a.x)/point::dist(a, b));
}

class comp_pol
{
    private:
        point init_point;
    public:
        comp_pol(point data) : init_point(data) {}
        bool operator()(point a, point b)
        {
            return (pol_angle(init_point, a) < pol_angle(init_point, b)) or
                    ((pol_angle(init_point, a) == pol_angle(init_point, b))
                and (point::dist(init_point, a) < point::dist(init_point, b)));
        }
};

vector <edge> get_triangulation(vector <point>& data, vector <triplet>& triplet_base)
{
    sort(data.begin(), data.end(), [](point a, point b) {return (a.y < b.y) or ((a.y == b.y) and (a.x < b.x));});
    sort(data.begin()+1, data.end(), comp_pol(data[0]));
    vector <edge> result_edge;
    queue <edge> process_data;
    process_data.push(edge(data[0], data[1]));
    auto point_base = vector(data.begin(), data.end());
    while (!process_data.empty())
    {
        edge process_buf = process_data.front();
        process_data.pop();
        auto max_it = max_element(std::begin(point_base), std::end(point_base), [&](point a, point b)
        {
            return process_buf.edge_pol(a, process_buf.status) < process_buf.edge_pol(b, process_buf.status);
        });
        auto marker = process_buf.edge_pol(*max_it, process_buf.status);
        if ((marker == 0) or (marker == -0))
        {
            if (find(result_edge.begin(), result_edge.end(), process_buf) == result_edge.end())
                result_edge.push_back(process_buf);
            continue;
        }
        auto temp_buf = {edge(process_buf.a, *max_it, right), edge(process_buf.b, *max_it, right)};
        triplet_base.push_back(triplet(process_buf.a, *max_it, process_buf.b));
        for (auto x: temp_buf)
            if (find(result_edge.begin(), result_edge.end(), x) == result_edge.end())
                process_data.push(x);
        result_edge.push_back(process_buf);
    }
    return result_edge;
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (ui->checkCircle->isChecked())
        for (auto x : this->circle_base)
            painter.drawEllipse(x.centre.x - x.radius, x.centre.y - x.radius, 2*x.radius, 2*x.radius);
    painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
    for (auto x : this->data)
        painter.drawEllipse(int(x.x) - 2.5, int(x.y) - 2.5, 5, 5);
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
    if (this->triang_result.size() > 1)
    {
        for (auto x = this->triang_result.begin(); x != this->triang_result.end(); x++)
        {
            auto a = x->a, b = x->b;
            painter.drawLine(a.x, a.y, b.x, b.y);
        }
        painter.setBrush(QBrush());

    }

}


void MainWindow::mousePressEvent(QMouseEvent* pe)
{
    this->data.push_back(point(pe->position().x(), pe->position().y()));
    this->repaint();
}


vector <duplet> get_circle(vector <triplet>& triplet_base)
{
    vector <duplet> result;
    for (auto obj : triplet_base)
    {
        long double x12 = obj.x1-obj.x2, x23 = obj.x2-obj.x3,
                    x31 = obj.x3-obj.x1, y12 = obj.y1-obj.y2,
                    y23 = obj.y2-obj.y3,
                    y31 = obj.y3-obj.y1,
                    z1 = pow(obj.x1, 2) + pow(obj.y1, 2),
                    z2 = pow(obj.x2, 2) + pow(obj.y2, 2),
                    z3 = pow(obj.x3, 2) + pow(obj.y3, 2);
        long double zx = y12*z3+y23*z1+y31*z2,
                    zy = x12*z3+x23*z1+x31*z2,
                    z = x12*y31-y12*x31;
        long double a = -zx/(2*z),
                    b = zy/(2*z);
        long double rad = sqrt(pow(obj.x1 - a, 2) + pow(obj.y1 - b, 2));
        result.push_back(duplet(point(a, b), rad));
    }
    return result;
}




void MainWindow::on_calcButton_clicked()
{
    if (this->data.size() < 3) return;
    this->triplet_base = {};
    this->triang_result = get_triangulation(this->data, this->triplet_base);
    this->circle_base = get_circle(this->triplet_base);
    this->repaint();
}


void MainWindow::on_clearButton_clicked()
{
    this->data.clear();
    this->circle_base.clear();
    this->triang_result.clear();
    this->triplet_base.clear();
    this->repaint();
}

