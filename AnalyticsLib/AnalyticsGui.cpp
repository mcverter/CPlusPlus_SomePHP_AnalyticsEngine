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

#include "AnalyticsGui.h"
#include <QDateTime>


const QString AnalyticsGUI::FOOTBASENAME ="footTrafficAnalytics";
const QString AnalyticsGUI::FOOTDIR ="/home/dsion/curio_mitchell/AnalyticsLib/SampleKbpf/SampleFootTraffic";
const bool AnalyticsGUI::FOOTTRAFFICONLY = FALSE;

// Constructor:  Sets up the GUI
AnalyticsGUI::AnalyticsGUI(QWidget *parent) :
    QWidget(parent)
{

    // for error messages
    m_errorDialog = new QErrorMessage(this);

    // for file conversion
    m_fileConverter = new AnalyticsUtils();

    // for graphing
    m_analyzer = new KbpfAnalyzer();
    graphedFile="";


    //GUI
    QVBoxLayout * mainLayout = new QVBoxLayout;


    if (!FOOTTRAFFICONLY)
    {
        // TOP SECTION
        mainLayout->addWidget(new QLabel("Create Graph from Kbpf File:", this));

        // FILE CHOOSER
        QHBoxLayout * fileChoiceLayout = new QHBoxLayout;
        m_kbpfSelectorDialog = new QFileDialog(this, "Kbpf File",QDir::currentPath(),"*.kbpf");
        m_kbpfChooseFileBtn = new QPushButton("Choose File",this);
        m_filenameDisplay = new QLineEdit(this);
        m_filenameDisplay->setReadOnly(TRUE);
        QLabel * FileLabel = new QLabel("Analytics File:  ", this);
        fileChoiceLayout->addWidget(FileLabel);
        fileChoiceLayout->addWidget(m_kbpfChooseFileBtn);
        fileChoiceLayout->addWidget(m_filenameDisplay);
        mainLayout->addLayout(fileChoiceLayout);
    }


     // CHOOSE Average or Total
    QHBoxLayout * statChoiceLayout = new QHBoxLayout;
    m_analysisTypeSelector = new QComboBox(this);
    m_analysisTypeSelector->addItem("Total Count");
    m_analysisTypeSelector->addItem("Average Duration");
    QLabel * statTypeLabel = new QLabel("Statistic Type:  ");
    statChoiceLayout->addWidget(statTypeLabel);
    statChoiceLayout->addWidget(m_analysisTypeSelector);
    mainLayout->addLayout(statChoiceLayout);


    // Divider
    QFrame * separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(separator);

    // SECTION TWO:  Time
    mainLayout->addWidget(new QLabel("Select Time Period: "));


    // Choose Final Time
    QHBoxLayout * lastTimeLayout = new QHBoxLayout;
    QLabel * lastTimeLabel = new QLabel("From:  ");
    m_lastTimeSelector = new QComboBox (this);
    m_lastTimeSelector->addItem("Now");
    m_lastTimeSelector->addItem("Choose Date Time ... ");
    lastTimeLayout->addWidget(lastTimeLabel);
    lastTimeLayout->addWidget(m_lastTimeSelector);
    mainLayout->addLayout(lastTimeLayout);

    // Specify Final Date
    m_lastTimePicker = new QDateTimeEdit(this);
    m_lastTimePicker->setDisplayFormat("MM/dd/yyyy hh:00 AP");
    m_lastTimePicker->setCalendarPopup(TRUE);
    mainLayout->addWidget(m_lastTimePicker);
    m_lastTimePicker->hide();


    // Choose Previous Period
    QHBoxLayout * timeChoiceLayout = new QHBoxLayout;
    QLabel * timePeriodLabel = new QLabel("For the last:  ");
    m_timeQuantityInput = new QLineEdit(this);
    m_periodTypeSelector = new QComboBox (this);
    m_periodTypeSelector->addItem("Hours");
    m_periodTypeSelector->addItem("Weeks");
    timeChoiceLayout->addWidget(timePeriodLabel);
    timeChoiceLayout->addWidget(m_timeQuantityInput);
    timeChoiceLayout->addWidget(m_periodTypeSelector);
    mainLayout->addLayout(timeChoiceLayout);

    // Choose aggregation Type
    QHBoxLayout * aggregationChoiceLayout = new QHBoxLayout;
    m_aggregationTypeSelector = new QComboBox();
    m_aggregationTypeSelector->addItem("None");
    m_aggregationTypeSelector->addItem("Hour of Day");
    m_aggregationTypeSelector->addItem("Day of Week");
    m_aggregationTypeLabel = new QLabel("Aggregation Type:  ");
    aggregationChoiceLayout->addWidget(m_aggregationTypeLabel);
    aggregationChoiceLayout->addWidget(m_aggregationTypeSelector);
    m_aggregationTypeSelector->hide();
    m_aggregationTypeLabel->hide();
    mainLayout->addLayout(aggregationChoiceLayout);


    // Dividing Line
    QFrame * separator2 = new QFrame();
    separator2->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(separator2);


    // Button for generating graph
    m_generateGraphBtn = new QPushButton ("Generate Graph");
    mainLayout->addWidget(m_generateGraphBtn);

    // Splitter
    mainLayout->addWidget(new QSplitter());

    //Graphing Area
    m_chartView = new QGraphicsView();
    chartDisplay = new QGraphicsScene();
    m_chartView->setScene(chartDisplay);
    m_chartLayout = new QVBoxLayout;
    m_chartLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    m_chartView->setLayout(m_chartLayout);

    // Scroll bars for Graph Area
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidget(m_chartView);
    mainLayout->addWidget(m_scrollArea);

    if (!FOOTTRAFFICONLY)
    {
        // Splitter
        mainLayout->addWidget(new QSplitter());

        // File Cnversion utilities
        mainLayout->addWidget(new QLabel("File Conversion Utilities: ", this));
        m_csvToKbpfDialog = new QFileDialog(this, "Kbpf File",QDir::currentPath(),"*.csv");
        m_kbpfToCsvDialog = new QFileDialog (this, "Kbpf File",QDir::currentPath(),"*.kbpf");
        m_csvToKbpfBtn = new QPushButton (".csv to .kbpf", this);
        m_kbpfToCsvBtn = new QPushButton (".kbpf to .csv", this);
        mainLayout->addWidget(m_kbpfToCsvBtn);
        mainLayout->addWidget(m_csvToKbpfBtn);
    }
    setLayout(mainLayout);
    setWindowState(Qt::WindowMaximized);


    // Signals and slots
    if (!FOOTTRAFFICONLY)
    {
        connect(m_kbpfChooseFileBtn, SIGNAL(clicked()), m_kbpfSelectorDialog, SLOT(open()));
        connect(m_kbpfSelectorDialog, SIGNAL(fileSelected(QString)), m_filenameDisplay, SLOT(setText(QString)));
    }

    connect(m_generateGraphBtn, SIGNAL(clicked()), this, SLOT(generateGraphSlot()));
    connect(m_periodTypeSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(timePeriodChangedSlot(QString)));


    connect(m_analyzer, SIGNAL(AnalysisWarning(QString)), this, SLOT(displayErrorMessageSlot(QString)));
    connect(m_analyzer, SIGNAL(AnalysisError(QString)), this, SLOT(displayErrorMessageSlot(QString)));
    connect(m_lastTimeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(showLastDateTimePickerSlot(int)));


    if (!FOOTTRAFFICONLY)
    {
        connect(m_csvToKbpfBtn, SIGNAL(clicked()), m_csvToKbpfDialog, SLOT(open()));
        connect(m_kbpfToCsvBtn, SIGNAL(clicked()), m_kbpfToCsvDialog, SLOT(open()));
        connect(m_csvToKbpfDialog, SIGNAL(fileSelected(QString)), this, SLOT(csvToKbpfConversionSlot(QString)));
        connect(m_kbpfToCsvDialog, SIGNAL(fileSelected(QString)), this, SLOT(kbpfToCsvConversionSlot(QString)));
        connect(m_fileConverter, SIGNAL(FileConversionError(QString)), this, SLOT(displayErrorMessageSlot(QString)));
        connect(m_fileConverter, SIGNAL(FileConversionWarning(QString)), this, SLOT(displayErrorMessageSlot(QString)));
    }

}

