#include "cgraph.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include <QtMath>

#define UDATE_TIME_MS 50


CGraphPlot::CGraphPlot(CGraph *owner_, const QColor &color_, const QList<double> &values_)
{
    owner = owner_;
    color = color_;
    values = values_;
}

CGraphPlot::~CGraphPlot()
{

}

void CGraphPlot::updatePlotValues(const QList<double> & values_)
{
    values = values_;
    if(owner != nullptr)
    {
        owner->slotAlarmTimer();
    }
}


CGraph::CGraph(QWidget *parent)
    : QGraphicsView(parent)
{

    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
    //this->setAlignment(Qt::AlignCenter);                        // Делаем привязку содержимого к центру
    //this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    // Растягиваем содержимое по виджету

    this->setAlignment(Qt::


                       //AlignLeft
                       //AlignRight
                       //AlignHCenter
                       //AlignJustify
                       //AlignAbsolute
                       //AlignHorizontal_Mask

                       //AlignTop
                       //AlignBottom
                       //AlignVCenter
                       //AlignBaseline
                       //AlignVertical_Mask

                       AlignCenter

                       );

    this->setMinimumHeight(100);
    this->setMinimumWidth(100);

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    QBrush brush;
    brush.setColor(QColor(Qt::black));
    brush.setStyle(Qt::SolidPattern);
    scene->setBackgroundBrush(brush);

    drawedGrid = new QGraphicsItemGroup();
    scene->addItem(drawedGrid);

    drawedPathesGroup = new QGraphicsItemGroup();
    scene->addItem(drawedPathesGroup);

    timer = new QTimer();               // Инициализируем Таймер
    timer->setSingleShot(true);
    // Подключаем СЛОТ для отрисовки к таймеру
    connect(timer, SIGNAL(timeout()), this, SLOT(slotAlarmTimer()));
    timer->start(UDATE_TIME_MS);                   // Стартуем таймер на UDATE_TIME_MS миллисекунд


    gap_x_major.gap = 20.0;
    gap_x_major.size = 10.0;

    gap_x_minor.gap = 10.0;
    gap_x_minor.size = 5.0;

    gap_y_major.gap = 20.0;
    gap_y_major.size = 10.0;

    gap_y_minor.gap = 10.0;
    gap_y_minor.size = 5.0;


}

CGraph::~CGraph()
{
    delete scene;
}

void CGraph::gap_calc(
        double centerx,
        double w,
        const gap_t * gap,
        int * gap_num,
        double * x,
        double * y1,
        double * y2
)
{
    *gap_num = w / gap->gap;
    *x = centerx - ((*gap_num) / 2 * gap->gap);
    *y1 = -gap->size / 2.0;
    *y2 = gap->size / 2.0;
}

void CGraph::P_grid_draw()
{
    this->P_deleteItemsFromGroup(drawedGrid);

    int w = this->width();
    int h = this->height();





    QPen pen;
    int i;
    int gap_num;
    qreal x;
    qreal y1;
    qreal y2;
    qreal y;
    qreal x1;
    qreal x2;

    /* main cross */
    pen.setColor(Qt::green);

    x = 0.0;
    y1 = 0.0;
    y2 = h;
    drawedGrid->addToGroup(
                scene->addLine(start_x + x, y1, start_x + x, y2, pen)
                );
    y = 0.0;
    x1 = 0.0;
    x2 = w;
    drawedGrid->addToGroup(
                scene->addLine(x1, start_y + y, x2, start_y + y, pen)
                );

    /* minor */
    pen.setColor(Qt::darkGreen);
    gap_calc(start_x, w, &gap_x_minor, &gap_num, &x, &y1, &y2);
    for(i = 0; i < gap_num; ++i)
    {
        x += gap_x_minor.gap;
        drawedGrid->addToGroup(
                    scene->addLine(x, start_y + y1, x, start_y + y2, pen)
                    );
    }

    gap_calc(start_y, h, &gap_y_minor, &gap_num, &y, &x1, &x2);
    for(i = 0; i < gap_num; ++i)
    {
        y += gap_y_minor.gap;
        drawedGrid->addToGroup(
                    scene->addLine(start_x + x1, y, start_x + x2, y,  pen)
                    );
    }

    /* major */
    pen.setColor(Qt::green);
    gap_calc(start_x, w, &gap_x_major, &gap_num, &x, &y1, &y2);
    for(i = 0; i < gap_num; ++i)
    {
        x += gap_x_major.gap;
        drawedGrid->addToGroup(
                    scene->addLine(x, start_y + y1, x, start_y + y2, pen)
                    );
    }

    gap_calc(start_y, h, &gap_y_major, &gap_num, &y, &x1, &x2);
    for(i = 0; i < gap_num; ++i)
    {
        y += gap_y_major.gap;
        drawedGrid->addToGroup(
                    scene->addLine(start_x + x1, y, start_x + x2, y,  pen)
                    );
    }

}

CGraphPlot * CGraph::addPlot(const QColor &color, const QList<double> & values)
{
    CGraphPlot *plot = new CGraphPlot(this, color, values);

    plots.push_back(plot);

    slotAlarmTimer();

    return plot;
}

void CGraph::slotAlarmTimer()
{
    double cx = 0.0;
    double halfx = this->width() / 2;
    double cy = 0.0;
    double halfy = this->height() / 2;

    start_x = cx + halfx;
    start_y = cy + halfy;


    P_grid_draw();

    /* Удаляем все элементы со сцены,
     * если они есть перед новой отрисовкой
     * */
    this->P_deleteItemsFromGroup(drawedPathesGroup);

    int width = this->width();      // определяем ширину нашего виджета
    int height = this->height();    // определяем высоту нашего виджета

    /* Устанавливаем размер сцены по размеру виджета
     * Первая координата - это левый верхний угол,
     * а Вторая - это правый нижний угол
     * */
    scene->setSceneRect(0,0,width,height);


    /*********************/

    QWidget::update();
    double x;


    QPen pen;

    foreach(CGraphPlot * plot, plots)
    {
        x = 0.0;
        pen.setColor(plot->color);

        QPainterPath * path = new QPainterPath();

        bool first = true;

        foreach(double value, plot->values)
        {
            if(first)
            {
                first = false;
                path->moveTo(start_x + x, start_y - value);
            }
            else
            {
                path->lineTo(start_x + x, start_y - value);
            }
            x = x + gap_x_major.gap;
        }
        QGraphicsPathItem * pathItem = scene->addPath(*path, pen);

        drawedPathesGroup->addToGroup(pathItem);

        delete path;
    }


}


/* Этим методом перехватываем событие изменения размера виджет
 * */
void CGraph::resizeEvent(QResizeEvent *event)
{
    timer->start(UDATE_TIME_MS);   // Как только событие произошло стартуем таймер для отрисовки
    QGraphicsView::resizeEvent(event);  // Запускаем событие родителького класса
}


/**
 * Метод для удаления всех элементов из группы
 */
void CGraph::P_deleteItemsFromGroup(QGraphicsItemGroup *group)
{
    /**
     * Перебираем все элементы сцены, и если они принадлежат группе,
     * переданной в метод, то удаляем их
     */
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
        if(item->group() == group ) {
            delete item;
        }
    }
}
