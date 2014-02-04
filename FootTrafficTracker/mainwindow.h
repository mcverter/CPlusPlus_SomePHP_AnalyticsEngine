#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QKinectWrapper.h>
#include <QFile>
#include <QDateTime>
#include <XnFPSCalculator.h>
#include "/home/dsion/.represents/curiokulbooks.h"
#include "KinectMonitor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void writeXMLtoCSV();

signals:
    void footTrafficTrackerProblem(QString message);


public slots:
    void kinectUserSlot(unsigned,bool);
    void kinectStatusSlot(QKinect::KinectStatus);
    void handleApplicationCloseSlot();



private:
    QKinect::QKinectWrapper kreader;
    XnFPSData xnFPS;


    // this should actually be taken from QKinectWrapper.h
    static const int MAX_BODIES = 20;
    static const int MAX_FILE_SIZE = 999 * 1024 * 1024;
    static const int MAX_POSSIBLE_DURATION = 60 * 60 * 2;

    QVector <QString> enterTimes;

    // variables for analytics file
    QString m_analyticsDirPath;
    QString m_analyticsFileBaseName;


    QFile m_kbpfFile;
    QXmlStreamWriter m_stream;
    bool FileDATA_OK;
    QString m_tagRoot;
    QString m_docType;



    // To Set the CurioKulBooks 
    QString m_activeProject;
    CurioKulBooks * kulbooks;


    KinectMonitor * km;


    

    // methods for handling the analytics file
    void initializeAnalyticsFile();
    void createNewAnalyticsFile(QString baseDir);
    void closeAnalyticsFile();
    void writeUserToAnalyticsFile(int userID,
				  QString projectName,
				  QString entryTime,
                                  QString exitTime="unknown");
    void emitErrorMessage();
};

#endif // MAINWINDOW_H