// Generic Error/Message Dialogue
void AnalyticsGUI::displayErrorMessageSlot(QString msg)
{
    m_errorDialog->showMessage(msg);
}


// Shows DateTime Picker in response to Date Selector Dropdown
void AnalyticsGUI::showLastDateTimePickerSlot(int index)
{
    if (index==0)
    {
        m_lastTimePicker->hide();
    }
    else if (index==1)
    {
        m_lastTimePicker->setDateTime(QDateTime::currentDateTime());
        m_lastTimePicker->setMaximumDateTime(QDateTime::currentDateTime());
        m_lastTimePicker->show();
    }
}

// Shows Aggregation Selector if Weekly Time Period is chosen
void AnalyticsGUI::timePeriodChangedSlot(QString periodType)
{
    // no aggregation for hours
    if (periodType == "Hours")
    {
        m_aggregationTypeSelector->hide();
        m_aggregationTypeLabel->hide();
    }
    else
    {
        m_aggregationTypeSelector->show();
        m_aggregationTypeLabel->show();
    }
}


// Clears all the Graphs from the Display Area
void AnalyticsGUI::clearHistory()
{
    drawnGraphs.clear();
    QLayoutItem* item;
    while ( ( item = m_chartLayout->takeAt( 0 ) ) != NULL )
        {
            delete item->widget();
            delete item;
        }
}

