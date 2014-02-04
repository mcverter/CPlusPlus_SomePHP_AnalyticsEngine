/**************************
Filename:  KbpfAnalyzer.cpp
Author:  Mitchell Verter

  kbpfAnalyzer is the engine for analysing kpbf log files.
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

void KbpfAnalyzer::initializeAnalysis(
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
****************************/



#include "KbpfAnalyzer.h"


KbpfAnalyzer::KbpfAnalyzer()
{
}

// Initialize Must be called before one creates a graph
// in order to properly set all the variables
void KbpfAnalyzer::initializeAnalysis(
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
    QString timeFormat,
    QDateTime lastTime,
    int width,
    int height
    )
{
    // kbpf file locations
    m_analyticsDirPath = directory;
    m_analyticsFileBaseName = filebase;


    // xml headers
    m_docType = docType;
    m_tagRoot = rootTag;

    // xml entity
    m_tagElement = elementTag;

    // xml attributes
    m_attributeNameProject = projectAttributeName;
    m_attributeNameUserID = userIDAttributeName;

    // for setting and parsing times
    m_lastTime = lastTime;
    m_timeFormat = timeFormat;
    m_attributeNameTimeStart = timeStartAttributeName;
    m_attributeNameTimeEnd = timeEndAttributeName;


    // for graph display
    m_grapher = new AnalyticsGrapher();
    m_statisticName = statName;
    m_graphWidth = width;
    m_graphHeight=height;
}


// Detects trends in data, creating graphable data pairs
//  To access the graph, user must also call getGraph()
bool KbpfAnalyzer::Analyze(AnalysisType analysistype,
                           TimePeriodType timeperiodtype,
                           int timeQuantity,
                           AggregationType aggregationType)
{

    // find time boundaries
    uint maxTime = MaxTime(timeperiodtype);
    uint minTime = MinTime(maxTime, timeperiodtype, timeQuantity);


    // Initialize Arrays for Recording Data
    int num_data_points = numDataPoints(aggregationType, timeperiodtype, timeQuantity);
    // so we can start from 1 instead of 0
    num_data_points += 1;

    int events[num_data_points];
    int durations[num_data_points];
    for (int i=0; i<num_data_points;i++)
      {
	events[i]=0;
	durations[i]=0;
      }


    // Enter directory containing kpbf files and get list of files
    QDir analytics_dir(m_analyticsDirPath);
    analytics_dir.cd(m_analyticsDirPath);

    QStringList xml_file_paths= analytics_dir.entryList(
                QStringList(m_analyticsFileBaseName+"*kbpf"),
                QDir::Files,QDir::Time);


    // Process files
    for (int i=0; i<xml_file_paths.size(); i++)
    {
        QString xml_path = m_analyticsDirPath + "/" + xml_file_paths[i];

        // Do not process any files modified before mintime
        if (QFileInfo(xml_path).lastModified().toTime_t()
                < minTime)
        {continue;}


        // process files

        m_analyticsFile.setFileName(xml_path);
        m_readStream.setDevice(&m_analyticsFile);

        if(!m_analyticsFile.open(QIODevice::ReadOnly))
        {
            emit AnalysisError("ERROR: Failed opening Analytics File for Reading:  " +  m_analyticsFile.fileName());
            return FALSE;
        }

        while (!m_readStream.atEnd() &&
               !m_readStream.hasError())
        {
            QXmlStreamReader::TokenType token =  m_readStream.readNext();
            if(token == QXmlStreamReader::StartElement)
            {
                if(m_readStream.name() == m_tagElement)
                {
                    QXmlStreamAttributes attributes = m_readStream.attributes();
                    m_valueUserID = attributes.value(m_attributeNameUserID).toString();
                    m_valueProject = attributes.value(m_attributeNameProject).toString();


                    // time handling is a bit tricky
                    // default time representation is seconds since epoch
                    // but user can specify time representation
                    m_valueTimeStart = attributes.value(m_attributeNameTimeStart).toString();
                    m_valueTimeEnd = attributes.value(m_attributeNameTimeEnd).toString();

                    if ( m_valueTimeStart==NULL)
                    {
                        emit AnalysisError("ERROR: No start time given for " +  m_tagElement);
                        return FALSE;
                    }
                    if (m_valueTimeEnd==NULL)
                    {
                        emit AnalysisError("ERROR: No end time given for " +  m_tagElement);
                        return FALSE;
                    }

                    // Assume Default Format is UInt.  Maybe change to "dd/MM/yyyy "hh:mm:ss AP"?
                    if (m_timeFormat == NULL)
                    {
                        m_startTime = m_valueTimeStart.toUInt();
                    }
                    else
                    {
                        m_startTime = (QDateTime::fromString(m_valueTimeStart,m_timeFormat)).toTime_t();
                    }

                    // end time can be "unknown"
                    // this is only important for calculating Duration
                    if (m_valueTimeEnd == "unknown")
                    {
                        if  (analysistype == DurationAverage)
                        {
                            continue;
                        }
                        else if (analysistype==EventCount)
                        {
                            m_exitTime=0;  // exit time does not matter
                        }
                    }
                    else
                    {
                        if (m_timeFormat==NULL)
                        {
                            m_exitTime = m_valueTimeEnd.toUInt();
                        }
                        else
                        {
                            m_exitTime = (QDateTime::fromString(m_valueTimeEnd,m_timeFormat)).toTime_t();
                        }


                    }

                    // We have to stop the data analysis once we have
                    // processed all of the relevant entries.
                    // However, events get logged upon EXIT, not upon ENTRY
                    if (m_startTime > (maxTime + MAXIMUM_POSSIBLE_DURATION))
                    {
                        break;
                    }

                    else
                    {
                        if ((m_startTime >=minTime) &&
                                (m_startTime <= maxTime))
                        {

                            int graphIndex=GraphIndex(aggregationType, timeperiodtype, num_data_points, m_startTime, minTime);


                            events[graphIndex]++;
                            durations[graphIndex] += (m_exitTime-m_startTime);

                        }
                    }
                }
            }

            if (m_readStream.hasError())
            {
                if (m_readStream.error() != m_readStream.PrematureEndOfDocumentError)
                {
                  emit AnalysisError("ERROR: Could not parse XML file: " +  m_readStream.errorString());
                  return FALSE;
                }
                else
                {
                    emit AnalysisWarning("WARNING:  Did not find XML end tag.");
                }
            }
        }
            m_analyticsFile.close();

    }


    // now we start preparing output for the graph;
    int * graphYValues;

    if (analysistype==EventCount)
    {  graphYValues = events ;}
    else if (analysistype==DurationAverage)
    {
        graphYValues = new int [num_data_points];
        for (int i=1; i<num_data_points;i++)
        {
            if (events[i]==0)
            {
                graphYValues[i]=0;
            }
            else
            {
                graphYValues[i]=durations[i]/events[i];
            }
        }
    }

    QString * graphXValues = GraphXValues(num_data_points, maxTime, aggregationType, timeperiodtype);

    QList<QPair<QString,QString> > graphPoints;
    makeGraphPoints(graphPoints, aggregationType, num_data_points, graphXValues, graphYValues);

    qDebug() << " ---> " << graphPoints.count() << "points" ;
    qDebug() << graphPoints << "\n";


    QString title =GraphTitle(analysistype, timeperiodtype, aggregationType, timeQuantity);
    QString xLabel=XLabel(aggregationType, timeperiodtype);
    QString yLabel=YLabel(analysistype);


    m_grapher->graphDataPoints(graphPoints,
                               m_graphWidth, m_graphHeight,
                    title,
                    xLabel,
                    yLabel );
    return TRUE;
}

