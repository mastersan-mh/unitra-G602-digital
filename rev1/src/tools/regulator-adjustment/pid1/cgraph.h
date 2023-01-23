#ifndef CGRAPH_H
#define CGRAPH_H

#include <QWidget>
#include <QLayout>
#include <QList>

#include <QGraphicsView>
#include <QGraphicsScene>


#include <QTimer>
#include <QResizeEvent>


class CGraph;

class CGraphPlot
{
public:
    void updatePlotValues(const QList<double> &values);
    friend CGraph;

private:
    explicit CGraphPlot(CGraph * owner, const QColor &color, const QList<double> &values)
        : m_owner(owner)
        , m_color(color)
        , m_values(values)
    {}
    ~CGraphPlot() = default;

    CGraph * m_owner;
    QColor m_color;
    QList<double> m_values;
};

class CGraph : public QGraphicsView
{
    Q_OBJECT
public:

/*
    Plot        Legend—Defines the color and style of plots. Resize the legend to display multiple plots. This option does not apply to intensity graphs or charts.
    Scale       Legend—Defines labels for scales and configures scale properties.
    Graph       Palette—Allows you to move the cursor and zoom and pan the graph or chart while a VI runs.
    Cursor      Legend—Displays a marker at a defined point coordinate. You can display multiple cursors on a graph. This option applies only to graphs.
    X Scrollbar             —Scrolls through the data in the graph or chart. Use the scroll bar to view data that the graph or chart does not currently display.
    X Scale and Y Scale     —Formats the x- and y-scales.
    Digital Display         —Displays the numeric value of the chart. This option applies only to waveform charts.
  */

    explicit CGraph(QWidget *parent = 0);
    ~CGraph();

    CGraphPlot * addPlot(const QColor &color = QColor(), const QList<double> &values = QList<double>());

    friend CGraphPlot;
signals:

private slots:
    void slotAlarmTimer();

public slots:

private:
    typedef struct
    {
        double gap; /* расстояние между засечками */
        double size; /* размер засечки */
    } gap_t;

    void gap_calc(
            double centerx,
            double w,
            const gap_t * gap,
            int * gap_num,
            double * x,
            double * y1,
            double * y2
    );
    void P_grid_draw();
    void resizeEvent(QResizeEvent *event);
    void P_deleteItemsFromGroup(QGraphicsItemGroup *group);

    QList<CGraphPlot*> plots;

    QGraphicsScene * scene;

    QTimer              *timer;

    QGraphicsItemGroup   *drawedGrid;
    QGraphicsItemGroup   *drawedPathesGroup;

    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;


    double start_x;
    double start_y;

    gap_t gap_x_major;
    gap_t gap_x_minor;

    gap_t gap_y_major;
    gap_t gap_y_minor;

};

#endif // CGRAPH_H
