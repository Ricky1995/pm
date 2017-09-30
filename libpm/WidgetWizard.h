#ifndef WidgetWizard_H
#define WidgetWizard_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QListWidget>

namespace Ui {
  class WidgetWizard;
}

class WidgetWizard : public QWidget
{
    Q_OBJECT

  public:
    explicit WidgetWizard(QWidget *parent = nullptr);
    ~WidgetWizard();
    void addWidget(QWidget *w);

  private:
    Ui::WidgetWizard *ui_;

    QComboBox *selectedMask;
    QGroupBox *properties;
    QLineEdit *name;
    QLineEdit *displayname;
    QButtonGroup *network;
    QListWidget *dns;
    QLineEdit *dnsNew;
    QListWidget *provider;
    QLineEdit *providerNew;
    QButtonGroup *browser;
    QButtonGroup *language;
    QButtonGroup *webRTC;
    QButtonGroup *java;
    QButtonGroup *flash;
    QButtonGroup *cookies;
    QPushButton *colour;
    QPlainTextEdit *description;

  private slots:
    void handleNewMask();
    void handleChangeColour();
    void handleSave();
    void handleAddDns();
    void handleDeleteDns();
    void handleAddProvider();
    void handleDeleteProvider();

};

#endif // WidgetWizard_H
