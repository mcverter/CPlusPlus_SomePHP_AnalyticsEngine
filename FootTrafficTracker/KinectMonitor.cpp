/**********************

 Class: KinectMonitor

 The Kinect Monitor implements a thread that checks periodically whether the Kinect is connected to the USB
 (1)  If the Kinect is running and the Kinect is disconnected, the Kinect is stopped
 (2)  If the Kinect is not running and the Kinect is connected, the Kinect is started


 A few implementation notes:
 (1)  When the Kinect is reconnected it takes a while to restart -- maybe 15 seconds or so.
 (2)  When the Kinect is disconnected, this Thread seems to be blocked for a while.  I assume this is because it hangs on the call to QtKinectMonitor::isRunning()

  (3)  The location of the KinectConnectionLog is currently hardcoded in the constructor.

  *********************/


#include "KinectMonitor.h"
#include <QDebug>


/***************
 Constructor
 Initialized with a pointer to a QKinectWrapper.
 Note that the location of the Connection Log is hardcoded.
****************/
KinectMonitor::KinectMonitor(QKinect::QKinectWrapper * kinect)
{
    kreader = kinect;
    logFileName="/home/dsion/KinectConnectionLog";

    logFile.setFileName(logFileName);
    logReader.setDevice(&logFile);
    if(!logFile.open(QIODevice::ReadOnly))
     {
       emit KinectLogError("Could not open up Kinect Log File:  " +  logFileName);
     }

    running=isRunning();
    connected=isConnected();
}

// Asks the QConnectWrapper whether Kinect is running
bool KinectMonitor::isRunning()
{
    return kreader->isRunning();
}

/**************
  Determines whether the Kinect is connected by reading the Log File
  It does so by finding the last character "d" in the file and
  then checking whether the 2nd previous character is
  "t" ("Connected") or "v" ("Removed")

  This function is written C standard library style yet it uses Qt objects instead.  It shoud be easy to change to C if necessary
  *************/
bool KinectMonitor::isConnected()
{
    int eof = logFile.size();
    QString lastChar;
    do {
        eof--;
        logReader.seek(eof);
        lastChar = logReader.read(1);
    } while (lastChar != "d");

    logReader.seek(eof-2);
    lastChar = logReader.read(1);
    if (lastChar=="v")
    {
        return FALSE;
    }
    else if (lastChar=="t")
    {
        return TRUE;
    }
    else
    {
        emit KinectLogError("Error Reading Last Character of logfile");
    }
    return FALSE;
}

/****
 every second poll the connection and running status
(1)  If the Kinect is running and the Kinect is disconnected, the Kinect is stopped
(2)  If the Kinect is not running and the Kinect is connected, the Kinect is started
*****/
void KinectMonitor::run()
{
    while (TRUE)
    {
        sleep(1);
        connected = isConnected();
        running=isRunning();

        if ( ! connected && running)
        {
                qDebug()<<"Stopping Kinect ...";
                kreader->stop();
        }
        else if (connected && !running)
            {
                qDebug() << "Starting Kinect ...";
                kreader->start();
            }

    }
}
