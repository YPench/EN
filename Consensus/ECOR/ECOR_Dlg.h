/*
 * Copyright by Yanping Chen <ypench@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
