#include "dlgmakequake.h"

CDialogMakeQuake::CDialogMakeQuake(const int iTime, const int iCountdown, const QString& strPrn, QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f)
{
	setModal(true);  // make it an application level modal window
	setWindowModality(Qt::ApplicationModal);
	setWindowTitle(tr("Make Your Own Earthquake"));
	//setFixedSize(QSize(500,270)); // spin controls aren't showing up if I let Qt decide on size, at least on Mac
	
	m_bStart = false;
	m_strPrinter = strPrn;
	if (m_strPrinter.isEmpty()) m_strPrinter = PRINTER_PDF; // default to PDF file

	m_iTime = iTime;
	m_iCountdown = iCountdown;

    CreateControls();
}


CDialogMakeQuake::~CDialogMakeQuake()
{
	// get rid of all our objects if they exist
	if (m_labelName) delete m_labelName;
	if (m_labelPrinter) delete m_labelPrinter;
	
    if (m_textctrlName) delete m_textctrlName;
    if (m_spinctrlTime) delete m_spinctrlTime;
    if (m_spinctrlCountdown) delete m_spinctrlCountdown;
	
	if (m_buttonStart) delete m_buttonStart;
	if (m_buttonCancel) delete m_buttonCancel;
	if (m_comboPrinter) delete m_comboPrinter;

	if (m_gridlayout) delete m_gridlayout;

	if (m_layoutMain) delete m_layoutMain;
	if (m_layoutSpin) delete m_layoutSpin;
	if (m_layoutButton) delete m_layoutButton;
	if (m_layoutPrinter) delete m_layoutPrinter;
	
	if (m_groupName) delete m_groupName;
	if (m_groupSpin) delete m_groupSpin;
	if (m_groupButton) delete m_groupButton;
	if (m_groupPrinter) delete m_groupPrinter;
	
	//if (m_dialogPrint) delete m_dialogPrint;
	InitPointers();

}

void CDialogMakeQuake::InitPointers()
{
    m_strName.clear();
	m_qlpi.clear();  // this crashes badly with a heap memory problem!

    // init ptrs to null to test for deletion later
	m_labelName = NULL;
	m_labelPrinter = NULL;
	
    m_textctrlName = NULL; 
    m_spinctrlTime = NULL; 	
    m_spinctrlCountdown = NULL; 	
	
	m_buttonStart = NULL;
	m_buttonCancel = NULL;
	m_comboPrinter = NULL;
	
	m_layoutMain = NULL;
	m_layoutSpin = NULL;
    m_layoutButton = NULL;
    m_layoutPrinter = NULL;

	m_groupName = NULL;
	m_groupSpin = NULL;
	m_groupButton = NULL;
	m_groupPrinter = NULL;

	m_gridlayout = NULL;
	
	//m_dialogPrint = NULL;
}

void CDialogMakeQuake::getUserString(char* strName)
{ 
	memset(strName, 0x00, sizeof(char) * 64);
	strncpy(strName, m_strName.toLatin1(), 63);
}

