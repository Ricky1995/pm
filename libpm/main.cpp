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

#include "utils.h"
#include "getMemorySize.h"
#include "WindowMain.h"
#include "SystemConfig.h"
#include "CpuFeatures.h"
#include "PmData.h"
#include "PmLog.h"
#include "RunGuard.h"

#include <QApplication>
#include <QString>
#include <QDir>
#include <QLayout>
#include <QSplashScreen>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QProcess>
#include <QStringList>
#include <QStyleFactory>

#include <string>
#include <algorithm>
#include <iostream>

#include <CLI/CLI.hpp>

using namespace std;

// local function declarations
void handleMessages(QtMsgType type, const QMessageLogContext& context, const QString& msg);
void initTranslation(QApplication& parApp);
bool checkSystemInstallation(QString& parVboxDefaultMachineFolder);


int main(int argc, char *argv[])
{


  // libsodium needs to be initialized before first use, because this can be security relevant, we make it at the beginning
  if (sodium_init() == -1)
  {
    cerr << "error initialize libsodium" << endl;
    return -1;
  }

  // Hide the console on windows if no args specified
  #ifdef PM_WINDOWS
    if (argc == 1)
      FreeConsole();
  #endif

  // parse command line input using CLI
  CLI::App cliApp{"PrivacyMachine, a browser virtualisation programm wich aims to protect the users privacy."};

  // enable config files (INI)
  cliApp.set_config("-c, --config");

  // create a CLI-INI-file
  bool createCliIniFile;
  auto cliOptionCreateCliIniFile = cliApp.add_flag("-C, --create-config", createCliIniFile, "Prints a default configuration file in INI format and exits.");

  bool logSensitiveData;
  cliApp.add_flag("-s, --log-sensitive", logSensitiveData, "Enable logging of sensible data, i.e. passwords");

  std::string pmConfigDirPath = getPmDefaultConfigQDir();
  auto cliOptionPmConfigDir = cliApp.add_option("-w, --working-dir", pmConfigDirPath, "Change the config/working directory. A independend instance will be build there. (default " + pmConfigDirPath +")");

  std::string vmMaskPrefix = "VmMask_";
  auto cliOptionVmMaskPrefix = cliApp.add_option("--VmMask-prefix", vmMaskPrefix, "DO NOT CHANGE! The VmMask prefix for the INI file. (default " + vmMaskPrefix +")");

  std::string pmVmMaskPrefix = "pm_"+vmMaskPrefix;
  auto cliOptionPmVmMaskPrefix = cliApp.add_option("--PmVmMask-prefix", pmVmMaskPrefix, "The PM-VmMask prefix. (default " + pmVmMaskPrefix +")");

  std::string vpnPrefix = "VPN_";
  cliApp.add_option("--VPN-prefix", vpnPrefix, "DO NOT CHANGE! The VPN prefix for the INI file. (default " + vpnPrefix +")");

  std::string pmUserConfigFilePath = pmConfigDirPath+"/PrivacyMachine.ini";
  auto cliOptionPmUserConfigFilePath = cliApp.add_option("--pm-user-config", pmUserConfigFilePath, "NOT SUPPORTED! The path to the PM user config file. (default " + pmUserConfigFilePath +")");

  std::string pmInternalConfigFilePath = pmConfigDirPath+"/PrivacyMachineInternals.ini";
  auto cliOptionPmInternalConfigFilePath = cliApp.add_option("--pm-internal-config", pmInternalConfigFilePath, "NOT SUPPORTED! The path to the PM internal config file. (default " + pmInternalConfigFilePath +")");

  std::string pmServerIp = "127.0.0.1";
  cliApp.add_option("--pm-server-ip", pmServerIp, "NOT SUPPORTED! The IP of a remote PM-server. (default " + pmServerIp +")");

  std::string vmSnapshotName = "UpAndRunning";
  cliApp.add_option("--VM-snapshot-name", vmSnapshotName, "Name of the clean VM-Mask snapshots. (default " + vmSnapshotName +")");

  std::string baseDiskRootUser = "root";
  cliApp.add_option("--BaseDisk-root-user", baseDiskRootUser, "Name of the base disk 'root' user. (default " + baseDiskRootUser +")");

  std::string baseDiskRootUserPassword = "123";
  cliApp.add_option("--BaseDisk-root-user-password", baseDiskRootUserPassword, "Password of the base disk 'root' user. (default " + baseDiskRootUserPassword +")");

  std::string baseDiskLiveUser = "liveuser";
  cliApp.add_option("--BaseDisk-live-user", baseDiskLiveUser, "Name of the base disk 'live' user. (default " + baseDiskLiveUser +")");

  std::string baseDiskLiveUserPassword = "123";
  cliApp.add_option("--BaseDisk-live-user-password", baseDiskLiveUserPassword, "Password of the base disk 'live' user. (default " + baseDiskLiveUserPassword +")");


  // parse the command line options
  CLI11_PARSE(cliApp, argc, argv);

  // Print default configuration file
  if (createCliIniFile)
  {
      cliApp.remove_option(cliOptionCreateCliIniFile);
      std::cout << cliApp.config_to_str(true);
      exit(0);
  }

  // Set PmData values
  auto& pmData = PmData::getInstance();
  pmData.setPmConfigDirPath( QString::fromStdString( pmConfigDirPath ) );

  // check that we not accedentially use the same PM-VmMask prefix
  if( cliOptionPmConfigDir->count() > 0 && cliOptionPmVmMaskPrefix->count() == 0 )
    pmVmMaskPrefix += calculateConfigDirId(pmConfigDirPath) + "_";

  // check that we not accedentially use the wrong UserConfigFile
  if( cliOptionPmConfigDir->count() > 0 && cliOptionPmUserConfigFilePath->count() == 0 )
    pmUserConfigFilePath = pmConfigDirPath + "/PrivacyMachine.ini";

  // check that we not accedentially use the wrong InternalConfigFile
  if( cliOptionPmConfigDir->count() > 0 && cliOptionPmInternalConfigFilePath->count() == 0 )
    pmInternalConfigFilePath = pmConfigDirPath + "/PrivacyMachineInternals.ini";


  pmData.setPmServerIp( QString::fromStdString( pmServerIp ) );
  pmData.setBaseDiskLiveUser( QString::fromStdString( baseDiskLiveUser ) );
  pmData.setBaseDiskLiveUserPassword( QString::fromStdString( baseDiskLiveUserPassword ) );
  pmData.setBaseDiskRootUser( QString::fromStdString( baseDiskRootUser ) );
  pmData.setBaseDiskRootUserPassword( QString::fromStdString( baseDiskRootUserPassword ) );
  pmData.setPmInternalConfigFilePath( QString::fromStdString( pmInternalConfigFilePath ) );
  pmData.setPmUserConfigFilePath( QString::fromStdString( pmUserConfigFilePath ) );
  pmData.setPmVmMaskPrefix( QString::fromStdString( pmVmMaskPrefix ) );
  pmData.setVmMaskPrefix( QString::fromStdString( vmMaskPrefix ) );
  pmData.setVpnPrefix( QString::fromStdString( vpnPrefix ) );
  pmData.setVmSnapshotName( QString::fromStdString( vmSnapshotName ) );


  int retCode = -1;
  bool guiNeverShown = true;
  QString vboxDefaultMachineFolder;

  QApplication app(argc, argv);
  app.setOrganizationName("PrivacyMachine");
  app.setApplicationName(PMRELEASENAME);
  app.setApplicationVersion(PMVERSION);
  app.setOrganizationDomain("https://www.privacymachine.eu");

  // Get the path to the current running executable (only works before we change the current path inside this application)
  QString pmInstallDirPath = app.applicationDirPath();
  // remember the path
  pmData.setInstallDirPath(pmInstallDirPath);

  // PmData should be compleatly filled now!
  if( !pmData.completelyFilled() )
  {
    IERR("FATAL: Logic error at initializing PmData!");
    exit(1);
  }
  // Be save to run only one instance
  RunGuard guard( "PrivacyMachine is running   Config ID = " + QString::fromStdString(calculateConfigDirId(pmConfigDirPath)) );
  if ( !guard.tryToRun() )
  {
    qDebug() <<  "An other instance of the PrivacyMachine is already running";
    QMessageBox abortMsg(QMessageBox::Information, QApplication::applicationName(),
                         "The PrivacyMachine is already running!");
    abortMsg.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    return abortMsg.exec();
  }

  QDir userConfigDir = PmData::getInstance().getPmConfigDir();
  if (!userConfigDir.exists())
  {
    if (!userConfigDir.mkpath("."))
    {
      qCritical() << "Error creating user config dir: " << userConfigDir;
      return false;
    }
  }
  if (!PmLog::getInstance().initAndRotateLogfiles(userConfigDir.path()))
    return false; // error message already logged to the console
  // If requested by the user, set the global variable to enable logging of sensitive data via ILOG_SENSITIVE
  if (logSensitiveData)
    PmLog::getInstance().enableSensitiveLogging();

  // Enable Warning messages triggered by qDebug(), qWarning(), qCritical(), qFatal()
  qInstallMessageHandler(handleMessages);


  QString startTime = currentTimeStampAsISO();
  ILOG("Starting up at " + startTime + " Version:" + PMVERSION);

  // TODO: bernhard: remove this encoding test under windows
  ILOG("remove this encoding test under windows: 'böße'");
  qDebug() << "remove this native encoding test under windows: 'böße'" << endl;

#ifdef WIN32
    // need to initialize the windows sockets
    WSADATA wsaData;
    int err;
    err = WSAStartup(0x101, &wsaData);
    if (err != 0)
    {
        IERR("WSAStartup failed with error: " + QString::number(err));
        return -1;
    }
#endif

    // if  logSensitiveData log PmData
    pmData.log();

  try // catching exceptions
  {  
    // init the translation of qt internal messages
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // init the translation of PrivacyMachine messages
    initTranslation(app);

    // init name
    QCoreApplication::setApplicationName(QCoreApplication::translate("mainfunc", PMRELEASENAME));
    QCoreApplication::setApplicationVersion(PMVERSION);

    // Check if all is installed what will be needed to run the PrivacyMachine
    if (!checkSystemInstallation(vboxDefaultMachineFolder))
    {
      guiNeverShown = false;
      return -1; // error messages to User are already presented
    }

    WindowMain mainWindow;
    if (mainWindow.init(pmInstallDirPath, vboxDefaultMachineFolder))
    {
      mainWindow.show();
      guiNeverShown = false;
      retCode = app.exec();
    }
    else
    {
      retCode = -1;
    }
  }
  catch (std::exception& e)
  {
    IERR("exception occoured: " + QString::fromStdString(e.what()));
    retCode = -1;
  }

  if (guiNeverShown && retCode != 0)
  {
    QString message = QCoreApplication::translate("mainfunc", "Some errors occoured: Please check the logfile for Details.");
    QMessageBox::warning(Q_NULLPTR, "PrivacyMachine", message);
  }

  QString stopTime = currentTimeStampAsISO();
  ILOG("shutting down at " + stopTime);
  return retCode;
}

