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
#include "afxcmn.h"
#include "afxwin.h"

#define MAX_TERM_NUM 25
// CDENer_Dlg dialog
class CENET_Dlg;
class CDENer_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CDENer_Dlg)

public:
	CDENer_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDENer_Dlg();

// Dialog Data
	enum { IDD = IDD_DOC_EVENT_REVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:

	CENET_Dlg* p_CParentDlg;
	CListCtrl m_DocEventList;
	afx_msg void OnBnClickedAnalLoadDoc();
	afx_msg void OnBnClickedViewerCollectDocE();
	afx_msg void OnBnClickedViewerFrontTime();
	afx_msg void OnBnClickedViewerNextTime();
	afx_msg void OnBnClickedChildren();
	afx_msg void OnBnClickedFather();

	void Init_Document_Event_Viewer(const char* cWorkSpace);
	void Load_and_Display_Timestamp_inner_EventTree(const char* cTimestampPath);
	int Iteration_Finding_Hierarchy_Number(const char* cTimestampPath, int Num);
	void Enable_Usable_Button_DEVER();
	void Disable_Button_DEVER();
	void Init_Reviewer();

	CRichEditCtrl m_TimestampEdit;
	CRichEditCtrl m_DocNumEdit;
	CRichEditCtrl m_EventNumEdit;
	CRichEditCtrl m_HierarchyEdit;
	CRichEditCtrl m_SelEventEdit;

	CButton m_ActiveAnalysisButton;
	CButton m_ChildrenButton;
	CButton m_FatherButton;
	CButton m_FrontTimeButton;
	CButton m_NextTimeButton;

	map<string, map<size_t, string>> m_TotalID2EventName_map;
	map<string, map<size_t, string>> m_TotalID2EventPath_map;
	map<string, map<size_t, string>> m_TotalID2Keywords_map;
	map<string, map<string, pair<size_t, size_t>>> m_TotalDocInfoOfTimestamp_map;

	map<string, map<string, size_t>> m_TotalEventName2IDmap;
	map<string, map<string, string>> m_TotalChild2Father_map;
	
	map<size_t, size_t> m_CurnItem2ID_map;
	map<string, string> m_Timestamp2Path_map;
	
	int m_Collected_nItem;
	int m_hierarchy;
	int m_TotalHierarchy;
	CString m_DocEventFolder;
	
	map<string, map<string, set<string>>> m_TimetampEventTree;
	map<string, map<string, set<string>>>::iterator m_EventTreeIter;
	
	
};