// Returns the graph that was created in the analysis
QDeclarativeView * KbpfAnalyzer::getGraph()
{
    return m_grapher->getGraph();
}

// Change graph size
void KbpfAnalyzer::setGraphSize(int width, int height) {
    m_graphWidth=width;
    m_graphHeight=height;
}

// Public aliases for Analyze() function
bool KbpfAnalyzer::AnalyzeEventLastXHours(int numHours){
    return Analyze(EventCount, Hour, numHours, None);
}
bool KbpfAnalyzer::AnalyzeEventLastXWeeks(int numWeeks){
    return Analyze(EventCount, Week, numWeeks, None);
}
bool KbpfAnalyzer::AnalyzeDurationLastXHours(int numHours){
    return Analyze(DurationAverage, Hour, numHours, None);
}
bool KbpfAnalyzer::AnalyzeDurationLastXWeeks(int numWeeks){
    return Analyze(DurationAverage, Week, numWeeks, None);
}
bool KbpfAnalyzer::AnalyzeEventLastXWeeksAggregateByClockHour(int numWeeks){
    return Analyze(EventCount, Week, numWeeks, ClockHour);
}
bool KbpfAnalyzer::AnalyzeEventLastXWeeksAggregateByWeekday(int numWeeks){
    return Analyze(EventCount, Week, numWeeks, Weekday);
}
bool KbpfAnalyzer::AnalyzeDurationLastXWeeksAggregateByClockHour(int numWeeks){
    return Analyze(DurationAverage, Week, numWeeks, ClockHour);
}
bool KbpfAnalyzer::AnalyzeDurationLastXWeeksAggregateByWeekday(int numWeeks){
    return Analyze(DurationAverage, Week, numWeeks, Weekday);
}
bool KbpfAnalyzer::graphAvgSessionDurationLast24Hours(){
    return AnalyzeDurationLastXHours(24);
}
bool KbpfAnalyzer::graphSessionCountLast24Hours(){
    return AnalyzeEventLastXHours(24);
}
bool KbpfAnalyzer::graphAvgSessionDurationLast4Weeks(){
    return AnalyzeDurationLastXWeeks(4);
}
bool KbpfAnalyzer::graphSessionCountLast4Weeks(){
    return AnalyzeEventLastXWeeks(4);
}
bool KbpfAnalyzer::graphHourlyAvgSessionDurationLast4Weeks(){
    return AnalyzeDurationLastXWeeksAggregateByClockHour(4);
}
bool KbpfAnalyzer::graphDailyAvgSessionDurationLast4Weeks(){
    return AnalyzeDurationLastXWeeksAggregateByWeekday(4);
}
bool KbpfAnalyzer::graphHourlySessionCountLast4Weeks(){
    return AnalyzeEventLastXWeeksAggregateByClockHour(4);
}
bool KbpfAnalyzer::graphDailySessionCountLast4Weeks(){
    return AnalyzeEventLastXWeeksAggregateByWeekday(4);
}