// Draws graph in the chart area
void AnalyticsGUI::generateGraphSlot()
{

    QString filename;
    if (!FOOTTRAFFICONLY)
    {
        // check filename
        filename = m_filenameDisplay->text();
        if (filename==NULL || filename.length()<1)
        {
            m_errorDialog->showMessage("You must pick a file for analysis");
            m_kbpfChooseFileBtn->setFocus();

            return;
        }
    }
    else
    {
        filename = FOOTDIR + "/" + FOOTBASENAME;
    }

    // Check time quanity input
    QString timeQuantity = m_timeQuantityInput->text() ;
    if (timeQuantity==NULL || timeQuantity.length()<1)
    {
        m_errorDialog->showMessage("You must pick enter a number of " + m_periodTypeSelector->currentText() + " to analyze");
        m_timeQuantityInput->setFocus();

        return;
    }

    int tq = timeQuantity.toInt();
    if (tq < 1)
      {
          m_errorDialog->showMessage("You must pick enter a number of " + m_periodTypeSelector->currentText() + " to analyze");
          m_timeQuantityInput->setFocus();

          return;
      }


    // initialize Analysis Engine
        QFileInfo * info = new QFileInfo(filename);
        QString base = info->baseName();
        QString filetype = base.left(base.indexOf("_"));

        if (! m_fileConverter->doesTagSetExist(filetype))
        {
            m_errorDialog->showMessage("Could not find complete schema for Filetype " + filetype);
            return;
        }

        AnalyticsUtils::AnalysisTagSet tagSet = m_fileConverter->getTagSet(filetype);

        QDateTime lastTime;
        if (m_lastTimeSelector->currentIndex()==0)
        {
            lastTime = QDateTime::currentDateTime();
        }
        else
        {
            lastTime = m_lastTimePicker->dateTime();
        }

        m_analyzer->initializeAnalysis(
                    tagSet.statname,
                    info->absolutePath(),
                    info->baseName(),
                    tagSet.docType,
                    tagSet.rootTag,
                    tagSet.elementTag,
                    tagSet.projectTag,
                    tagSet.userIDTag,
                    tagSet.startTag,
                    tagSet.endTag,
                    tagSet.timeFormat,
                    lastTime,
                    AnalyticsGrapher::DEFAULTWIDTH,
                    AnalyticsGrapher::DEFAULTHEIGHT
                    );

    KbpfAnalyzer::AggregationType aggtype;
    KbpfAnalyzer::TimePeriodType timetype;
    KbpfAnalyzer::AnalysisType antype;


    QString anSelection = m_analysisTypeSelector->currentText();
    if (anSelection=="Total Count")
    {
        antype = KbpfAnalyzer::EventCount;
    }
    else if (anSelection== "Average Duration")
    {
        antype = KbpfAnalyzer::DurationAverage;
    }

    QString aggSelection = m_aggregationTypeSelector->currentText();
    if (aggSelection=="None")
    {
        aggtype = KbpfAnalyzer::None;
    }
    else if (aggSelection=="Hour of Day")
    {
        aggtype = KbpfAnalyzer::ClockHour;

    }
    else if (aggSelection=="Day of Week")
    {
        aggtype = KbpfAnalyzer::Weekday;

    }

    QString tpSelection = m_periodTypeSelector->currentText();
    if (tpSelection=="Hours")
    {
        timetype = KbpfAnalyzer::Hour;
        aggtype = KbpfAnalyzer::None;
    }
    else if (tpSelection=="Weeks")
    {
        timetype = KbpfAnalyzer::Week;
    }

    // Clear Graph Area if New Datafile
    if (filename != graphedFile)
    {
     graphedFile=filename;
     clearHistory();

    }
    // Don't redraw graphs
    if (! graphAlreadyDrawn(lastTime, antype, timetype, aggtype, tq))
    {
        // analyse and display
        if (m_analyzer->Analyze(antype, timetype, tq, aggtype))
        {
            QDeclarativeView * graph = m_analyzer->getGraph();
            showGraph(graph);
            drawnGraphs.append((GraphType){lastTime, antype,timetype,aggtype,tq});
        }
        else
        {
            m_errorDialog->showMessage("Could not analyze file");
        }

    }
    else
    {
        m_errorDialog->showMessage("Graph already drawn");
    }
}

