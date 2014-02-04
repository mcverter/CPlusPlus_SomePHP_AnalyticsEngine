#include "KbpfLogger.h"

KbpfLogger::KbpfLogger(QObject *parent) :
    QObject(parent)
{
}

void KbpfLogger::initializeLogger(
                                      QString directory,
                                      QString filebase,
                                      QString docType,
                                      QString rootTag,
                                      QString elementTag,
                                      QString projectValue,
                                      QString projectAttributeName,
                                      QString userIDAttributeName,
                                      QString timeStartAttributeName,
                                      QString timeEndAttributeName
                                      )
{
    // kbpf file locations
    m_logDirPath = directory;
    m_logFileBaseName = filebase;


    // xml headers
    m_docType = docType;
    m_tagRoot = rootTag;

    // xml entity
    m_tagElement = elementTag;

    // xml attributes
    m_attributeNameProject = projectAttributeName;
    m_attributeNameUserID = userIDAttributeName;
    m_attributeNameTimeStart = timeStartAttributeName;
    m_attributeNameTimeEnd = timeEndAttributeName;


    m_valueProject = projectValue;

    // create log directory
    QDir log_dir(m_logDirPath);
    if (! log_dir.exists())
    {
        log_dir.mkdir(m_logDirPath);
     }


    // change this cd thing

     log_dir.cd(m_logDirPath);
    QStringList xml_file_paths= log_dir.entryList(
                QStringList(m_logFileBaseName+"*"),
                QDir::Files,QDir::Name|QDir::Reversed);
    log_dir.cdUp();

    if (xml_file_paths.size()<1)
    {
        createNewLogFile();
    }

     else
      {
        QString file_path = xml_file_paths.at(0);
        m_kbpfFile.setFileName(m_logDirPath + "/" + file_path);
        if (m_kbpfFile.size()>=MAX_FILE_SIZE)
          {
            createNewLogFile();
          }
     else
       {
           // append to existing Log File
         if(!m_kbpfFile.open(QIODevice::ReadWrite))
           qDebug("Failed opening Log File for Writing.");

         // rewind file pointer to strip off closing tag
         QString  endTag = "</" + m_tagRoot + ">\n";
         m_kbpfFile.seek(m_kbpfFile.size()-endTag.size());

         m_streamWriter.setAutoFormatting(true);
         m_streamWriter.setDevice(&m_kbpfFile);
       }
   }
}



void KbpfLogger::createNewLogFile()
{
  QString fileName = m_logFileBaseName + "_"
          + QString::number(QDateTime::currentDateTime().toTime_t())
          + ".kbpf";
  m_kbpfFile.setFileName(m_logDirPath + "/" + fileName);
  if(!m_kbpfFile.open(QIODevice::ReadWrite))
    qDebug() << "Failed opening Log File for Writing:  " << m_kbpfFile.fileName();
  m_streamWriter.setAutoFormatting(true);
  m_streamWriter.setDevice(&m_kbpfFile);
  m_streamWriter.writeStartDocument();
  m_streamWriter.writeDTD("<!DOCTYPE " + m_docType + ">");
  m_streamWriter.writeStartElement(m_tagRoot);
  m_kbpfFile.flush();
}


void KbpfLogger::logEvent(int userID,
                          QString entryTime,
                          QString exitTime)
{
  m_streamWriter.writeStartElement(m_tagElement);

  m_streamWriter.writeAttribute(m_attributeNameUserID, QString::number(userID));
  m_streamWriter.writeAttribute(m_attributeNameTimeStart, entryTime);
  m_streamWriter.writeAttribute(m_attributeNameTimeEnd, exitTime);

  m_streamWriter.writeAttribute(m_attributeNameProject, m_valueProject);


  m_streamWriter.writeEndElement();

  m_kbpfFile.flush();

  if (m_kbpfFile.size()>=MAX_FILE_SIZE)
    {
      closeLogFile();
      createNewLogFile();
    }

}

void KbpfLogger::closeLogFile()
{
  // terminate root tag before closing
  QTextStream out(&m_kbpfFile);
  out << "\n</" << m_tagRoot << ">\n";
  out.flush();

  m_kbpfFile.flush();
  m_kbpfFile.close();

}

void KbpfLogger::emitErrorMessage()
{
    emit loggingProblem("Problem with the Kbpf Logger detected!");
}



KbpfLogger::~KbpfLogger()
{
}
