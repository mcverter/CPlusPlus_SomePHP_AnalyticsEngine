/****************************
  This class implements the User Interface for the Analytics Engine.

  Using this interface, the user can graph an analysis based upon the contents of a datafile.

  The top portion of the Interface allows te User to
  (1)  Choose a Data File
  (2)  Choose either
    ..(a) "Total"
    ..(b)"Average"

  --
  The second part of the GUI allows the user to specify the time period for graphing.

(1)  Choose terminal date of analysis:
..(a) NOW
..(b) DateTime previous to NOW
(2)  Choose a preceding Time Period:
..(a) X Weeks
..(b) X Hours
(3) If Weeks, choose aggregation time
..(a) None
..(b) Clock Hour
..(c) WeekDay
----

The third part of the GUI displays the graphs.
(1)  A graph with a given set of parameters (end date, x weeks, aggregation type, etc) can only displayed once
(2)  Choosing a new datafile will erase all of the currently drawn graps
-----

The fourth part of the GUI allows the user to perform file conversions
(1)  CSV to KBPF
(2)  KBPF to CSV


  ***************************/


#ifndef ANALYTICSGUI_H
#define ANALYTICSGUI_H
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QGraphicsView>
#include <QDateTimeEdit>
#include "KbpfAnalyzer.h"
#include "AnalyticsUtils.h"


class AnalyticsGUI : public QWidget
{
    Q_OBJECT
public:
    explicit AnalyticsGUI(QWidget *parent = 0);



signals:

public slots:
    void generateGraphSlot();
    void timePeriodChangedSlot(QString);
    void csvToKbpfConversionSlot(QString);
    void kbpfToCsvConversionSlot(QString);
    void displayErrorMessageSlot(QString);
    void showLastDateTimePickerSlot(int);

private:
    void showGraph(QDeclarativeView *);
    void clearHistory();
    bool  graphAlreadyDrawn(QDateTime, KbpfAnalyzer::AnalysisType, KbpfAnalyzer::TimePeriodType,
                            KbpfAnalyzer::AggregationType, int);


    // Object Handles for algorithmic implementations
    AnalyticsUtils * m_fileConverter;
    KbpfAnalyzer * m_analyzer;

    // error dialogue
    QErrorMessage * m_errorDialog;


    // GUI Elements
    QFileDialog * m_kbpfSelectorDialog;
    QPushButton * m_kbpfChooseFileBtn;
    QLineEdit * m_filenameDisplay;
    QComboBox * m_analysisTypeSelector;

    // time
    QLineEdit * m_timeQuantityInput;
    QComboBox * m_lastTimeSelector;
    QDateTimeEdit * m_lastTimePicker;
    QLabel * m_aggregationTypeLabel;
    QComboBox * m_aggregationTypeSelector;
    QComboBox * m_periodTypeSelector;

    // Main Action Button
    QPushButton * m_generateGraphBtn;

    // file conversion
    QFileDialog * m_csvToKbpfDialog;
    QFileDialog * m_kbpfToCsvDialog;
    QPushButton * m_csvToKbpfBtn;
    QPushButton * m_kbpfToCsvBtn;

    // display area
    QScrollArea * m_scrollArea;
    QGraphicsView * m_chartView;
    QGraphicsScene * chartDisplay;
    QVBoxLayout * m_chartLayout;

    // keeping track of what graphs are displayed
    struct GraphType {
       QDateTime lastTime;
        KbpfAnalyzer::AnalysisType anType;
        KbpfAnalyzer::TimePeriodType tpt;
        KbpfAnalyzer::AggregationType aggType;
        int timeQty ;
    };

    QString graphedFile;
    QVector <GraphType> drawnGraphs;



    static const bool FOOTTRAFFICONLY;
    static const QString FOOTBASENAME;
    static const QString FOOTDIR;
};

#endif // ANALYTICSGUI_H
