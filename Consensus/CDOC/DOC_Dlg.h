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

#include "CDOC.h"
#include "afxwin.h"
// CDOC_Dlg dialog
class CConsensus_Dlg;
class CDOC_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CDOC_Dlg)

public:
	CDOC_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDOC_Dlg();

// Dialog Data
	enum { IDD = IDD_CDOC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCdocSparsing();
	afx_msg void OnBnClickedCdocMatrix();
	afx_msg void OnBnClickedDocParseAce();
	afx_msg void OnBnClickedCdocLdaTraining();
	afx_msg void OnBnClickedCdocActiveAce();
	afx_msg void OnBnClickedCdocActiveGigaword();
	afx_msg void OnBnClickedCdocDoAll();
	afx_msg void OnBnClickedCdocClustering();
	afx_msg void OnBnClickedDocFilteringTerms();
	afx_msg void OnBnClickedCdocActiveOrg();
	afx_msg void OnBnClickedCdocOrganizing();

	void Enable_Usable_Button_CDOC();
	void Disable_Button_CDOC();
	void Updata_CDOC_Configurations();
	void Reckon_By_Time(bool START_Flag);

	CButton m_ParsingGigaButton;
	CButton m_DocMatrixButton;
	CButton m_LDAButton;
	CButton m_ParsingACEButton;
	CButton m_FilteringTermButton;
	CButton m_EventClusteringButton;
	CButton m_EventOrganizingButton;

	CConsensus_Dlg* p_CParentDlg;
	CDOC m_CDOC;

	HANDLE ImpThread;
	DWORD ImpphreadId;

	CTime StartTime;
	CTime EndTime;

	bool CDOC_Busy_Flag;
	bool Parsing_Gigaword_Flag;
	bool Generating_Doc_Matrix_Flag;
	bool Parsing_ACE_Corpus_Flag;
	bool LDA_Training_Flag;
	bool Filtering_Term_Flag;
	bool Documents_Clustering_Flag;
	bool Document_Event_Organizing_Flag;
	
	string ACECorpusFolder;
	string GigawordFolder;
	string m_DocMatrix;

	BOOL m_DebugDoc_Flag;
	BOOL m_ActiveACE_Flag;
	BOOL m_ActiveGigaWord_Flag;
	BOOL m_AcitveOrg_Flag;
	BOOL LCWCC_Flag;
	BOOL ACE_ENTITY_Flag;
	BOOL ICTCLAS_Flag;

	int m_HighRatio;
	int m_LowRatio;
	int m_LowFreq;
	int m_DebugDocNum;
	int m_Events;
	float m_alpha;
	float m_beta;
	int m_twords;
	int m_niters;
	int m_Hierarchies;
	int m_TimeStep;

	afx_msg void OnBnClickedCheck1();
};
