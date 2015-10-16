#pragma once
#include "afxwin.h"
#include "..\\DENer_Dlg.h"

// CConvention_Dlg dialog

class CConvention_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CConvention_Dlg)

public:
	CConvention_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConvention_Dlg();

// Dialog Data
	enum { IDD = IDD_CONVENTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck12();
	afx_msg void OnBnClickedEntityViewButton();
	afx_msg void OnBnClickedFingerViewButton();
	afx_msg void OnBnClickedRelationViewButton();
	afx_msg void OnBnClickedTemperViewButton();
	afx_msg void OnBnClickedIntergationViewButton();
	afx_msg void OnBnClickedVisualizationViewButton();
	afx_msg void OnBnClickedRunningButton();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedResetButton();
	afx_msg void OnBnClickedRunEntity();
	afx_msg void OnBnClickedRunFinger();
	afx_msg void OnBnClickedRunRelation();
	afx_msg void OnBnClickedRunHot();
	afx_msg void OnBnClickedEntityVisualize();
	afx_msg void OnBnClickedFingerVisualize();
	afx_msg void OnBnClickedRelationVisualize();
	afx_msg void OnBnClickedHotVisualize();


	void Disable_Button_Convertion();
	void Enable_Usable_Button_Convertion();
	int igraph_Port(int argc, char* argv[]);

public:

	CButton m_EntityPadB;
	CButton m_FingerPadB;
	CButton m_RelationPadB;
	CButton m_HotPadB;

	CButton m_IntegB;
	CButton m_enIntegB;
	
	CButton m_VisEntityB;
	CButton m_VisFingerB;
	CButton m_VisRelationB;
	CButton m_VisHotB;
	CButton m_VisIntegB;
	
	string Convention_Data_Floder;
	CDENer_Dlg* p_m_CDENer_Dlg;

	double m_Cycle;
	double m_Duration;
	afx_msg void OnBnClickedDocEventsButten();
};
