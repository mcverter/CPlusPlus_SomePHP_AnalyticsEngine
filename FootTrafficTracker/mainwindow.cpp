#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    kulbooks = new CurioKulBooks("/home/dsion/.represents/.represents.kbpf");
    initializeAnalyticsFile();

    enterTimes.resize(MAX_BODIES);
    kreader.start();
    xnFPSInit(&xnFPS, 180);

    km = new KinectMonitor(&kreader);
    km->start();


    connect(&kreader,SIGNAL(statusNotification(QKinect::KinectStatus)),this,SLOT(kinectStatusSlot(QKinect::KinectStatus)));
    connect(&kreader,SIGNAL(userNotification(unsigned,bool)),this,SLOT(kinectUserSlot(unsigned,bool)));
}


void MainWindow::kinectStatusSlot(QKinect::KinectStatus s)
{
    // if not started, exit with a code number and message
        if(s==QKinect::ErrorStop)
        {

            qDebug("ERROR:  Kinect is not properly attached to DSION Engine. \n\tFoot Traffic Data can not be collected\t");
            exit(55);
        }
        /* Other possible statuses
    if  (s==QKinect::Idle)
    if(s==QKinect::Initializing)
    if(s==QKinect::OkRun)
    */
}

void MainWindow::kinectUserSlot(unsigned id, bool found)
{
    if (found == TRUE)
          {
            QDateTime now = QDateTime::currentDateTime();
            enterTimes[id] = now.toString("MM/dd/yyyy hh:mm:ss");
            qDebug()<< "ENTERED: " << id << " TIME: " << now.toString("MM/dd/yyyy hh:mm:ss") <<" Active " << m_activeProject << "\n";
          }
        else
          {
            QDateTime now = QDateTime::currentDateTime();
            qDebug()<< "Entered" << enterTimes[id] << " LEFT: " << id << " TIME: " << now.toString("MM/dd/yyyy hh:mm:ss")  << " Active " << m_activeProject <<"\n";

            uint time_elapsed = now.toTime_t() - (QDateTime::fromString(enterTimes[id], "MM/dd/yyyy hh:mm:ss")).toTime_t();
            if (time_elapsed > MAX_POSSIBLE_DURATION)
            {
              writeUserToAnalyticsFile(id, m_activeProject, enterTimes[id],  "unknown");

            }
            else
            {
                writeUserToAnalyticsFile(id, m_activeProject, enterTimes[id],  now.toString("MM/dd/yyyy hh:mm:ss"));
            }
            enterTimes[id]="";
          }
}


void MainWindow::writeXMLtoCSV()
{

    QFile csv_output_file;
    QTextStream csv_write_stream;
    csv_output_file.setFileName("footTrafficAnalytics.csv");
    csv_write_stream.setDevice(&csv_output_file);

     if(!csv_output_file.open(QIODevice::ReadWrite))
     {
       qDebug("Failed opening CSV File for Writing.");
       return;
     }

     // change directory
  QString analyticsDirPath = QDir::currentPath() + "/FootTrafficFiles";
  QDir analytics_dir(analyticsDirPath);

  // get xml files 
  QString analyticsFileBaseName = "footTrafficAnalytics_"; // + m_activeProject;
  QStringList xml_file_paths= analytics_dir.entryList(
             QStringList(analyticsFileBaseName+"*"),
             QDir::Files,QDir::Name);

  for (int i=0; i<xml_file_paths.size(); i++)
    {
      QFile xml_input_file;
      QXmlStreamReader xml_read_stream;
      QString xml_path = analyticsDirPath + "/" + xml_file_paths[i];
      xml_input_file.setFileName(xml_path);
      xml_read_stream.setDevice(&xml_input_file);

      if(!xml_input_file.open(QIODevice::ReadOnly))
	{
	  qDebug("Failed opening Analytics File for Reading.");
	  return;
	}

      QString userID, enter, exit, project;
      while (!xml_read_stream.atEnd() &&
	     !xml_read_stream.hasError())
	{
	  QXmlStreamReader::TokenType token =  xml_read_stream.readNext();
	  if(token == QXmlStreamReader::StartElement)
	    {
	      if(xml_read_stream.name() == "User")
		{
		  QXmlStreamAttributes attributes = xml_read_stream.attributes();
		  userID = attributes.value("id").toString();
		  project = attributes.value("project").toString();
		  enter = attributes.value("entry").toString();
		  exit = attributes.value("exit").toString();
		  csv_write_stream << userID << "," << project << "," << enter << "," << exit << "\n";
		}
	    }
	}
      if (xml_read_stream.hasError())
	{
	  qDebug() << "Could not parse XML. " <<  xml_read_stream.errorString();
	  return;
	}
    }
    csv_write_stream.flush();
    csv_output_file.flush();
    csv_output_file.close();
}

