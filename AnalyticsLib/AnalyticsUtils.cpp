#include "AnalyticsUtils.h"

const QString AnalyticsUtils::TAGSETFILE = "AnalysisTypes.xml";

AnalyticsUtils::AnalyticsUtils(QObject *parent) :
    QObject(parent)
{
    initializeTagSets();
}

bool AnalyticsUtils::initializeTagSets()
{
    QFile analyticsMasterFile;
    analyticsMasterFile.setFileName(TAGSETFILE);
    QXmlStreamReader tagReader;
    tagReader.setDevice(&analyticsMasterFile);

    if(!analyticsMasterFile.open(QIODevice::ReadOnly))
    {
        emit FileConversionError("ERROR:  Could not initialize XML Tags.");
        return FALSE;
    }
    else
    {
        QXmlStreamReader tagReader;
        tagReader.setDevice(&analyticsMasterFile);

        while (!tagReader.atEnd() &&
               !tagReader.hasError())
        {
            QXmlStreamReader::TokenType token =  tagReader.readNext();
            if(token == QXmlStreamReader::StartElement)
            {
                if(tagReader.name() == "TagSet")
                {
                    QXmlStreamAttributes attributes = tagReader.attributes();
                    m_allTagSets[attributes.value("Filename").toString()] =
                            (AnalysisTagSet) {

                        attributes.value("DocType").toString(),
                                attributes.value("DocRootTag").toString(),
                                attributes.value("ElementTag").toString(),
                                attributes.value("ProjectAttribute").toString(),
                                attributes.value("IDAttribute").toString(),
                                attributes.value("StartAttribute").toString(),
                                attributes.value("EndAttribute").toString(),
                                attributes.value("TimeFormat").toString(),
                                attributes.value("Filename").toString(),
                                attributes.value("StatName").toString(),

                    };

                }
            }
        }
        if (tagReader.hasError())
        {
            emit FileConversionError("ERROR: Could not parse Master Analysis XML file: " +  tagReader.errorString());
            return FALSE;
        }
    }
    return TRUE;
}



AnalyticsUtils::AnalysisTagSet AnalyticsUtils::getTagSet(QString filetype)
{
    return m_allTagSets[filetype];
}


bool AnalyticsUtils::doesTagSetExist(QString filetype)
{
    return m_allTagSets.contains(filetype);
}

bool AnalyticsUtils::convertCsvToKbpf(QString filename)
{
    QFile csvFile;
    QFile kbpfFile;
    QTextStream csvReader;
    QXmlStreamWriter  kbpfWriter;


    csvFile.setFileName(filename);
    if(!csvFile.open(QIODevice::ReadOnly))
      {
       emit FileConversionError("Failed opening CSV File for Reading.");
       return FALSE;
      }
    csvReader.setDevice(&csvFile);


    QString kbpfFilename = filename;
    kbpfFilename.truncate(kbpfFilename.length()-3);
    kbpfFilename.append("kbpf");
    kbpfFile.setFileName(kbpfFilename);
    if(!kbpfFile.open(QIODevice::WriteOnly))
    {
       emit FileConversionError("Failed opening KBPF File for Writing.");
       return FALSE;
    }
     kbpfWriter.setDevice(&kbpfFile);
     kbpfWriter.setAutoFormatting(true);



     QString basename = (QFileInfo(filename)).baseName();
     QString filetype = basename.left(basename.indexOf("_"));

    if  (! doesTagSetExist(filetype))
    {
        emit FileConversionError("Could not find complete schema for Filetype " + filetype);
        return FALSE;
    }

     AnalyticsUtils::AnalysisTagSet tagSet = getTagSet(filetype);
     if (tagSet.docType==NULL || tagSet.rootTag==NULL ||
             tagSet.elementTag==NULL || tagSet.userIDTag==NULL ||
             tagSet.projectTag==NULL || tagSet.startTag==NULL || tagSet.endTag==NULL)
     {
         emit FileConversionError("Could not find complete schema for Filetype " + filetype);
         return FALSE;
     }


     csvReader.readLine();  // we don't use the first line


    QString csvLine;
    QStringList csvEntries;
    QString elementName;

    kbpfWriter.writeStartDocument();
     kbpfWriter.writeDTD("<!DOCTYPE " + tagSet.docType + ">");
     kbpfWriter.writeStartElement(tagSet.rootTag);
     elementName = tagSet.elementTag;

     while (!csvReader.atEnd())
     {
         csvLine = csvReader.readLine();
         csvEntries = csvLine.split(",");
         if (csvEntries.size()<4)
         {
             emit FileConversionError("Error parsing CSV line.");
             return FALSE;
         }
         kbpfWriter.writeStartElement(tagSet.elementTag);
         kbpfWriter.writeAttribute(tagSet.userIDTag, csvEntries.at(0));
         kbpfWriter.writeAttribute(tagSet.projectTag, csvEntries.at(1));
         kbpfWriter.writeAttribute(tagSet.startTag, csvEntries.at(2));
         kbpfWriter.writeAttribute(tagSet.endTag, csvEntries.at(3));
         kbpfWriter.writeEndElement();
     }
     kbpfWriter.writeEndDocument();

    kbpfFile.flush();
    kbpfFile.close();
    return TRUE;

}

