#include "mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qDebug() << "hello world";
    kreader.start();
    xnFPSInit(&xnFPS, 180);
    connect(&kreader,SIGNAL(dataNotification()),this,SLOT(kinectData()));
    connect(&kreader,SIGNAL(statusNotification(QKinect::KinectStatus)),this,SLOT(kinectStatus(QKinect::KinectStatus)));
    connect(&kreader,SIGNAL(userNotification(unsigned,bool)),this,SLOT(kinectUser(unsigned,bool)));
    connect(&kreader,SIGNAL(poseNotification(unsigned,QString)),this,SLOT(kinectPose(unsigned,QString)));
    connect(&kreader,SIGNAL(calibrationNotification(unsigned,QKinect::CalibrationStatus)),this,SLOT(kinectCalibration(unsigned,QKinect::CalibrationStatus)));
}

void MainWindow::kinectStatus(QKinect::KinectStatus s)
{

    QString str("Kinect: ");
    if(s==QKinect::Idle)
            str += "Idle";
    if(s==QKinect::Initializing)
            str += "Initializing";
    if(s==QKinect::OkRun)
            str += "Running";
    if(s==QKinect::ErrorStop)
            str += "Error";
    qDebug() << "Status is " << str;
}

void MainWindow::kinectData()
{
/*
    sbKinectFrame->setText(QString("Frame %1").arg(kreader.getFrameID()));
        sbKinectTime->setText(QString("Timestamp %1").arg(kreader.getTimestamp()));


        QImage img = kreader.getDepth();
        ui->label->setPixmap(QPixmap::fromImage(img));
        img = kreader.getCamera();
        ui->labelImage->setPixmap(QPixmap::fromImage(img));
        */
        QKinect::Bodies bodies = kreader.getBodies();

/*
        sbKinectNumBody->setText(QString("Body: %1").arg(bodies.size()));
        xnFPSMarkFrame(&xnFPS);
        sbKinectFPS->setText(QString("FPS: %1").arg(xnFPSCalc(&xnFPS)));

        printf("Available bodies: ");
        for(unsigned i=0;i<bodies.size();i++)
                if(bodies[i].status == QKinect::Tracking)
                        printf("%d ",i);
        printf("\n");

*/
}


void MainWindow::kinectPose(unsigned id,QString pose)
{
        printf("User %u: Pose '%s' detected\n",id,pose.toStdString().c_str());
}

void MainWindow::kinectUser(unsigned id, bool found)
{
    if (found == TRUE)
    {
        qDebug() << "New User Found.  ID " << id;
    }
    else
    {
        qDebug() << "User Lost.  ID " << id;

    }
    printf("User %u: %s\n",id,found?"found":"lost");
}

void MainWindow::kinectCalibration(unsigned id, QKinect::CalibrationStatus s)
{
        printf("User %u: Calibration ",id);
        if(s==QKinect::CalibrationStart)
                printf("started\n");
        if(s==QKinect::CalibrationEndSuccess)
                printf("successful\n");
        if(s==QKinect::CalibrationEndFail)
                printf("failed\n");


}

/*
        XnStatus nRetVal = XN_STATUS_OK;
        xn::EnumerationErrors errors;

        const char *fn = NULL;
        if    (fileExists(SAMPLE_XML_PATH)) fn = SAMPLE_XML_PATH;
        else if (fileExists(SAMPLE_XML_PATH_LOCAL)) fn = SAMPLE_XML_PATH_LOCAL;
        else {
            printf("Could not find '%s' nor '%s'. Aborting.\n" , SAMPLE_XML_PATH, SAMPLE_XML_PATH_LOCAL);
            return XN_STATUS_ERROR;
        }
        printf("Reading config from: '%s'\n", fn);

        nRetVal = g_Context.InitFromXmlFile(fn, g_scriptNode, &errors);
        if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
        {
            XnChar strError[1024];
            errors.ToString(strError, 1024);
            printf("%s\n", strError);
            return (nRetVal);
        }
        else if (nRetVal != XN_STATUS_OK)
        {
            printf("Open failed: %s\n", xnGetStatusString(nRetVal));
            return (nRetVal);
        }

        nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
        CHECK_RC(nRetVal,"No depth");

        nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
        if (nRetVal != XN_STATUS_OK)
        {
            nRetVal = g_UserGenerator.Create(g_Context);
            CHECK_RC(nRetVal, "Find user generator");
        }

        XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        {
            printf("Supplied user generator doesn't support skeleton\n");
            return 1;
        }
        nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
        CHECK_RC(nRetVal, "Register to user callbacks");
        nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
        CHECK_RC(nRetVal, "Register to calibration start");
        nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
        CHECK_RC(nRetVal, "Register to calibration complete");

        if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
        {
            g_bNeedPose = TRUE;
            if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
            {
                printf("Pose required, but not supported\n");
                return 1;
            }
            nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
            CHECK_RC(nRetVal, "Register to Pose Detected");
            g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
        }

        g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

        nRetVal = g_Context.StartGeneratingAll();
        CHECK_RC(nRetVal, "StartGenerating");

        XnUserID aUsers[MAX_NUM_USERS];
        XnUInt16 nUsers;
        XnSkeletonJointTransformation torsoJoint;

        printf("Starting to run\n");
        if(g_bNeedPose)
        {
            printf("Assume calibration pose\n");
        }
        XnUInt32 epochTime = 0;
        while (!xnOSWasKeyboardHit())
        {
            g_Context.WaitOneUpdateAll(g_UserGenerator);
            // print the torso information for the first user already tracking
            nUsers=MAX_NUM_USERS;
            g_UserGenerator.GetUsers(aUsers, nUsers);
            int numTracked=0;
            int userToPrint=-1;
            for(XnUInt16 i=0; i<nUsers; i++)
            {
                if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])==FALSE)
                    continue;

                g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_TORSO,torsoJoint);
                    printf("user %d: head at (%6.2f,%6.2f,%6.2f)\n",aUsers[i],
                                                                    torsoJoint.position.position.X,
                                                                    torsoJoint.position.position.Y,
                                                                    torsoJoint.position.position.Z);
            }

        }
        g_scriptNode.Release();
        g_DepthGenerator.Release();
        g_UserGenerator.Release();
        g_Context.Release();


*/
