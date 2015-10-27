/*
 * Copyright (C) 2015 by Yanping Chen <ypench@gmail.com>
 * Begin       : 01-Oct-2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser GPL (LGPL) as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
 */
#pragma once


// CENET_Dlg dialog
#include "DENer_Dlg.h"
#include "ENET.h"
#include "afxwin.h"
#include "afxcmn.h"
class CConsensus_Dlg;

class CENET_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CENET_Dlg)

public:
	CENET_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CENET_Dlg();
// Dialog Data
	enum { IDD = IDD_ANALYZER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAnalDewer();
	afx_msg void OnBnClickedENETDocEventOrg();
	afx_msg void OnBnClickedAnalEntityRun();
	afx_msg void OnBnClickedAnalRelRun();
	afx_msg void OnBnClickedAnalRunAll();
	afx_msg void OnBnClickedAnalGenEn();
	afx_msg void OnBnClickedAnalEntityGen();
	afx_msg void OnBnClickedAnalRelGen();
	afx_msg void OnBnClickedAnalEntityViewer();
	afx_msg void OnBnClickedAnalRelViewer();
	afx_msg void OnBnClickedAnalExample();
	afx_msg void OnBnClickedEnetSpCheck();
	afx_msg void OnBnClickedAnalReset();

	afx_msg LRESULT ENET_Message_Processing( WPARAM wParam, LPARAM lParam );

	void Reckon_By_Time(bool START_Flag);
	void Updata_ENET_Configurations();
	void Dispaly_Event_Network_Pictures();

	CDENer_Dlg* p_m_CDENer_Dlg;
	CConsensus_Dlg* p_CParentDlg;
	ENET m_CENET;
	CMenu m_Menu;

	HANDLE ImpThread;
	DWORD ImpphreadId;
	CTime StartTime;
	CTime EndTime;

	bool Opened_CDENer_Dlg_Flag;
	bool CENET_Busy_Flag;
	bool Running_Named_Entity_Flag;
	bool Running_Relation_Flag;
	bool Running_Document_Event_Flag;
	bool Generating_Entity_Flag;
	bool Generating_Relation_Flag;
	bool Filtering_Vertex_and_Edges_Flag;
	BOOL m_RelTYPECheck;
	BOOL m_RelSubtypeCheck;
	bool Loading_EN_Info_Flag;
	
	string ACECorpusFolder;
	string GigawordFolder;

	BOOL m_PersonCheck;
	BOOL m_LocationCheck;
	BOOL m_OrgCheck;

	void Disable_Button_ENET();
	void Enable_Usable_Button_ENET();


	bool Command_From_Notification(const char* command);

	bool Collecting_Info_Flag;
	bool Deleting_Events_Organizing_Files_Flag;
	bool PLT_Analysis_Flag;
	bool Action_Analysis_Flag;
	string DeletingFileProx;
	
	CStatic m_AnalPicture;
	
	CButton m_RunEntityButton;
	CButton m_DocRunButton;
	CButton m_RunRelationButton;
	
	CButton m_RunAll;
	bool Collected_Doc_Event_Flag;
	string Collected_EventName;
	string Collected_EventPath;
	string Collected_KeyWords;
	string Collected_FilePrix;

	CRichEditCtrl m_EventNameEdit;
	CRichEditCtrl m_KeyWordsEdit;
	
	CButton m_EntityResultButton;
	CButton m_EntityReviewButton;
	CButton m_RelResultButton;
	CButton m_RelReviewButton;
	CButton m_GenViewENButton;

	set<string> m_NE_TYPE_s;
	set<string> m_REL_TYPE_s;
	
	BOOL m_SP_Check;
	CComboBox m_CentboBox;
	CComboBox m_SPArg1boBox;
	CComboBox m_SPArg2boBox;
	CRichEditCtrl m_BottonEdit;
	double m_NamedEntityWeight;
	double m_RelationWeight;
	size_t m_MaxVexNum;

	bool Entity_Star_Layout_Flag;
	bool Short_Path_Flag;
	CButton m_LoadInfoButton;
	CButton m_ENETResetButton;

	set<string> m_FilterEntity_s;
	set<string> m_FilterRelation_s;
	BOOL m_Filter_PER_Flag;
	BOOL m_Filter_LOC_Flag;
	BOOL m_Filter_ORG_Flag;
	BOOL m_Filter_PER_SOC_Flag;
	BOOL m_Filter_GEN_AFF_Flag;
	BOOL m_Filter_ORG_AFF_Flag;
	BOOL m_Filter_PHYS_Flag;
	BOOL m_Filter_PART_WHOLE_Flag;
};