// The Following Functions are used within Analyze
// They have been factored out for code-readability purposes
// because the specific funcitiong of each depends on the type
// of analysis being performed


int KbpfAnalyzer::numDataPoints(AggregationType at, TimePeriodType tpt, int timeQuantity)
{
    if (at == ClockHour)
    {
        return HOURS_PER_DAY;

    }
    else if (at == Weekday)
    {
        return DAYS_PER_WEEK;
    }
    else if (at == None)
    {
        if (tpt == Hour)
        {
            return timeQuantity;
        }
        else if (tpt == Week)
        {
            return timeQuantity * DAYS_PER_WEEK;
        }

    }


}

uint KbpfAnalyzer::MaxTime(TimePeriodType tpt)
{
    if (tpt == Hour)
    {
        return QDateTime(m_lastTime.date(),
                            QTime(m_lastTime.time().hour(),0)).toTime_t();
    }
    else if (tpt == Week)
    {
        return QDateTime(m_lastTime.date()).toTime_t();;
  }
}

uint KbpfAnalyzer::MinTime (uint maxTime, TimePeriodType tpt, int timeQuantity)
{
    if (tpt == Hour)
    {
        return (QDateTime::fromTime_t(maxTime).addSecs(
                       SECONDS_PER_HOUR*-1*timeQuantity)).toTime_t();
    }
    else if (tpt == Week)
    {
        return (QDateTime::fromTime_t(maxTime).addDays(
                        -1*timeQuantity*DAYS_PER_WEEK )).toTime_t();
    }

}

QString * KbpfAnalyzer::GraphXValues(int num_data_points, uint maxTime, AggregationType at, TimePeriodType tpt)
{

    QString * graphXValues = new QString [num_data_points];
    if (at == None)
        {
            if (tpt == Hour)
            {
                for  (int i=1; i<num_data_points; i++)
                {

                    QString clockHour =
                      ((QDateTime::fromTime_t(maxTime)).addSecs(3600*-1*i)).time().toString().split(":")[0] + ":00";
                    int daysOffset = i/24;
                    if (daysOffset==0)
                    {
                        graphXValues[i]=clockHour;
                    }
                    else
                    {
                        graphXValues[i]=clockHour + " -"+ QString::number(daysOffset)+"d";
                    }
                }
                return graphXValues;
            }
            else if (tpt == Week)
            {

                for  (int i=1; i<num_data_points; i++)
                {
                    graphXValues[i]=
                            (QDateTime::fromTime_t(maxTime)).addDays(-1*i).date().toString();
                }
                return graphXValues;
            }
        }
        else if (at == ClockHour)
        {

            graphXValues[1] = QString("00:00");
            graphXValues[2] = QString("01:00");
            graphXValues[3] = QString("02:00");
            graphXValues[4] = QString("03:00");
            graphXValues[5] = QString("04:00");
            graphXValues[6] = QString("05:00");
            graphXValues[7] = QString("06:00");
            graphXValues[8] = QString("07:00");
            graphXValues[9] = QString("08:00");
            graphXValues[10] = QString("09:00");
            graphXValues[11] = QString("10:00");
            graphXValues[12] = QString("11:00");
            graphXValues[13] = QString("12:00");
            graphXValues[14] = QString("13:00");
            graphXValues[15] = QString("14:00");
            graphXValues[16] = QString("15:00");
            graphXValues[17] = QString("16:00");
            graphXValues[18] = QString("17:00");
            graphXValues[19] = QString("18:00");
            graphXValues[20] = QString("19:00");
            graphXValues[21] = QString("20:00");
            graphXValues[22] = QString("21:00");
            graphXValues[23] = QString("22:00");
            graphXValues[24] = QString("23:00");
            return graphXValues;
        }
        else if (at == Weekday)
        {
            graphXValues[1] = QString("Mon");
            graphXValues[2] = QString("Tue");
            graphXValues[3] = QString("Wed");
            graphXValues[4] = QString("Thu");
            graphXValues[5] = QString("Fri");
            graphXValues[6] = QString("Sat");
            graphXValues[7] = QString("Sun");
            return graphXValues;
        }

}

