﻿/*==============================================================================
        Copyright (c) 2013-2017 by the Developers of PrivacyMachine.eu
                         contact@privacymachine.eu
     OpenPGP-Fingerprint: 0C93 F15A 0ECA D404 413B 5B34 C6DE E513 0119 B175

                     Licensed under the EUPL, Version 1.1
     European Commission - subsequent versions of the EUPL (the "Licence");
        You may not use this work except in compliance with the Licence.
                  You may obtain a copy of the Licence at:
                        http://ec.europa.eu/idabc/eupl

 Unless required by applicable law or agreed to in writing, software distributed
              under the Licence is distributed on an "AS IS" basis,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
      See the Licence for the specific language governing permissions and
                        limitations under the Licence.
==============================================================================*/

#include "XmlUpdateParser.h"
#include "utils.h"

XmlUpdateParser::XmlUpdateParser()
{
}

bool XmlUpdateParser::parse(QByteArray parRawData)
{
  QDomDocument dom;
  QString errorMsg;
  bool ok;
  int errorLine;
  int errorColumn;
  if (!dom.setContent(parRawData, &errorMsg, &errorLine, &errorColumn))
  {
    IWARN("XML-parsing error: line:" + QString::number(errorLine) + ", column:" + QString::number(errorColumn));
    return false;
  }

  QDomElement root = dom.documentElement();
  if (root.tagName() != "rss")
  {
    IWARN("Unexpected XML-Document");
    return false;
  }

  // get <channel>
  QDomNode nodeChannel = root.firstChild();
  if (nodeChannel.nodeName() != "channel")
  {
    IWARN("Unexpected XML-Document");
    return false;
  }

  // loop over all <item> (which is as Blog-Entry)
  QDomNode childOfChannel = nodeChannel.firstChild();
  while (!childOfChannel.isNull())
  {
    if (childOfChannel.nodeName() == "item")
    {
      QString title;
      QDateTime date;
      QString description;
      QString updateType;

      QDomNode nodeTitle = childOfChannel.firstChildElement("title");
      if (!nodeTitle.isNull())
        title = nodeTitle.toElement().text();

      QDomNode nodeDate = childOfChannel.firstChildElement("pubDate");
      if (!nodeDate.isNull())
      {
        QString secondsSince1970;
        if (nodeDate.attributes().contains("secondsSince1970"))
          secondsSince1970 = nodeDate.attributes().namedItem("secondsSince1970").nodeValue();

        ok = true;
        qint64 seconds = secondsSince1970.toLongLong(&ok);
        if (ok)
          date.setTime_t(seconds);
      }
      QDomNode nodeDesc = childOfChannel.firstChildElement("description");
      if (!nodeDesc.isNull())
        description = nodeDesc.toElement().text();

      QDomNode nodeType = childOfChannel.firstChildElement("PmUpdateType");
      if (!nodeType.isNull())
        updateType = nodeType.toElement().text();

      QString versionStr;
      QDomNode nodeVersion = childOfChannel.firstChildElement("PmVersion");
      if (!nodeVersion.isNull())
        versionStr = nodeVersion.toElement().text();

      if (updateType == "binary")
      {
        UpdateInfoBinary newBinary;
        newBinary.Title = title;
        newBinary.Date = date;
        newBinary.Description = description;
        newBinary.Date = date;
        newBinary.Version.parse(versionStr); // ignore errors?

        QDomNode nodeCheckSums = childOfChannel.firstChildElement("PmSHA3-256CheckSums");
        if (!nodeCheckSums.isNull())
        {
          QDomNode nodeCheckSum = nodeCheckSums.firstChild();
          while(!nodeCheckSum.isNull() && nodeCheckSum.nodeName() == "CheckSum")
          {
            QString os;
            if (nodeCheckSum.attributes().contains("os"))
              os = nodeCheckSum.attributes().namedItem("os").nodeValue();

            QString url;
            if (nodeCheckSum.attributes().contains("url"))
              url = nodeCheckSum.attributes().namedItem("url").nodeValue();

            QString checkSum = nodeCheckSum.toElement().text();

            // append the CheckSum
            CheckSumListBinary newCheckSum;
            newCheckSum.Os = os;
            newCheckSum.Url = url;
            newCheckSum.CheckSum = checkSum;
            newBinary.CheckSums.append(newCheckSum);

            // find next <CheckSum>
            nodeCheckSum = nodeCheckSum.nextSibling();
          }
        }

        // store the data
        binaries_.append(newBinary);
      }

      if(updateType == "BaseDisk")
      {
        UpdateInfoBaseDisk newBaseDisk;
        newBaseDisk.Title = title;
        newBaseDisk.Date = date;
        newBaseDisk.Description = description;
        newBaseDisk.Date = date;
        newBaseDisk.Version.parse(versionStr); // ignore errors?

        QDomNode nodeCheckSums = childOfChannel.firstChildElement("PmSHA3-256CheckSums");
        if (!nodeCheckSums.isNull())
        {
          QDomNode nodeCheckSum = nodeCheckSums.firstChild();
          while(!nodeCheckSum.isNull() && nodeCheckSum.nodeName() == "CheckSum")
          {
            QString url;
            if (nodeCheckSum.attributes().contains("url"))
              url = nodeCheckSum.attributes().namedItem("url").nodeValue();

            QString upFrom;
            if (nodeCheckSum.attributes().contains("from"))
              upFrom = nodeCheckSum.attributes().namedItem("from").nodeValue();

            QString checkSum = nodeCheckSum.toElement().text();

            // append the CheckSum
            CheckSumListBaseDisk newCheckSum;
            newCheckSum.ComponentMajorUp = upFrom.toInt();
            newCheckSum.Url = url;
            newCheckSum.CheckSum = checkSum;
            newBaseDisk.CheckSums.append(newCheckSum);

            // find next <CheckSum>
            nodeCheckSum = nodeCheckSum.nextSibling();
          }
        }

        // store the data
        baseDisks_.append(newBaseDisk);
      }

    }
    // find next
    childOfChannel = childOfChannel.nextSibling();
  }

  return true;
}

XmlUpdateParser::UpdateInfoBinary* XmlUpdateParser::getLatestBinaryVersion()
{
  if(binaries_.count() == 0)
    return NULL;

  PmVersion latestVersion;
  UpdateInfoBinary* latestItem = 0;
  for (int i=0; i < binaries_.count(); i++)
  {
    if (binaries_[i].Version > latestVersion)
    {
      latestVersion = binaries_[i].Version;
      latestItem = &binaries_[i];
    }
  }
  return latestItem;
}

XmlUpdateParser::UpdateInfoBaseDisk* XmlUpdateParser::getLatestBaseDiskVersion()
{
  if(baseDisks_.count() == 0)
    return NULL;

  PmVersion latestVersion;
  UpdateInfoBaseDisk* latestItem = 0;
  for (int i=0; i < baseDisks_.count(); i++)
  {
    if (baseDisks_[i].Version > latestVersion)
    {
      latestVersion = baseDisks_[i].Version;
      latestItem = &baseDisks_[i];
    }
  }
  return latestItem;
}
