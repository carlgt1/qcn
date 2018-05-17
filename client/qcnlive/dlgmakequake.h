#ifndef _DLG_MAKEQUAKE_H_
#define _DLG_MAKEQUAKE_H_

#include "qcnqt.h"
#include <QList>
#include <QPrinterInfo>

class CDialogMakeQuake  : public QDialog
{	
private:
	Q_OBJECT
    void InitPointers();
    void CreateControls();

	//data
	QString m_strName;
	QString m_strPrinter;
	QPrinterInfo m_printerInfo;
	QList<QPrinterInfo> m_qlpi;
	
	int m_iTime;
	int m_iCountdown;
	bool m_bStart; 

	// controls & layouts
	// informative labels for the various controls
	QLabel* m_labelName;
	QLabel* m_labelPrinter;
	
    QLineEdit* m_textctrlName; 
    QSpinBox* m_spinctrlTime;
    QSpinBox* m_spinctrlCountdown;

	QPushButton* m_buttonStart;
	QPushButton* m_buttonCancel;
	QComboBox* m_comboPrinter;
	
	QVBoxLayout* m_layoutMain;
	QVBoxLayout* m_layoutSpin;
	QHBoxLayout* m_layoutButton;
	QHBoxLayout* m_layoutPrinter;
	
	QGroupBox* m_groupName;
	QGroupBox* m_groupSpin;
	QGroupBox* m_groupButton;
	QGroupBox* m_groupPrinter;
	
	QGridLayout* m_gridlayout;	
	
	//QPrintDialog* m_dialogPrint;

	
private slots:
	void onStart();		
	//void onPrinterSelect();
	//void onPrinterAccepted();
	
public:
	CDialogMakeQuake(const int iTime, const int iCountdown, const QString& strPrn, QWidget* parent, Qt::WindowFlags f);
    ~CDialogMakeQuake();

	void getUserString(char* strName);
        int getMakeQuakeTime() const { return m_iTime; }
        int getMakeQuakeCountdown() const { return m_iCountdown; }
	bool start() { return m_bStart; };
	
	const QPrinterInfo& getMakeQuakePrinterInfo() { return m_printerInfo; }
	void setMakeQuakePrinterInfo(const QPrinterInfo& prn) { m_printerInfo = prn; }
	
	const QString& getMakeQuakePrinterString() { return m_strPrinter; }
	void setMakeQuakePrinterString(const QString& prn) { m_strPrinter = prn; }
	
};

#endif // _DLG_MAKEQUAKE_H_
