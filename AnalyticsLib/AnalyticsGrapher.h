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

#ifndef ANALYTICSGRAPHER_H
#define ANALYTICSGRAPHER_H

#include <QObject>
#include <QtDeclarative>

class AnalyticsGrapher : public QObject
{
    Q_OBJECT
public:
    explicit AnalyticsGrapher(QObject *parent = 0);

    // creates graph but does not dislay it
    QDeclarativeView * graphDataPoints(
            QList<QPair<QString,QString> > dataPoints,
            int graphWidth, int graphHeight,
            QString graphTitle,
            QString xAxisLabel, QString yAxisLabel);



    // Displays graph
  static void showGraph(QDeclarativeView * graph);
  QDeclarativeView * getGraph();

  static const int DEFAULTWIDTH = 700;
  static const int DEFAULTHEIGHT = 200;


signals:
  void graphProblem(QString message);

private:
  int getMax(QList<QString>);
  void setGraphTitle(QString graphTitle);
  void setGraphXTitle(QString axesTitle);
  void setGraphYTitle(QString axesTitle);
  void setGraphNodeColor(QString color);
  void setGraphXMin(QString xMin);
  void setGraphXMax(QString xMax);
  void moveGraph(int x, int y);

  static void plotGraph(QDeclarativeView *view);


  enum Axes {
        xAxis, yAxis
    };

    QDeclarativeView * view;
    QObject *item;
    QString m_nodeColor;
};

#endif // ANALYTICSGRAPHER_H

