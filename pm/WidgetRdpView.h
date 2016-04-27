/*==============================================================================
        Copyright (c) 2013-2016 by the Developers of PrivacyMachine.eu
                         contact@privacymachine.eu
     OpenPGP-Fingerprint: 5134 F10C 58ED FCA2 7F25 7483 50F4 40FC 4347 D242

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
#pragma once

#include <QProcess>
#include <QGridLayout>
#include <QTimer>
#include <QLabel>

#include "PMInstance.h"

#ifndef SKIP_FREERDP_CODE
  #include <remotedisplaywidget.h>
#endif


class WidgetRdpView : public QWidget
{
    Q_OBJECT

  public:
    // override protected:
    void resizeEvent (QResizeEvent * event);

    WidgetRdpView( QString parHost, PMInstance* pmInstance );
    virtual ~WidgetRdpView();

    PMInstance* GetPmInstance(){ return pmInstance_; }
    
    int screenHeight_;
    int screenWidth_;

  signals:
    void signalResize( QWidget *widget );

  private:
    void resizeVmDesktopAndConnectViaRdp();
    void disconnectRdpConnection();

  private:
    QString host_;
    unsigned short rdpPort_;
    unsigned short sshPort_;
    bool connectionEstablished_;
    bool onDisconnecting_;
    int wholeWidgetWidth_;
    int wholeWidgetHeight_;
    
    // The minimum amount to subtract - will be padded to suit FreeRDP
    unsigned short subtractDisplayWidthMin_;
    unsigned short subtractDisplayHeightMin_;
    
    // The actual amount to subtract, which is the minimum amount + padding so that it suits FreeRDP
    unsigned short subtractDisplayWidthCurrent_;
    unsigned short subtractDisplayHeightCurrent_;
    #ifndef SKIP_FREERDP_CODE
      RemoteDisplayWidget* freeRDPwidget_;
    #else
      QLabel* freeRDPwidget_;
    #endif
    QLabel* lblWidth_;
    QLabel* lblHeight_;
    QGridLayout* loGrid_;
    QTimer timerResizeHappend_;
    QString name_;
    QLabel* lblErrorMessage_; // needed in case of error
    bool inErrorMode_; // true on error

    /// This is the machine to which we connect. Every WidgetRdpView has at most one machine it is
    /// connected to.
    PMInstance* pmInstance_;

    // Number of connection retries (we need that to detect endless reconnect-loops
    int retryCount_;

  private slots:

    void slotTimerResizeHappendFired();

    // from RemoteDisplayWidget
    void slotDisconnected();
};