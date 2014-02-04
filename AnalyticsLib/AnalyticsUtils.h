/*********************
File:  AnalyticsUtils.h
Class:  AnalyticsUtils


This class is being used as a container for various utilities that can be used alongside the main analytics task as well as any functions that might need to be shared among the various main classes.

Quite possibly, we will need to break up this class into functional components.

For now, this class is used to encompass the following functionaliies:

(1) File Conversions
..(a) Kbpf to Csv
..(b) Csv to Kbpf

(2) Managing system
This  class will also be used as a container for any file conversion,
such as converting the kpbf files to csv files or normalizing kbpf files that are in non-standard formats.


This class can also be used for any constants that need to be utilized by a variety of classes.

***********************************/
#ifndef ANALYTICSUTILS_H
#define ANALYTICSUTILS_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDebug>


class AnalyticsUtils : public QObject
{
    Q_OBJECT
public:
    explicit AnalyticsUtils(QObject *parent = 0);

    struct AnalysisTagSet {
        QString docType;
        QString rootTag;
        QString elementTag;
        QString projectTag;
        QString userIDTag;
        QString startTag;
        QString endTag;
        QString timeFormat;
        QString fileprefix;
        QString statname;
    };


    static const QString TAGSETFILE;
    QHash<QString, AnalysisTagSet> m_allTagSets;
    AnalysisTagSet getTagSet(QString);
    bool doesTagSetExist(QString filetype);
    bool initializeTagSets();


    bool convertCsvToKbpf(QString filename);
    bool convertKbpfToCsv(QString filename);


    void directoryConversion();

signals:
    void FileConversionError(QString);
    void FileConversionWarning(QString);

};

#endif // ANALYTICSUTILS_H