int KbpfAnalyzer::GraphIndex(AggregationType aggregationType, TimePeriodType timeperiodtype, int num_data_points, uint starttime, uint minTime)
{
    if (aggregationType == None)
    {
        if (timeperiodtype == Hour)
        {
            return num_data_points -
                    (((starttime-minTime)/SECONDS_PER_HOUR)+1);
        }
        else if (timeperiodtype == Week)
        {
            return num_data_points -
                    (((starttime-minTime)/SECONDS_PER_DAY)+1);

        }
    }
    else if (aggregationType == ClockHour)
    {
      return ((QDateTime::fromTime_t(starttime)).time().hour())+1;
    }
    else if (aggregationType == Weekday)
    {
        return  (QDateTime::fromTime_t(starttime)).date().dayOfWeek();
    }
}

void KbpfAnalyzer::makeGraphPoints(QList<QPair<QString,QString> > & graphPoints, AggregationType at, int num_data_points, QString* graphXValues, int* graphYValues)
{

    if (at==Weekday)
      {

        graphPoints.append(qMakePair(graphXValues[6],
                                     QString::number(graphYValues[6])));
        graphPoints.append(qMakePair(graphXValues[5],
                                     QString::number(graphYValues[5])));
        graphPoints.append(qMakePair(graphXValues[4],
                                     QString::number(graphYValues[4])));
        graphPoints.append(qMakePair(graphXValues[3],
                                     QString::number(graphYValues[3])));
        graphPoints.append(qMakePair(graphXValues[2],
                                     QString::number(graphYValues[2])));
        graphPoints.append(qMakePair(graphXValues[1],
                                     QString::number(graphYValues[1])));
        graphPoints.append(qMakePair(graphXValues[7],
                                     QString::number(graphYValues[7])));

      }
    else     if (at==ClockHour)
      {
        for (int i=num_data_points-1; i>=1;i--)
          {
            graphPoints.append(qMakePair(graphXValues[i],
                                         QString::number(graphYValues[i])));
          }
      }
    else
      {
        for (int i=1; i<num_data_points;i++)
          {
            graphPoints.append(qMakePair(graphXValues[i],
                                         QString::number(graphYValues[i])));
          }
      }

}



QString KbpfAnalyzer::XLabel(AggregationType aggregationType, TimePeriodType timeperiodtype)
{
    if (aggregationType == ClockHour)
    {
        return "Clock Hour";
    }
    else if (aggregationType == Weekday)
    {
        return "Weekday";

    }
    else if (aggregationType == None)
    {
        if (timeperiodtype == Hour)
        {
            return "Time (hour)";
        }
        else if (timeperiodtype == Week)
        {
            return "Date";
        }

    }
}
QString KbpfAnalyzer::YLabel(AnalysisType analysistype)
{

    if (analysistype==EventCount)
    {
        return  m_statisticName + "\n Total Count ";
    }
    else if (analysistype==DurationAverage)
    {
        return m_statisticName + "\n Average Duration ";
    }
}

QString KbpfAnalyzer::GraphTitle(AnalysisType analysistype, TimePeriodType timeperiodtype, AggregationType aggregationtype, int timeQuantity)
{
    QString title = m_statisticName;

    if (analysistype==EventCount)
    {
        title += ": Total Count ";
    }
    else if (analysistype==DurationAverage)
    {
        title += ": Average Duration ";
    }
    if (timeperiodtype == Hour)
    {
        title += " -- Last " + QString::number(timeQuantity) + " Hours";
    }
    else if (timeperiodtype == Week)
    {
        title += " -- Last " + QString::number(timeQuantity) + " Weeks";

    }
    title += "Since " + m_lastTime.toString();
    if (aggregationtype == ClockHour)
    {
        title += " (Aggregated by TOD) ";
    }
    else if (aggregationtype == Weekday)
    {
        title += " (Aggregated by Day) ";
    }
    return title;
}
