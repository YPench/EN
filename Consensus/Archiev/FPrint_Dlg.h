#pragma once


// CFPrint_Dlg dialog

class CFPrint_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CFPrint_Dlg)

public:
	CFPrint_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFPrint_Dlg();

// Dialog Data
	enum { IDD = IDD_FINGERPRINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