void CDialogMakeQuake::CreateControls()
{
	//m_dialogPrint = new QPrintDialog(m_printer, this); // signals/slots done on the open()
	
    m_groupName    = new QGroupBox(this);
    m_groupSpin    = new QGroupBox(this);
	m_groupButton = new QGroupBox(this);
	m_groupPrinter = new QGroupBox(this);

	m_buttonStart = new QPushButton(tr("&Start"));
	m_buttonCancel = new QPushButton(tr("&Cancel"));
	
    connect(m_buttonStart, SIGNAL(clicked()), this, SLOT(onStart()));
    connect(m_buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
	
	m_buttonStart->setDefault(true);

    // control 1 - name
    m_labelName = new QLabel(tr("Name:"), this);
    m_textctrlName = new QLineEdit(this);
	m_textctrlName->setMaxLength(63);
	m_textctrlName->setText(m_strName);
		
    // control 2 - countdown
    //m_labelTime = new QLabel(tr(""), this);
    m_spinctrlCountdown = new QSpinBox(this);
	m_spinctrlCountdown->setPrefix(tr("Countdown is "));
	m_spinctrlCountdown->setSuffix(tr(" seconds long"));
    m_spinctrlCountdown->setMinimum(1);
    m_spinctrlCountdown->setMaximum(60);
    m_spinctrlCountdown->setSingleStep(1);
	m_spinctrlCountdown->setValue(m_iCountdown);
	
    // control 3 - shake time
    //m_labelTime = new QLabel(tr(""), this);
    m_spinctrlTime = new QSpinBox(this);
	m_spinctrlTime->setPrefix(tr("Quake record time is "));
	m_spinctrlTime->setSuffix(tr(" seconds"));
    m_spinctrlTime->setMinimum(1);
    m_spinctrlTime->setMaximum(60);
    m_spinctrlTime->setSingleStep(1);
	m_spinctrlTime->setValue(m_iTime);
	
	// control 4 printer
	m_labelPrinter = new QLabel("Printer"); 
	m_comboPrinter = new QComboBox(this);
	
	// add printer combo box info
	m_qlpi = QPrinterInfo::availablePrinters();
	int iSel = 0, iCtr = 1;

	m_comboPrinter->addItem(PRINTER_PDF, 0);
	if (m_strPrinter == PRINTER_PDF) iSel = 0;
	if (m_qlpi.size() > 0) {
		QList<QPrinterInfo>::iterator itP = m_qlpi.begin();
		while (itP != m_qlpi.end()) {
			QString strPI = itP->printerName();
			if (strPI == m_strPrinter) iSel = iCtr;
		    m_comboPrinter->addItem(strPI, iCtr);
			itP++;
			iCtr++;
		}
	}
	m_comboPrinter->setCurrentIndex(iSel);
	
    //connect(m_comboPrinter, SIGNAL(clicked()), this, SLOT(onPrinterSelect()));	

	
	// now do the grid layout of the controls
    m_gridlayout = new QGridLayout(this);
    m_gridlayout->addWidget(m_labelName, 0, 0);
    m_gridlayout->addWidget(m_textctrlName, 0, 1);
	m_groupName->setLayout(m_gridlayout);

	m_layoutSpin = new QVBoxLayout(this);
    m_layoutSpin->addWidget(m_spinctrlCountdown, 1, Qt::AlignTop);
    m_layoutSpin->addWidget(m_spinctrlTime, 1, Qt::AlignBottom);
	m_groupSpin->setLayout(m_layoutSpin);
	
	// layout printer stuff
	m_layoutPrinter = new QHBoxLayout(this);
	m_layoutPrinter->addWidget(m_labelPrinter, 1, Qt::AlignLeft);
	m_layoutPrinter->addWidget(m_comboPrinter, 1, Qt::AlignRight);
	m_groupPrinter->setLayout(m_layoutPrinter);
	
	// layout buttons
	m_layoutButton = new QHBoxLayout(this);
    m_layoutButton->addWidget(m_buttonStart, 1, Qt::AlignLeft);
    m_layoutButton->addWidget(m_buttonCancel, 1, Qt::AlignRight);
	m_groupButton->setLayout(m_layoutButton);
	
	// do the overall layout for the dialog box

    m_layoutMain = new QVBoxLayout(this);
    m_layoutMain->addWidget(m_groupName);
    m_layoutMain->addWidget(m_groupSpin);
    m_layoutMain->addSpacing(12);
    m_layoutMain->addWidget(m_groupPrinter);
    m_layoutMain->addSpacing(12);
    m_layoutMain->addWidget(m_groupButton);
    m_layoutMain->addStretch(1);
	
    setLayout(m_layoutMain);
}

/*
void CDialogMakeQuake::onPrinterAccepted()
{
	m_printer = m_dialogPrint->printer();
	if (m_printer && m_printer->isValid()) {
		m_strPrinterName = m_printer->printerName();
		m_strPrinterSelectionOption = m_printer->printerSelectionOption();
		if (m_labelPrinter) m_labelPrinter->setText(m_strPrinterName);
	}
}

void CDialogMakeQuake::onPrinterSelect()
{
	// bring up printer dialog and select string, or else defaults to PDF file
	m_dialogPrint->open(this, SLOT(onPrinterAccepted())); // == QDialog::Accepted) {
}
*/

void CDialogMakeQuake::onStart()
{
	//QString strError;
	m_bStart = true;
	
	m_iTime = m_spinctrlTime->value();
	m_iCountdown = m_spinctrlCountdown->value();
	m_strName = m_textctrlName->text();
	
	int iIndex = m_comboPrinter->currentIndex();
	m_strPrinter = m_comboPrinter->currentText();
	if (iIndex <= 0) m_strPrinter = PRINTER_PDF;
	else if (iIndex-1 < m_qlpi.size()) {
		m_printerInfo = m_qlpi.at(iIndex-1);
	}
	
	//if (m_bStart) {
		close();
	//}
	//else { // must have an error
	//	QMessageBox::warning(this, tr("Error in Values"), QString(tr("Please correct the following error(s):<BR><BR>")) + strError, QMessageBox::Ok);
	//}
}