void MainWindow::createNewAnalyticsFile(QString baseDir)
{
  QString fileName = m_analyticsFileBaseName + "_"
          + QString::number(QDateTime::currentDateTime().toTime_t())
          + ".kbpf";
  m_kbpfFile.setFileName(baseDir + "/" + fileName);
  if(!m_kbpfFile.open(QIODevice::ReadWrite))
    qDebug("Failed opening Analytics File for Writing.");

  m_stream.setAutoFormatting(true);
  m_stream.setDevice(&m_kbpfFile);
  m_stream.writeStartDocument();
  m_stream.writeDTD("<!DOCTYPE " + m_docType + ">");
  m_stream.writeStartElement(m_tagRoot);
  m_kbpfFile.flush();
}

void MainWindow::initializeAnalyticsFile()
{

  m_docType = "FootTrafficML";
  m_tagRoot = "FootTraffic";
  m_activeProject = kulbooks->activeKulBook();

  m_analyticsFileBaseName = "footTrafficAnalytics_"; // + m_activeProject;

  m_analyticsDirPath = QDir::currentPath() + "/FootTrafficFiles";

  QDir analytics_dir(m_analyticsDirPath);
  if (! analytics_dir.exists())
  {
      analytics_dir.mkdir(m_analyticsDirPath);
  }

  analytics_dir.cd(m_analyticsDirPath);
 QStringList xml_file_paths= analytics_dir.entryList(
             QStringList(m_analyticsFileBaseName+"*"),
             QDir::Files,QDir::Name|QDir::Reversed);
 analytics_dir.cdUp();

 if (xml_file_paths.size()<1)
   {
     createNewAnalyticsFile(m_analyticsDirPath);
   }
 else
   {
     QString file_path = xml_file_paths.at(0);
     m_kbpfFile.setFileName(m_analyticsDirPath + "/" + file_path);
     if (m_kbpfFile.size()>=MAX_FILE_SIZE)
       {
         createNewAnalyticsFile(m_analyticsDirPath);
       }
     else 
       {
	 if(!m_kbpfFile.open(QIODevice::ReadWrite))
	   qDebug("Failed opening Analytics File for Writing.");

	 // rewind file pointer to strip off closing tag
	 QString  endTag = "</" + m_tagRoot + ">\n";

         // check to see if the end of the file is the end tag
         m_kbpfFile.seek(m_kbpfFile.size()-endTag.size());

         // if not, set the write pointer to EOF
         if ((QString(m_kbpfFile.read(endTag.size()))).compare(endTag)!=0)
         {
                m_kbpfFile.seek(m_kbpfFile.size());

         }
         else
         {
             m_kbpfFile.seek(m_kbpfFile.size()-endTag.size());
         }


	 m_stream.setAutoFormatting(true);
	 m_stream.setDevice(&m_kbpfFile);	 
       }
   }
}

void MainWindow::writeUserToAnalyticsFile(int userID,
					  QString projectName,
					  QString entryTime,
					  QString exitTime)
{
  m_stream.writeStartElement("User");

  m_stream.writeAttribute("id", QString::number(userID));
  m_stream.writeAttribute("project", projectName);
  m_stream.writeAttribute("entry", entryTime);
  m_stream.writeAttribute("exit", exitTime);

  // End "User" tag
  m_stream.writeEndElement();

  m_kbpfFile.flush();

  if (m_kbpfFile.size()>=MAX_FILE_SIZE)
    {
      closeAnalyticsFile();
      createNewAnalyticsFile(m_analyticsDirPath);
    }

}
void MainWindow::handleApplicationCloseSlot()
{
  //  write out remaining users
    for (int i=0; i<MAX_BODIES; i++)
    {
        if (enterTimes[i]!= "" &&
                enterTimes[i] != NULL)
        {
            writeUserToAnalyticsFile(i, m_activeProject, enterTimes[i],  "unknown");

        }
    }

    closeAnalyticsFile();


}

void MainWindow::closeAnalyticsFile()
{
  // terminate root tag
  QTextStream out(&m_kbpfFile);
  out << "\n</" << m_tagRoot << ">\n";
  out.flush();

  m_kbpfFile.flush();
  m_kbpfFile.close();

}
void MainWindow::emitErrorMessage()
{
    emit footTrafficTrackerProblem("Problem with the Foot Traffic Tracker detected!");
}



MainWindow::~MainWindow()
{
}
