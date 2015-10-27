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

#include "ECOR.h"
// ECOR_Dlg dialog
class CConsensus_Dlg;

class ECOR_Dlg : public CDialog
{
	DECLARE_DYNAMIC(ECOR_Dlg)

public:
	ECOR_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ECOR_Dlg();

// Dialog Data
	enum { IDD = IDD_ECOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	

public:
	afx_msg void OnBnClickedEcorTrainTest();

	void Enable_Usable_Button_ECOR();
	void Updata_ECOR_Configurations();

public:
	CConsensus_Dlg* p_CParentDlg;
	bool ECOR_Busy_Flag;
	HANDLE ImpThread;
	DWORD ImpphreadId;	

	//bool ECOR_ACE_Corpus_Parzing_Flag;
	bool ECOR_Training_and_Testing_Flag;
	bool ECOR_Stanford_Corpus_Parzing_Flag;
	bool ECOR_Coreference_Parting_Flag;


	CECOR m_CECOR;

	size_t m_nCross;
	afx_msg void OnBnClickedEcorStanfordParsing();
	size_t m_ECOR_ite;
	BOOL m_ALoadCandits_Flag;
	BOOL m_UpdateCandits_Flag;
	BOOL m_PsAutoLoad_Flag;
	BOOL m_PsUpdate_Flag;
	afx_msg void OnBnClickedEcorUpdateCandit();
	BOOL m_SaveTested_Flag;
	afx_msg void OnBnClickedEcorParting();
};