// make sure that graph isn't aready displayed
bool  AnalyticsGUI::graphAlreadyDrawn(QDateTime lastTime, KbpfAnalyzer::AnalysisType anType, KbpfAnalyzer::TimePeriodType tpt,
                        KbpfAnalyzer::AggregationType aggType, int timeQty)
{
    for (int i=0; i< drawnGraphs.size(); i++)
    {
        GraphType gt = drawnGraphs.at(i);
        if (lastTime == gt.lastTime && anType == gt.anType && tpt == gt.tpt && aggType==gt.aggType && timeQty==gt.timeQty)
        {
            return TRUE;
        }
    }
    return FALSE;
}

// display graph
void AnalyticsGUI::showGraph(QDeclarativeView * graph)
{
    QMetaObject::invokeMethod(graph->rootObject(), "plotPoints");
    m_chartLayout->addWidget(graph);
}

// convert file format
void AnalyticsGUI::csvToKbpfConversionSlot(QString filename)
{
    if (m_fileConverter->convertCsvToKbpf(filename))
    {
        m_errorDialog->showMessage("Conversion Successful");
    }
}

// convert file format
void AnalyticsGUI::kbpfToCsvConversionSlot(QString filename)
{
    if (m_fileConverter->convertKbpfToCsv(filename))
    {
        m_errorDialog->showMessage("Conversion Successful");
    }
    else
    {
        m_errorDialog->showMessage("ERROR:  No CSV file was created");

    }
}

