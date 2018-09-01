#include "WidgetWizard.h"
#include "ui_WidgetWizard.h"
#include "PmManager.h"
#include "WindowMain.h"
#include "VmMaskData.h"
#include "VmMaskUserConfig.h"

#include <QLabel>
#include <QFont>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QListWidget>
#include <QRadioButton>
#include <QPlainTextEdit>
#include <QButtonGroup>
#include <QColorDialog>
#include <QString>

WidgetWizard::WidgetWizard(QWidget *parent, PmManager *pmManager) :
    QWidget(parent),
    ui_(new Ui::WidgetWizard)
{
    ui_->setupUi(this);
    this->pmManager = pmManager;

    // title
    QLabel *title = new QLabel("Wizard");
    QFont font = title->font();
    font.setPointSize(36);
    font.setBold(true);
    title->setFont(font);
    this->addWidget(title);

    // masks
    QHBoxLayout *layout = new QHBoxLayout();
    ui_->verticalLayout->addLayout(layout);
    layout->addWidget(new QLabel("Mask:"));

    selectedMask = new QComboBox();
    for (int vmMaskId = 0; vmMaskId < pmManager->getVmMaskData().count(); vmMaskId++)
    {
        VmMaskUserConfig* vmMask = pmManager->getVmMaskData()[vmMaskId]->UserConfig;
        selectedMask->addItem(vmMask->getName());
    }
    connect(selectedMask, SIGNAL (currentIndexChanged(int)), this, SLOT (handleSelectionChanged(int)));
    layout->addWidget(selectedMask);

    QPushButton *button = new QPushButton("New", this);
    connect(button, SIGNAL (released()), this, SLOT (handleNewMask()));
    layout->addWidget(button);
    button = new QPushButton("Change", this);
    layout->addWidget(button);
    button = new QPushButton("Delete", this);
    layout->addWidget(button);

    // properties
    properties = new QGroupBox();
    QGridLayout *propLayout = new QGridLayout();
    properties->setLayout(propLayout);
    this->addWidget(properties);
    int row = 0;

    // name
    propLayout->addWidget((new QLabel("Name:")), row, 0);
    name = new QLineEdit();
    propLayout->addWidget(name, row, 1);
    row++;

    // displayname
    propLayout->addWidget((new QLabel("Displayname:")), row, 0);
    displayname = new QLineEdit();
    propLayout->addWidget(displayname, row, 1);
    row++;

    // network
    propLayout->addWidget((new QLabel("Network Connection:")), row, 0);
    network = new QButtonGroup();
    network->setExclusive(false);
    QGroupBox *box = new QGroupBox();
    QVBoxLayout *vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    QAbstractButton *aButton = new QCheckBox("LocalIP");
    vLayout->addWidget(aButton);
    network->addButton(aButton);
    aButton = new QCheckBox("VPN - CryptoFree");
    vLayout->addWidget(aButton);
    network->addButton(aButton);
    aButton = new QCheckBox("VPN - Gate");
    vLayout->addWidget(aButton);
    network->addButton(aButton);
    aButton = new QCheckBox("TOR");
    vLayout->addWidget(aButton);
    network->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;

    // dns
    propLayout->addWidget((new QLabel("DNS-Server:")), row, 0);
    box = new QGroupBox();
    layout = new QHBoxLayout();
    box->setLayout(layout);
    vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();
    dns = new QListWidget(this);
    hLayout->addWidget(dns);
    button = new QPushButton("Delete", this);
    connect(button, SIGNAL (released()), this, SLOT (handleDeleteDns()));
    hLayout->addWidget(button);
    vLayout->addLayout(hLayout);
    hLayout = new QHBoxLayout();
    dnsNew = new QLineEdit();
    hLayout->addWidget(dnsNew);
    button = new QPushButton("Add", this);
    connect(button, SIGNAL (released()), this, SLOT (handleAddDns()));
    hLayout->addWidget(button);
    vLayout->addLayout(hLayout);
    layout->addLayout(vLayout);
    propLayout->addWidget(box, row, 1);
    row++;

    // provider
    propLayout->addWidget((new QLabel("Provider:")), row, 0);
    box = new QGroupBox();
    layout = new QHBoxLayout();
    box->setLayout(layout);
    vLayout = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    provider = new QListWidget(this);
    hLayout->addWidget(provider);
    button = new QPushButton("Delete", this);
    connect(button, SIGNAL (released()), this, SLOT (handleDeleteProvider()));
    hLayout->addWidget(button);
    vLayout->addLayout(hLayout);
    hLayout = new QHBoxLayout();
    providerNew = new QLineEdit();
    hLayout->addWidget(providerNew);
    button = new QPushButton("Add", this);
    connect(button, SIGNAL (released()), this, SLOT (handleAddProvider()));
    hLayout->addWidget(button);
    vLayout->addLayout(hLayout);
    layout->addLayout(vLayout);
    propLayout->addWidget(box, row, 1);
    row++;

    // browser
    propLayout->addWidget((new QLabel("Browser:")), row, 0);
    browser = new QButtonGroup();
    browser->setExclusive(false);
    box = new QGroupBox();
    vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    aButton = new QCheckBox("Firefox");
    vLayout->addWidget(aButton);
    browser->addButton(aButton);
    aButton = new QCheckBox("Firefox-ESR");
    vLayout->addWidget(aButton);
    browser->addButton(aButton);
    aButton = new QCheckBox("Torbrowser");
    vLayout->addWidget(aButton);
    browser->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;

    // language
    propLayout->addWidget((new QLabel("Languages:")), row, 0);
    language = new QButtonGroup();
    language->setExclusive(false);
    box = new QGroupBox();
    vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    aButton = new QCheckBox("English");
    vLayout->addWidget(aButton);
    language->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;

    // webrtc
    /*propLayout->addWidget((new QLabel("WebRTC:")), row, 0);
    webRTC = new QButtonGroup();
    box = new QGroupBox();
    vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    aButton = new QRadioButton("Yes");
    vLayout->addWidget(aButton);
    webRTC->addButton(aButton);
    aButton = new QRadioButton("No");
    vLayout->addWidget(aButton);
    webRTC->addButton(aButton);
    aButton = new QRadioButton("Don't care");
    vLayout->addWidget(aButton);
    webRTC->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;*/

    // javascript
    propLayout->addWidget((new QLabel("Javascript:")), row, 0);
    java = new QButtonGroup();
    box = new QGroupBox();
    vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    aButton = new QRadioButton("Yes");
    vLayout->addWidget(aButton);
    java->addButton(aButton);
    aButton = new QRadioButton("No");
    vLayout->addWidget(aButton);
    java->addButton(aButton);
    aButton = new QRadioButton("Don't care");
    vLayout->addWidget(aButton);
    java->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;

    // flashplayer
    propLayout->addWidget((new QLabel("Flash-Player:")), row, 0);
    flash = new QButtonGroup();
    box = new QGroupBox();
    vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    aButton = new QRadioButton("Yes");
    vLayout->addWidget(aButton);
    flash->addButton(aButton);
    aButton = new QRadioButton("No");
    vLayout->addWidget(aButton);
    flash->addButton(aButton);
    aButton = new QRadioButton("Don't care");
    vLayout->addWidget(aButton);
    flash->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;

    // cookies
    propLayout->addWidget((new QLabel("3rd-Party-Cookies:")), row, 0);
    cookies = new QButtonGroup();
    box = new QGroupBox();
    vLayout = new QVBoxLayout();
    box->setLayout(vLayout);
    aButton = new QRadioButton("Yes");
    vLayout->addWidget(aButton);
    cookies->addButton(aButton);
    aButton = new QRadioButton("No");
    vLayout->addWidget(aButton);
    cookies->addButton(aButton);
    aButton = new QRadioButton("Don't care");
    vLayout->addWidget(aButton);
    cookies->addButton(aButton);
    propLayout->addWidget(box, row, 1);
    row++;

    // colour
    propLayout->addWidget((new QLabel("Colour:")), row, 0);
    colour = new QPushButton("Change Colour", this);
    connect(colour, SIGNAL (released()), this, SLOT (handleChangeColour()));
    colour->setAutoFillBackground(true);
    propLayout->addWidget(colour, row, 1);
    row++;

    // description
    propLayout->addWidget((new QLabel("Description:")), row, 0);
    description = new QPlainTextEdit();
    propLayout->addWidget(description, row, 1);
    row++;

    // savebutton
    layout = new QHBoxLayout();
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    button = new QPushButton("Save", this);
    connect(button, SIGNAL (released()), this, SLOT (handleSave()));
    layout->addWidget(button);
    propLayout->addLayout(layout, row, 1);

    ui_->verticalLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    properties->setVisible(false);
    if(selectedMask->count() > 0) {
        selectedMask->currentIndexChanged(0); // fill in the first mask automatically
    }
}

