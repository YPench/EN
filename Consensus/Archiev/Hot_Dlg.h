#pragma once


// CHot_Dlg dialog

class CHot_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CHot_Dlg)

public:
	CHot_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHot_Dlg();

// Dialog Data
	enum { IDD = IDD_TEMPER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
