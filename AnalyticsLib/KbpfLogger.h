/**************************************
 This function is used to create and update the statistical logs which will be used by the Analytics engine.


Each accumulation of statistics occurs through the creation and updating of log files.
Log files are characterized by the following
(1)  They exist in a unique directory
(2)  They have a unique basename
(3)  The basename is appended with the TimeOfCreation

=====

Each Log File is an XML File.   Thereore it must contain:
(1)  Doctype
(2)  Document Root

  A log file consists of XML entries of the form:

  <Element id="idValue" project="projectValue start="startValue" end="endValue" />

    A log file consists of a list of these Elements.

  Each Element will contain the following 4 attributes:
  (1)  User ID
  (2)  Project Name
  (3)  Start Time
  (4)  End Time

  ==================

The variables and methods in this class are intended to facilitate the
(1) Creation of the Log Directory
(2) Creation of Log Files
(3) Writing/Appending to Log Files

Additional Functionality
(1)  Convert XML files to CSV file

=====================

FUNCTIONS:


Before an event can be logged, the logger must be initialized.


    void initializeLogger(
        QString directory,  // Directory where log files are stored
        QString filebase,     // Basename for log files
        QString docType,       // Doctype of log file
        QString rootTag,      // Root tag of log file
        QString elementTag,   // Name of Element
        QString projectValue,  // Value of Project Attribute
                               // I am assuming that, within a log file,
                               // The project Value will always be the same
        QString projectAttributeName,  // Name of Project Attribute
        QString userIDAttributeName,     // Name of UserID Attribute
        QString timeStartAttributeName, // Name of Start Attribute
        QString timeEndAttributeName     // Name of End Attribute
        );



In addition to initializing variables, this function also creates
the necessary files and directories.

As noted, I am assuming that the Project Name will be the same for
every value in the log file.
 ========

 This function is used to log each event.

void logEvent(int userID,
              QString entryTime,
              QString exitTime);

 As stated above, I am assuming that the Project Name is the same for every
 value in the log file.

===========

 This static function is used to convert all of the log files in a given
 directory into a single CSV file

 static void convertToCSV(
        QString log_dir_path,    // Directory where log files are stored
        QString log_basename,     // Basename for log files
        QString elementTag,       // Name of Element
        QString projectAttributeName,  // Name of Project Attribute
        QString userIDAttributeName,     // Name of UserID Attribute
        QString timeStartAttributeName, // Name of Start Attribute
        QString timeEndAttributeName     // Name of End Attribute
        );

  ===========
   PRIVATE FUNCTIONS


   void convertToCSV();  // member function, not static
                         // member variables already initialized
   void createNewLogFile();  // creates new log file
                             // prepends the necessary XML
   void closeLogFile();      // appends the closing XML tags and closes file

   void emitErrorMessage(); // nothing is being done with this.


  *************************************/


#ifndef KBPFLOGGER_H
#define KBPFLOGGER_H

#include <QObject>
#include <QFile>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QDir>
#include <QXmlStreamWriter>
#include <QDateTime>
#include <QDebug>
#include "AnalyticsUtils.h"


class KbpfLogger : public QObject
{
    Q_OBJECT


public:
    explicit KbpfLogger(QObject *parent = 0);
    void initializeLogger(
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
        );

    void logEvent(int userID,
                  QString entryTime,
                  QString exitTime);

    ~KbpfLogger();


signals:
        void loggingProblem(QString message);

private:
   void createNewLogFile();
   void closeLogFile();
   void emitErrorMessage();


   // file location
    QString m_logDirPath;
    QString m_logFileBaseName;

    // file output
    QFile m_kbpfFile;
    QXmlStreamWriter m_streamWriter;

    // XML headers
    QString m_docType;
    QString m_tagRoot;

    // Element Name
    QString m_tagElement;

    // Attribute Names
    QString m_attributeNameTimeEnd;
    QString m_attributeNameTimeStart;
    QString m_attributeNameProject;
    QString m_attributeNameUserID;

    // Attribute Values
    QString m_valueTimeEnd;
    QString m_valueTimeStart;
    QString m_valueProject;
    QString m_valueUserID;

    // Maximum size is 999MB.
    // On file close, a few extra bytes may be tagged on
     static const int MAX_FILE_SIZE = 999 * 1024 * 1024;
};

#endif // KBPFLOGGER_H