bool AnalyticsUtils::convertKbpfToCsv(QString filename)
{
    QFile kbpfFile;
    QFile csvFile;
    QTextStream csvWriter;
    QXmlStreamReader  xmlReader;


    kbpfFile.setFileName(filename);
    if(!kbpfFile.open(QIODevice::ReadOnly))
      {
       emit FileConversionError("Failed opening Log File for Reading.");
        return FALSE;
      }
    xmlReader.setDevice(&kbpfFile);


    QString csvFilename = filename;
    csvFilename.truncate(csvFilename.length()-4);
    csvFilename.append("csv");
    csvFile.setFileName(csvFilename);
     if(!csvFile.open(QIODevice::ReadWrite))
     {
       emit FileConversionError("Failed opening CSV File for Writing.");
       return FALSE;
     }
     csvWriter.setDevice(&csvFile);

     QXmlStreamReader::TokenType token;

     QString basename = (QFileInfo(filename)).baseName();
     QString filetype = basename.left(basename.indexOf("_"));

     if  (! m_allTagSets.contains(filetype))
     {
         emit FileConversionError("Could not find complete schema for Filetype " + filetype);
         return FALSE;
     }

     AnalyticsUtils::AnalysisTagSet tagSet = getTagSet(filetype);


     if (tagSet.elementTag==NULL || tagSet.userIDTag==NULL ||
             tagSet.projectTag==NULL || tagSet.startTag==NULL || tagSet.endTag==NULL)
     {
         emit FileConversionError("Could not find complete schema for Filetype " + filetype);
         return FALSE;
     }



     QString userTag = tagSet.userIDTag;
     QString projectTag = tagSet.projectTag;
     QString startTag = tagSet.startTag;
     QString endTag = tagSet.endTag;
     csvWriter << userTag << "," << projectTag << "," << startTag << "," << endTag << "\n";


     QString userID, enter, exit, project;

    while (!xmlReader.atEnd() &&
           !xmlReader.hasError())  // we might want to take this out bc
                                    // xml files will not have closing tag
   {
     token =  xmlReader.readNext();
     if(token == QXmlStreamReader::StartElement)
     {
        QXmlStreamAttributes attributes = xmlReader.attributes();
        userID = attributes.value(userTag).toString();
        project = attributes.value(projectTag).toString();
        enter = attributes.value(startTag).toString();
        exit = attributes.value(endTag).toString();
        if (userID != NULL && project != NULL && enter!=NULL && exit != NULL &&
           userID.size()>0 && project.size()>0 && enter.size() >0 && exit.size()>0)
           {
               csvWriter << userID << "," << project << "," << enter << "," << exit << "\n";
           }
     }
    }
    if (xmlReader.hasError())
      {
        if (xmlReader.error() != xmlReader.PrematureEndOfDocumentError)
        {
          QString errorString =  "ERROR: Could not parse XML file: " +  xmlReader.errorString();

          emit FileConversionError(errorString);
          csvWriter.flush();
          csvFile.flush();
          csvFile.close();
          csvFile.remove();
          return FALSE;
        }
        else
        {
            emit FileConversionWarning("WARNING:  Did not find XML end tag.");

        }
      }


    csvWriter.flush();
    csvFile.flush();
    csvFile.close();
    return TRUE;

}


// we are not actually doing directory-wide conversion but I would like to maintain this code in case we go back to it.
void AnalyticsUtils::directoryConversion()
{/*
 QStringList xml_file_paths= dirHandle.entryList(
               QStringList(log_basename+"*"),
               QDir::Files,QDir::Name);

    for (int i=0; i<xml_file_paths.size(); i++)
      {
        QFile xml_input_file;
        QXmlStreamReader xml_read_stream;
        QString xml_path = log_dir_path + "/" + xml_file_paths[i];
        xml_input_file.setFileName(xml_path);
        xml_read_stream.setDevice(&xml_input_file);

        if(!xml_input_file.open(QIODevice::ReadOnly))
          {
            qDebug("Failed opening Log File for Reading.");
            return;
          }

        QString userID, enter, exit, project;
        while (!xml_read_stream.atEnd() &&
               !xml_read_stream.hasError())
          {
            QXmlStreamReader::TokenType token =  xml_read_stream.readNext();
            if(token == QXmlStreamReader::StartElement)
              {
                if(xml_read_stream.name() == elementTag)
                  {
                    QXmlStreamAttributes attributes = xml_read_stream.attributes();
                    userID = attributes.value(userAttributeName).toString();
                    project = attributes.value(projectAttributeName).toString();
                    enter = attributes.value(enterAttributeName).toString();
                    exit = attributes.value(exitAttributeName).toString();
                    csv_write_stream << userID << "," << project << "," << enter << "," << exit << "\n";
                  }
              }
          }
        if (xml_read_stream.hasError())
          {
            qDebug() << "Could not parse XML. " <<  xml_read_stream.errorString();

          }
      }

*/
}