WidgetWizard::~WidgetWizard()
{
    delete ui_;
}

void WidgetWizard::addWidget(QWidget *w)
{
    ui_->verticalLayout->addWidget(w);
}

void WidgetWizard::handleNewMask()
{
    properties->setVisible(true);

    QList<QAbstractButton*> buttons = network->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        buttons[i]->setChecked(true);
    }
    buttons = browser->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        buttons[i]->setChecked(true);
    }
    buttons = language->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        buttons[i]->setChecked(true);
    }
    buttons = webRTC->buttons();
    buttons[2]->setChecked(true);
    buttons = java->buttons();
    buttons[2]->setChecked(true);
    buttons = flash->buttons();
    buttons[2]->setChecked(true);
    buttons = cookies->buttons();
    buttons[2]->setChecked(true);
    QPalette *pal = new QPalette();
    const QColor *color = new QColor(Qt::white);
    pal->setColor(QPalette::Button, *color);
    colour->setPalette(*pal);
}

void WidgetWizard::handleChangeColour()
{
    QColorDialog *colorD = new QColorDialog();
    QColor c = colorD->getColor(colour->palette().button().color());
    QPalette *pal = new QPalette();
    pal->setColor(QPalette::Button, c);
    colour->setPalette(*pal);
}

void WidgetWizard::handleSelectionChanged(int index)
{
    properties->setVisible(true);
    VmMaskUserConfig *vmMask = pmManager->getVmMaskData()[index]->UserConfig;
    name->setText(vmMask->getFullName());
    displayname->setText(vmMask->getName());

    QString networkConnection = vmMask->getNetworkConnectionType();
    QList<QAbstractButton*> buttons = network->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        buttons[i]->setChecked(false);
    }
    if(networkConnection.toStdString() == "LocalIp") buttons[0]->setChecked(true);
    if(networkConnection.toStdString() == "VPN_CryptoFree") buttons[1]->setChecked(true);
    if(networkConnection.toStdString() == "VPN_VPNGate") buttons[2]->setChecked(true);
    if(networkConnection.toStdString() == "TOR") buttons[3]->setChecked(true);

    dns->addItems(vmMask->getDnsServers());
    provider->addItems(vmMask->getIpAddressProviders());

    QStringList browsers = vmMask->getBrowsers();
    buttons = browser->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        buttons[i]->setChecked(false);
    }
    for(int i = 0; i < browsers.length(); i++) {
        if(browsers[i].toStdString() == "firefox") buttons[0]->setChecked(true);
        if(browsers[i].toStdString() == "firefox-esr") buttons[1]->setChecked(true);
        if(browsers[i].toStdString() == "torbrowser") buttons[2]->setChecked(true);
    }

    QStringList languages = vmMask->getBrowserLanguages();
    buttons = language->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        buttons[i]->setChecked(false);
    }
    if(languages.length()>1) name->setText(languages[1]);
    for(int i = 0; i < languages.length(); i++) {
        if(languages[i].toStdString() == "en-US, en") buttons[0]->setChecked(true);
    }

    if(vmMask->getJava()) java->buttons()[0]->setChecked(true);
    else java->buttons()[1]->setChecked(true);

    if(vmMask->getFlash()) flash->buttons()[0]->setChecked(true);
    else flash->buttons()[1]->setChecked(true);

    if(vmMask->getThirdPartyCookies().toStdString() == "on") cookies->buttons()[0]->setChecked(true);
    else cookies->buttons()[1]->setChecked(true);

    QPalette *pal = new QPalette();
    const QColor color = vmMask->getColor();
    pal->setColor(QPalette::Button, color);
    colour->setPalette(*pal);

    description->setPlainText(vmMask->getDescription());
}

void WidgetWizard::handleSave()
{
}

void WidgetWizard::handleAddDns()
{
    new QListWidgetItem(dnsNew->text(), dns);
}

void WidgetWizard::handleDeleteDns()
{
    dns->takeItem(dns->currentRow());
}

void WidgetWizard::handleAddProvider()
{
    new QListWidgetItem(providerNew->text(), provider);
}

void WidgetWizard::handleDeleteProvider()
{
    provider->takeItem(provider->currentRow());
}
