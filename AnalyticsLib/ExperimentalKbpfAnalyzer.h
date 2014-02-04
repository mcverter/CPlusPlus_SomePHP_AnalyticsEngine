/**************************
Filename:  ExperimentalKbpfAnalyzer.h
Author:  Mitchell Verter

  ExperimentalKbpfAnalyzer is the engine for analysing kpbf log files.
  A log file consists of XML entries of the form:

  <Element id="idValue" project="projectValue start="startValue" end="endValue" />

  A log file consists of a list of these Elements.

  Each Element will contain the following 4 attributes:
  (1)  User ID
  (2)  Project Name
  (3)  Start Time
  (4)  End Time

  The name for these attributes can be set in the initializer.
  The Date format can also be set in the initializer
  "End Time" can take the special value "unknown"


  FUNCTIONS
  ==========================================
Before an analysis can be performed, the analyzer must be initialized.

void ExperimentalKbpfAnalyzer::initializeAnalysis(
    QString directory,  // Directory where log files are stored
    QString filebase,   // Basename for log files
    QString docType,    // Doctype of log file
    QString rootTag,    // Root tag of log file
    QString elementTag,  // Name of Element
    QString projectAttributeName,  // Name of Project Attribute
    QString userIDAttributeName,   // Name of UserID Attribute
    QString timeStartAttributeName,// Name of Start Attribute
    QString timeEndAttributeName,  // Name of End Attribute
    QString timeFormat,   // Format of Time Entries
                          // If NULL, assumed to be String representation of
                          // uint POSIX timestamp
    QDateTime lastTime,   // Time from which to start analysis
                          // If NULL, assumed to be currentTime
    QString statName,     // For Graph Titles -- name of statistic
    int width,            // For Graph
    int height            // For Graph
    )



  ==========================================
  The most important function is the private function Analyze()

    void Analyze
        (AnalysisType analysistype,
        TimePeriodType timeperiodtype,
        int timeQuantity,
        AggregationType aggregationType);

  This will generate a graph based on the the kpbf log files.

  The following enumerated types are used as inputs to te Analyze() function

 (1) AnalysisType :  EventCount or DurationLength
 (2) TimePeriodType: Hourly or Weekly
    * Hourly will have data points for each Hour
    * Weekly will have data points for each day
    * "Daily" / "Monthly" / "Yearly" are not yet handled

(3) AggregationType:  None, Hourly, or Weekday
    * For now, this is only relevant for "Weekly" Analyses
    *  None: As stated above, data points for each day
    *  ClockHour:  Data points for each hour of Clock (00:00-23:00)
    * Weekday:  Data points for each Day of the Week  (Sun-Sat)



==================================================

  All public Analysis functions are wrappers around the Private Analyze() function

  For example,graphHourlyAvgSessionDurationLast4Weeks() is a wrapper for

    void Analyze (DurationLength, Weekly, 4, ClockHour);



    ==============================================

    Note that Analyze() only creates the graph but does not return it.
    To access the graph, getGraph() must be called.
****************************/


#ifndef ExperimentalKbpfAnalyzer_H
#define ExperimentalKbpfAnalyzer_H

#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include "AnalyticsGrapher.h"
#include <QDebug>
#include "AnalyticsUtils.h"
#include <QObject>

class ExperimentalKbpfAnalyzer  : public QObject
{
        Q_OBJECT
public:

    ExperimentalKbpfAnalyzer();

    void initializeAnalysis(
                            QString statName,
                            QString directory,
                            QString filebase,
                            QString docType,
                            QString rootTag,
                            QString elementTag,
                            QString projectAttributeName,
                            QString userIDAttributeName,
                            QString timeStartAttributeName,
                            QString timeEndAttributeName,
                            QString timeFormat= NULL,
                            QDateTime lastTime = QDateTime::currentDateTime(),
                            int width=AnalyticsGrapher::DEFAULTWIDTH,
                            int height=AnalyticsGrapher::DEFAULTHEIGHT
                            );

    void setGraphSize(int width, int height);
    QDeclarativeView * getGraph();


    enum AnalysisType {
        EventCount,
        DurationAverage
    };

    enum TimePeriodType {
        Hour,
        Week
    };

    enum AggregationType {
        None,
        ClockHour,
        Weekday
    };

    bool Analyze(AnalysisType analysistype,
             TimePeriodType timeperiodtype,
             int timeQuantity,
             AggregationType aggregationType);

    bool AnalyzeEventLastXHours(int numHours);
    bool AnalyzeEventLastXWeeks(int numWeeks);
    bool AnalyzeEventLastXWeeksAggregateByClockHour(int numWeeks);
    bool AnalyzeEventLastXWeeksAggregateByWeekday(int numWeeks);

    bool AnalyzeDurationLastXHours(int numHours);
    bool AnalyzeDurationLastXWeeks(int numWeeks);
    bool AnalyzeDurationLastXWeeksAggregateByClockHour(int numWeeks);
    bool AnalyzeDurationLastXWeeksAggregateByWeekday(int numWeeks);

    bool graphAvgSessionDurationLast24Hours();
    bool graphSessionCountLast24Hours();
    bool graphAvgSessionDurationLast4Weeks();
    bool graphSessionCountLast4Weeks();
    bool graphHourlyAvgSessionDurationLast4Weeks();
    bool graphDailyAvgSessionDurationLast4Weeks();
    bool graphHourlySessionCountLast4Weeks();
    bool graphDailySessionCountLast4Weeks();




signals:
    void AnalysisError(QString);
    void AnalysisWarning(QString);

private:
    // The following functions are used within "Analyze"
    uint MaxTime(TimePeriodType);
    uint MinTime (uint, TimePeriodType, int);
    int numDataPoints(AggregationType, TimePeriodType, int);
    QString * GraphXValues(int, uint, AggregationType, TimePeriodType);
    void makeGraphPoints(QList<QPair<QString,QString> > & ,
                         AggregationType, int, QString* , int* );
    int GraphIndex(AggregationType, TimePeriodType, int, uint, uint );
    QString GraphTitle(AnalysisType, TimePeriodType, AggregationType, int);
    QString XLabel(AggregationType, TimePeriodType);
    QString YLabel(AnalysisType);


    QString m_statisticName;
    QString m_analyticsDirPath;
    QString m_analyticsFileBaseName;

    QFile m_analyticsFile;
    QXmlStreamReader m_readStream;

    QString m_docType;
    QString m_tagRoot;
    QString m_tagElement;

    QString m_attributeNameProject;
    QString m_attributeNameUserID;
    QString m_attributeNameTimeEnd;
    QString m_attributeNameTimeStart;

    QString m_valueProject;
    QString m_valueUserID;
    QString m_valueTimeEnd;
    QString m_valueTimeStart;


    QString m_timeFormat;
    uint m_startTime;
    uint m_exitTime;
    QDateTime m_lastTime;




    AnalyticsGrapher * m_grapher;
    int m_graphWidth;
    int m_graphHeight;


    static const int SECONDS_PER_HOUR = 60*60;
    static const int DAYS_PER_WEEK = 7;
    static const int MAXIMUM_POSSIBLE_DURATION = 2 * SECONDS_PER_HOUR;
    static const int HOURS_PER_DAY = 24;
    static const int SECONDS_PER_DAY = HOURS_PER_DAY * SECONDS_PER_HOUR;
};



#endif // ExperimentalKbpfAnalyzer_H
