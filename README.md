## PrivacyMachine source

**Homepage: [https://www.privacymachine.eu](https://www.privacymachine.eu/en)**

The PrivacyMachine is a browser which protects the privacy of the users.

  * Technical Concept in [english](concept_and_notes/TechnicalConcept_en.pdf)
  * Technical Concept in [german](concept_and_notes/TechnicalConcept_de.pdf)

**Current Status:**

  We have a Beta-Release which can be used to get an impression how it should work.
  An important feature, the automated update, is currently not ready, so use it with care!  
  For Windows we have an [installer package](https://www.privacymachine.eu/de/download/)  
  For Linux (debian based distros) you have to compile the software yourself.  
  
Recently we moved our internal git repository to the public github repository to make it easier to get in touch with new developers.

The branch **'master'** contains the last released version (the beta1 now).  
On the branch **'develop'** we continue the development.  

Available documentation:
  * The UserGuide in [german](https://www.privacymachine.eu/de/user-manual/) describes hardware and software requirements.
  * Here we describe an overview of the [working principle](https://www.privacymachine.eu/en/how-webtracking-works/).
  * The VPN Guide is currently only available on the german homepage: https://www.privacymachine.eu/de/vpn
  * The folder [pm/concept_and_notes](pm/concept_and_notes) contains many design documents  
  * The BaseDisk-Build-HowTo is located at [packaging/BaseDisk/README.md](packaging/BaseDisk/README.md)  
  * In the source code we prefer to write the documentation in the header files.  



### If you want to join us you are very welcome!
We have also a big list of ideas and todo's where we really could need some help!  
Even if we are a little paranoid, we are nice people and always have some fun together:  
https://www.privacymachine.eu/de/about/


#### Code reviews and design suggestions are also very welcome!

For a Bug-Bounty it's too early - we would get poor ;)  

Please write us at contact@privacymachine.eu  
PGP-Fingerprint: 0C93 F15A 0ECA D404 413B 5B34 C6DE E513 0119 B175


### HowTo build from Source
 
Checkout the source (we work in the "develop" branch):  
```
git clone -b develop https://github.com/privacymachine/src.git
```  

Please check the build-requirements for linux or windows first (sections below)
  
This project uses the github repositories
 * https://github.com/pm-bernhard/FreeRDP (a snapshot of [FreeRDP](http://www.freerdp.com) with minor fixes)
 * https://github.com/pm-bernhard/RemoteDisplay (a qt-widget which uses FreeRDP)
 
These repsoitories are checked out automatically during the build process.

Even if you want to develop with Qt-Creator, please build the project one time from the console because problems are easier to target there.  
Under Windows a [cygwin](https://www.cygwin.com/) installation is needed to execute the build scripts.

#### Build the external libraries
start the script "./buildExtLibs.sh" to build in the directory "<git-root>build_ext_libs".  
The libraries will be created in the directory "<git-root>/working_dir/lib".  

#### Build the PrivacyMachine
start the script "./buildPm.sh" to build in the directory "<git-root>build_pm".  
The binaries will be created in the directory "<git-root>/working_dir".  

Usually it's not needed to build the BaseDisk yourself, the PrivacyMachine will download the actual version from our server.

The PrivacyMachine can be directly started within the directory "working_dir".  
Please note that on first start the 4GB-BaseDisk will be stored in the user config folder:
  ~/.config/privacyachine under linux  
  %UserProfile%\PrivacyMachine under Windows

#### Developing using QtCreator

There are some potential pitfalls you have to know:

  * The CMake integration in QtCreator is not perfect, so if you change something in the CMakeLists.txt, first close QtCreator, test if the changes work from commandline and then open QtCreator again.
  * When Opening the inital CMakeLists.txt file please specify as build folder the directory "<git-root>/build_pm" (the "<git-root>/working_dir", in which the binaries are build, is choosen automatically)
  * For starting/debugging the PrivacyMachine you have to manually configure the "Working directory" to "<git-root>/working_dir".

##### Tips&Tricks
  * after running 'doxygen' in the root direcory a qt-help file will be generated: "<git-root>/doc_generated/eu.privacymachine.dev-doc.qch"
  * you can specify this in QtCreator in the menu: Tools->Options:Help->Documentation:"Add..." QtCreater has to be reopened to take use of it.
  * Enable the plugin "Todo" in Help->About plugins.

### Build-requirements Linux (tested on ubuntu-16.04 and debian-jessie)

* check that you have these packages installed:
  
  ```
  apt install build-essential git-core cmake zlib1g-dev qt5-default qtmultimedia5-dev qttools5-dev-tools libssl-dev libsodium-dev sshpass p7zip-full libpopt-dev libbz2-dev doxygen graphviz xsltproc
  ```

* Ubuntu 18.04 needs one additional package:
  
  ```
  apt install qttools5-dev
  ```
  

### Build-requirements Windows (tested on Win7)

* Install cygwin

* Install VisualStudio Community Edition 2015 English
  * Deselect all
  * Select: Programming Language->VisualC++->Common Tools

* If running as jenkins-slave: Install Java JDK

* Install Qt via the Online Installer (OpenSource Edition)

  * https://www.qt.io/download-open-source/
  * Install to C:\\Qt59
  * Deselect Qt 5.8
  * Deselect Qt 5.7
  * Deselect Qt 5.6
  * Select Qt 5.9->msvc2015 64bit
  * Deselect Qt 5.9->Qt Purchasing
  * Deselect Qt 5.9->Qt WebEngine
  * Deselect Qt 5.9->Qt SCXML
  * Deselect Qt 5.9->Qt SerialBus
  * Deselect Qt 5.9->Qt Gamepad
  * Deselect Qt 5.9->Qt Script

* Install "Qt Creator Beta1"
  
  * https://www.qt.io/download-open-source/
  * Install to C:\\Qt59\\qtcreator-4.3.0-beta1

* Add to environment variable PATH of whole System C:\\Qt59\\5.9\\msvc2015_64\\bin

* Install cmake (we use currently 3.8.0-rc4)

  * https://cmake.org/download/
  * Install via 64Bit Installer
    * Select option: "Add CMake to system PATH for all users

* Install OpenSSL

  * Use the Installer "Win64 OpenSSL v1.0.*" from https://slproweb.com/products/Win32OpenSSL.html to install Win64 OpenSSL v1.0.2j to C:\\OpenSSL-Win64 directory
  * Use Option: "Copy OpenSSL-DLL's to /bin directory"

* Install Wix Toolset: Wix 3.10.3

* Install JOM

  * Download http://download.qt.io/official_releases/jom/jom.zip
  * extract to i.e. C:\\Qt\\jom
  * set environment variable PATH to the folder above

* Install Popt for Windows from http://downloads.sourceforge.net/project/gnuwin32/popt/1.8-1/popt-1.8-1-lib.exe

  * install to C:\\Program Files (x86)\\GnuWin32

* Download libsodium: (libsodium-1.0.11-msvc.zip) from https://download.libsodium.org/libsodium/releases/

 * extract the content to c:\\libsodium
