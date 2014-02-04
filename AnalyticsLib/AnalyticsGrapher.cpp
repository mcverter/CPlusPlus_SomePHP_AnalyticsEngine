/**********************************
  Class:  AnalyticsGrapher

  This class is used to handle all of the graphing functionaity of the analytics engine.
  It has been made a separate class in order to decouple analysis from display.

  Almost all of this code was copied from Billy's original design.

  The primary interface to the grapher is the function

void graphDataPoints(
    QList<QPair<QString,QString> > dataPoints,
    int graphWidth,
    int graphHeight,
    QString graphTitle,
    QString xAxisLabel,
    QString yAxisLabel);




  *********************************/


#include "AnalyticsGrapher.h"


QDeclarativeView * AnalyticsGrapher::getGraph()
{
    return view;
}

AnalyticsGrapher::AnalyticsGrapher(QObject *parent) :
    QObject(parent)
{
    m_nodeColor = "#3cf";
}

void AnalyticsGrapher::setGraphXTitle(QString axesTitle)
{
    item->setProperty("xName", axesTitle);
}

void AnalyticsGrapher::setGraphYTitle(QString axesTitle)
{
        item->setProperty("yName", axesTitle);

}

void AnalyticsGrapher::setGraphTitle(QString graphTitle)
{
        item->setProperty("graphTitle", graphTitle);

}

void AnalyticsGrapher::setGraphNodeColor(QString color)
{
        m_nodeColor = color;
}

void AnalyticsGrapher::setGraphXMin(QString xMin)
{
        item->setProperty("xMin", xMin);

}

void AnalyticsGrapher::setGraphXMax(QString xMax)
{
        item->setProperty("xMax", xMax);
}


int AnalyticsGrapher::getMax(QList<QString> intStringList)
{
    int maxValue = 0;
    for(int i=0; i<intStringList.length(); i++)
        maxValue = qMax(maxValue, intStringList[i].toInt());
    return maxValue;
}


QDeclarativeView * AnalyticsGrapher::graphDataPoints(QList<QPair<QString, QString> > dataPoints, int graphWidth, int graphHeight,
                                     QString graphTitle, QString xAxisLabel, QString yAxisLabel)
{
    view = new QDeclarativeView();
    view->setFixedSize(graphWidth, graphHeight);
    view->setSource(QUrl::fromLocalFile("qml/graph.qml"));
    item = view->rootObject();

    //set up x-axis points
    QVariantList xAxisPoints;
    for(int i=dataPoints.count()-1; i>-1; i--)
        xAxisPoints.append(dataPoints[i].first);
    QMetaObject::invokeMethod(item, "setAxis",Q_ARG(QVariant,"x"), Q_ARG(QVariant,xAxisPoints));

    //set up y-axis points
    QVariantList yAxisPoints;
    QList<QString> yPoints;
    for(int i=0; i<dataPoints.count(); i++)
        yPoints.append(dataPoints[i].second);
    int maxY = getMax(yPoints);
    for(int i = 0; i <= maxY; i++)
        yAxisPoints << QString::number(i);
    QMetaObject::invokeMethod(item, "setAxis",Q_ARG(QVariant,"y"), Q_ARG(QVariant,yAxisPoints));

    //set graph dimension
    QMetaObject::invokeMethod(item, "setDimension", Q_ARG(QVariant,graphWidth), Q_ARG(QVariant,graphHeight));

    //set up list of points
    QString pointName = "";
    QString xVal = "";
    QString yVal = "";

    for(int i=dataPoints.count()-1; i>-1; i--) {
        xVal = dataPoints[i].first;
        yVal = dataPoints[i].second;
        QMetaObject::invokeMethod(item, "createGraphPoint",
                                  Q_ARG(QVariant, pointName),
                                  Q_ARG(QVariant, m_nodeColor),
                                  Q_ARG(QVariant, xVal),
                                  Q_ARG(QVariant, yVal));
    }

    setGraphTitle(graphTitle);
    setGraphXTitle(xAxisLabel);
    setGraphYTitle(yAxisLabel);
    setGraphXMin(dataPoints.at(dataPoints.count()-1).first);
    setGraphXMax(dataPoints.at(0).first);

    return view;
}

void AnalyticsGrapher::showGraph(QDeclarativeView * graph)
{
    graph->show();
    QMetaObject::invokeMethod(graph->rootObject(), "plotPoints");
}



void AnalyticsGrapher::plotGraph(QDeclarativeView *view)
{
    QMetaObject::invokeMethod(view->rootObject(), "plotPoints");
}

void AnalyticsGrapher::moveGraph(int x, int y)
{
        view->move(x,y);

}
