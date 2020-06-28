#include <QSettings>
#include <QPainter>
#include <QMenu>
#include <QPaintEvent>
#include <math.h>
#include <stdlib.h>
#include <qmmp/qmmp.h>

#include "plusxrays.h"
#include "colorwidget.h"

PlusXRays::PlusXRays (QWidget *parent)
    : Visual(parent)
{
    setWindowTitle(tr("Plus XRays Widget"));
    setMinimumSize(2*300-30, 105);

    m_gridAction = new QAction(tr("Grid"), this);
    m_gridAction->setCheckable(true);
    connect(m_gridAction, SIGNAL(triggered(bool)), this, SLOT(changeGridState(bool)));

    readSettings();
}

PlusXRays::~PlusXRays()
{

}

void PlusXRays::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("PlusXRays");
    m_colors = ColorWidget::readColorConfig(settings.value("colors").toString());
    m_gridAction->setChecked(settings.value("show_grid", false).toBool());
    settings.endGroup();
}

void PlusXRays::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("PlusXRays");
    settings.setValue("colors", ColorWidget::writeColorConfig(m_colors));
    settings.setValue("show_grid", m_gridAction->isChecked());
    settings.endGroup();
}

void PlusXRays::changeColor()
{
    ColorWidget c;
    c.setColors(m_colors);
    if(c.exec())
    {
        m_colors = c.getColors();
    }
}

void PlusXRays::changeGridState(bool state)
{
    m_gridAction->setChecked(state);
    update();
}

void PlusXRays::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(e->rect(), Qt::black);
    draw(&painter);
}

void PlusXRays::contextMenuEvent(QContextMenuEvent *)
{
    QMenu menu(this);
    connect(&menu, SIGNAL(triggered(QAction*)), SLOT(writeSettings()));
    connect(&menu, SIGNAL(triggered(QAction*)), SLOT(readSettings()));

    menu.addAction(m_screenAction);
    menu.addSeparator();
    menu.addAction(tr("Color"), this, SLOT(changeColor()));
    menu.addAction(m_gridAction);
    menu.exec(QCursor::pos());
}

void PlusXRays::process(float *left, float *)
{
    const int rows = height();
    const int cols = width();

    if(m_rows != rows || m_cols != cols)
    {
        m_rows = rows;
        m_cols = cols;

        if(m_intern_vis_data)
        {
            delete[] m_intern_vis_data;
        }

        m_intern_vis_data = new int[m_cols]{0};
    }

    const int step = (QMMP_VISUAL_NODE_SIZE << 8) / m_cols;
    int pos = 0;

    for(int i = 0; i < m_cols; ++i)
    {
        pos += step;
        m_intern_vis_data[i] = int(left[pos >> 8] * m_rows / 2);
        m_intern_vis_data[i] = qBound(-m_rows / 2, m_intern_vis_data[i], m_rows / 2);
    }
}

void PlusXRays::draw(QPainter *p)
{
    if(m_gridAction->isChecked())
    {
        p->setPen(QPen(QColor(255, 255, 255, 50), 1));
        int per = width() / 8;

        for(int w=0; w<width(); ++w)
        {
            p->drawLine(QPoint(w*per, 0), QPoint(w*per, height()));
        }

        per = height() / 8;
        for(int h=0; h<height(); ++h)
        {
            p->drawLine(QPoint(0, h*per), QPoint(width(), h*per));
        }
    }

    QLinearGradient line(0, 0, 0, height());
    for(int i = 0; i < m_colors.count(); ++i)
    {
        line.setColorAt((i + 1) * 1.0 / m_colors.count(), m_colors[i]);
    }

    p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p->setPen(QPen(line, 1));

    for(int i = 0; i < m_cols; ++i)
    {
        if((i + 1) >= m_cols)
        {
            break;
        }

        int pFront = m_rows / 2 - m_intern_vis_data[i];
        int pEnd = m_rows / 2 - m_intern_vis_data[i + 1];

        if(pFront > pEnd)
        {
            qSwap(pFront, pEnd);
        }

        p->drawLine(i, pFront, i + 1, pEnd);
    }
}
