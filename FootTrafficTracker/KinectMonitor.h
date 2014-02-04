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

#ifndef KINECTMONITOR_H
#define KINECTMONITOR_H

#include <QThread>
#include <QObject>
#include <QKinectWrapper.h>
#include <QFile>
#include <QTextStream>

class KinectMonitor : public QThread
{
  Q_OBJECT

    public:
  KinectMonitor(QKinect::QKinectWrapper *);

signals:
  void KinectLogError(QString);

 protected:
  void run();

 private:
  bool isConnected();
  bool isRunning();

  QKinect::QKinectWrapper * kreader;

  QString logFileName;
  QFile logFile;
  QTextStream logReader;

  bool running;
  bool connected;
};

#endif
