#include "WidgetWizard.h"
#include "ui_WidgetWizard.h"

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

WidgetWizard::WidgetWizard(QWidget *parent) :
  QWidget(parent),
  ui_(new Ui::WidgetWizard)
{
  ui_->setupUi(this);

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
  propLayout->addWidget((new QLabel("DNS-Server:")), row, 0);
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
  aButton = new QCheckBox("German");
  vLayout->addWidget(aButton);
  language->addButton(aButton);
  propLayout->addWidget(box, row, 1);
  row++;

  // webrtc
  propLayout->addWidget((new QLabel("WebRTC:")), row, 0);
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
  row++;

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
        (buttons.operator [](i))->setChecked(true);
    }
    buttons = browser->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        (buttons.operator [](i))->setChecked(true);
    }
    buttons = language->buttons();
    for(int i = 0; i < buttons.length(); i++)
    {
        (buttons.operator [](i))->setChecked(true);
    }
    buttons = webRTC->buttons();
    (buttons.operator [](2))->setChecked(true);
    buttons = java->buttons();
    (buttons.operator [](2))->setChecked(true);
    buttons = flash->buttons();
    (buttons.operator [](2))->setChecked(true);
    buttons = cookies->buttons();
    (buttons.operator [](2))->setChecked(true);
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

void WidgetWizard::handleSave()
{
    properties->setVisible(false);
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