// this function will be used by qDebug(), qWarning(), qCritical(), qFatal()
void handleMessages(QtMsgType parType, const QMessageLogContext& parContext, const QString& parMsg)
{
  PmLog::getInstance().logMessages(parType, parContext, parMsg);
}


bool checkSystemInstallation(QString& parVboxDefaultMachineFolder)
{
  QString buttonContinueText = QCoreApplication::translate("'Continue-anyway'-Button of a shown warning", "Continue anyway");
  size_t memorySize = getMemorySize( );
  qint64 freeMemoryInMb = memorySize / (1024 * 1024);
  // currently not needed:
  //ILOG("Total Physical RAM size: " + QString::number(freeMemoryInMb) + " MB");
  //qint64 freeSpace = getFreeDiskSpace(parInstallPath);
  //ILOG("Free space on disk containing '" + parInstallPath + "': " + QString::number(freeSpace) + " Bytes");

  int availableRamNeededInMB;
  if (RunningOnWindows())
    availableRamNeededInMB = 1024 * 3;
  else
    availableRamNeededInMB = 1024 * 2;

  if (freeMemoryInMb < availableRamNeededInMB)
  {
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(QApplication::applicationName());
    QString message = QCoreApplication::translate("check available resources", "This computer has to low memory(%1 MegaByte) for the PrivacyMachine to work properly (highly recommended: %2 MegaByte).");
    message = message.arg(QString::number(freeMemoryInMb), QString::number(availableRamNeededInMB));
    msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
    msgBox.button(QMessageBox::Ignore)->setText(buttonContinueText);
    msgBox.setText(message);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ignore)
      return false;
    else
      ILOG("User pressed Button 'Continue Anyway' while memory check");
  }

  bool hasVirtualization = CpuFeatures::Virtualization();
  if (!hasVirtualization)
  {
    IERR("Hardware Virtualisation Support is not available");
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(QApplication::applicationName());
    msgBox.setTextFormat(Qt::RichText);   // this is what makes the links clickable
    QString message = QCoreApplication::translate("check available resources", "The Hardware Virtualization Support for the CPU is not enabled (or does not exist on very old computers). Please reboot to enter your BIOS and enable an option called like Virtualization, VT-x, AMD-V or AMD-SVM");
    message += "<br>";
    message += "<div style=\"text-align:center\">";
    message += "<a href='http://www.howtogeek.com/213795/how-to-enable-intel-vt-x-in-your-computers-bios-or-uefi-firmware/'>Explanation on howtogeek.com</a>";
    message += "</div>";
    msgBox.setText(message);
    msgBox.exec();
    return false;
  }

  bool vboxInstalled;
  QString vboxCommand;
  QString vboxVersion;  
  bool vboxExtensionPackInstalled;
  determineVirtualBoxInstallation(vboxInstalled, vboxCommand, vboxVersion, parVboxDefaultMachineFolder, vboxExtensionPackInstalled);

  if (!vboxInstalled)
  {
    IERR("VirtualBox is not installed");
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(QApplication::applicationName());
    msgBox.setTextFormat(Qt::RichText);   // this is what makes the links clickable
    QString message = QCoreApplication::translate("check of software dependencies", "VirtualBox from Oracle is not installed, please download it directly from:");
    message += "<br>";
    message += "<div style=\"text-align:center\">";
    message += "<a href='https://www.virtualbox.org'>https://www.virtualbox.org</a>";
    message += "</div>";
    msgBox.setText(message);
    msgBox.exec();
    return false;
  }

  if (!vboxExtensionPackInstalled)
  {
    IERR("ExtensionPack of VirtualBox is not installed");
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(QApplication::applicationName());
    msgBox.setTextFormat(Qt::RichText);   // this is what makes the links clickable
    QString messagePart1 = QCoreApplication::translate("check of software dependencies", "The 'Extension Pack' for VirtualBox is not installed, please download it directly from:");
    messagePart1 += "<br>";
    messagePart1 += "<div style=\"text-align:center\">";
    messagePart1 += "<a href='https://www.virtualbox.org/wiki/Downloads'>https://www.virtualbox.org/wiki/Downloads</a>";
    messagePart1 += "</div>";
    QString messagePart2 = QCoreApplication::translate("check of software dependencies", "Install the downloaded File 'Oracle_VM_VirtualBox_Extension_Pack-[CurrentVersion].vbox-extpack' with a Double-Click (it will be opened by VirtualBox)");
    msgBox.setText(messagePart1 + "<br><br>" + messagePart2);
    msgBox.exec();
    return false;
  }


  // We only support the VirtualBox-Versions supported by Oracle: One main and one legacy version
  // @Debian-Users: Sorry, but VirtualBox is not maintained any more: <https://www.debian.org/security/2016/dsa-3699>
  //
  // from <https://www.virtualbox.org/wiki/Download_Old_Builds>
  // i.e. Actual Version of VirtualBox-Stable : 5.0.28 (<StableMajor>.<StableMinor>.<StableBugfix>)
  // i.e. Actual Version of VirtualBox-Current: 5.1.8  (<CurrentMajor>.<CurrentMinor>.<CurrentBugfix>)
  // -> 5.0.28: ok
  // -> 5.0.29: ok, log warning
  // -> 5.0.1: show warning: too old
  // -> 5.1.8: ok
  // -> 5.1.9: ok, log warning
  // -> 5.1.1: show warning: too old
  // -> 5.2.1: show warning: unsupported
  int  StableMajor = 5; int  StableMinor = 0; int  StableBugfix = 38;
  int CurrentMajor = 5; int CurrentMinor = 1; int CurrentBugfix = 20;

  // Supported Versions to show User i.e. "5.0.* + 5.1.*"
  QString supportedVersions;
  supportedVersions += QString::number(StableMajor);
  supportedVersions += ".";
  supportedVersions += QString::number(StableMinor);
  supportedVersions += ".* ";
  supportedVersions += QString::number(CurrentMajor);
  supportedVersions += ".";
  supportedVersions += QString::number(CurrentMinor);


  // the current version contains the subversion revision i.e.: 5.0.28r111378
  ILOG("installed VirtualBox version: " + vboxVersion);
  QRegularExpression regExpVboxVersion("^(\\d+)\\.(\\d+)\\.(\\d+).*$", QRegularExpression::MultilineOption);
  QRegularExpressionMatch match;

  bool showVersionUnsupported = false;
  bool showVersionToOld = false;
  bool showVersionToNew = false;
  QString vboxVersionStripped = vboxVersion;
  match = regExpVboxVersion.match(vboxVersion);
  if (match.lastCapturedIndex() == 3)
  {
    vboxVersionStripped = match.captured(1) + "." + match.captured(2) + "." + match.captured(3);
    if (match.captured(1).toInt() == StableMajor && match.captured(2).toInt() == StableMinor )
    {
      // VirtualBox-Stable detected
      if (match.captured(3).toInt() < StableBugfix)
      {
        showVersionToOld = true;
      }
      if (match.captured(3).toInt() > StableBugfix)
      {
        IWARN("Currently installed Bugfix-Version of VirtualBox-'Stable' is newer than the Verion tested by the PrivacyMachine-Team");
      }
    }
    else if (match.captured(1).toInt() == CurrentMajor && match.captured(2).toInt() == CurrentMinor )
    {
      // VirtualBox-Current detected
      if (match.captured(3).toInt() < CurrentBugfix)
      {
        showVersionToOld = true;
      }
      if (match.captured(3).toInt() > CurrentBugfix)
      {
        IWARN("Currently installed Bugfix-Version of VirtualBox-'Current' is newer than the Verion tested by the PrivacyMachine-Team");
      }
    }
    else if (match.captured(1).toInt() == CurrentMajor && match.captured(2).toInt() > CurrentMinor )
    {
      // Minor(API?) version is newer
      IWARN("Currently installed Version of VirtualBox-'Current' is newer than the Verion tested by the PrivacyMachine-Team");
      showVersionToNew = true;
    }
    else
    {
      IWARN("Unknown version format of virtualbox");
      showVersionUnsupported = true;
    }
  }
  else
  {
    IWARN("Unknown version format of virtualbox");
    showVersionUnsupported = true;
  }

  if (showVersionUnsupported)
  {
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(QApplication::applicationName());
    QString messageLine1 = QCoreApplication::translate("check of software dependencies", "The currently installed VirtualBox version '%1' is unsupported.");
    messageLine1 = messageLine1.arg(vboxVersionStripped);
    QString messageLine2 = QCoreApplication::translate("check of software dependencies", "Currently supported versions are: %1");
    messageLine2 = messageLine2.arg(supportedVersions);
    QString message = messageLine1 + "<br>" + messageLine2;
    msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
    msgBox.button(QMessageBox::Ignore)->setText(buttonContinueText);
    msgBox.setText(message);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ignore)
      return false;
    else
      IWARN("User pressed Button 'Continue Anyway' while virtualbox version check");
  }

  if (showVersionToNew)
  {
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(QApplication::applicationName());
    QString messageLine1 = QCoreApplication::translate("check of software dependencies", "The currently installed VirtualBox version '%1' is newer than the Verion tested by the PrivacyMachine-Team.");
    messageLine1 = messageLine1.arg(vboxVersionStripped);
    QString messageLine2 = QCoreApplication::translate("check of software dependencies", "Currently tested versions are: %1");
    messageLine2 = messageLine2.arg(supportedVersions);
    QString message = messageLine1 + "<br>" + messageLine2;
    msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
    msgBox.button(QMessageBox::Ignore)->setText(buttonContinueText);
    msgBox.setText(message);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ignore)
      return false;
    else
      IWARN("User pressed Button 'Continue Anyway' while virtualbox version check");
  }

  if (showVersionToOld)
  {
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/privacymachine.svg"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(QApplication::applicationName());
    QString messageLine1 = QCoreApplication::translate("check of software dependencies", "The currently installed VirtualBox version '%1' is too old. Please update VirtualBox to the current version and start the PrivacyMachine again.");
    messageLine1 = messageLine1.arg(vboxVersionStripped);
    QString messageLine2;
    if (RunningOnWindows())
    {
      messageLine2 = "<br><br>" + QCoreApplication::translate("check of software dependencies", "To update VirtualBox: Start it manually from the windows start menu and navigate to the menu inside VirtualBox: File -> Check for Updates");
    }
    QString message = messageLine1 + messageLine2;
    msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
    msgBox.button(QMessageBox::Ignore)->setText(buttonContinueText);
    msgBox.setText(message);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ignore)
      return false;
    else
      IWARN("User pressed Button 'Continue Anyway' while virtualbox version check");
  }

  return true;
}

void initTranslation(QApplication& parApp)
{
  QTranslator pmTranslator;
  // TODO: read from PrivacyMachine.ini
  QString currentLanguage = "lang_" + QLocale::system().name();
  // TODO: remove hardcoded english language (i18n and german tranlation is incomplete now)
  currentLanguage = "lang_de_DE";
  pmTranslator.load(currentLanguage);
  parApp.installTranslator(&pmTranslator);
}
