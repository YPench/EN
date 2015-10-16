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

#include "CEDT.h"
#include "afxwin.h"
#include "afxcmn.h"

class CConsensus_Dlg;

class CCEDT_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CCEDT_Dlg)

public:
	CCEDT_Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCEDT_Dlg();

// Dialog Data
	enum { IDD = IDD_CEDT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedResetCEDT();
	afx_msg void OnBnClickedACETrainingAndTesting();
	afx_msg void OnBnClickedTrainingModelsButton();
	afx_msg void OnBnClickedButtonAnalysizeBoundaryRet();
	afx_msg void OnBnClickedOpenRetHistory();
	afx_msg void OnBnClickedCedtDocRecog();
	afx_msg void OnBnClickedCedtLoadModels();
	afx_msg void OnBnClickedCedtSentDet();
	afx_msg void OnBnClickedCedtSentRecog();
	afx_msg void OnBnClickedCedtSetTrainintPath();
	afx_msg void OnBnClickedCedtSetTestPath();
	afx_msg void OnBnClickedCedtTestingModel();
	afx_msg void OnBnClickedCedtSetRecogDoc();
	afx_msg void OnBnClickedCedtSentClear();

	void Disable_Button_CEDT();
	void Enable_Usable_Button_CEDT();
	void CCEDT_Dlg::Updata_CEDT_Configurations();

public:
	HANDLE ImpThread;
	DWORD ImpphreadId;
	STARTUPINFO si;
	PROCESS_INFORMATION processInformation;
	
public:
	CCEDT m_CCEDT;
	CConsensus_Dlg* p_CParentDlg;

	CButton m_ACETTButton;
	CButton m_TrainingModelsButton;
	CButton m_OHistoryB;

	CButton m_DocRecogButton;
	CButton m_LoadModelButton;
	CButton m_SentDetButton;
	CButton m_SentRecogButton;
	CButton m_TestModelButton;

	CRichEditCtrl m_SINputEditCtrl;
	CRichEditCtrl m_TestFilePath;
	CRichEditCtrl m_TrainFilePath;

	string CEDT_Data_Floder;
	string NE_Testing_Cases_Path;
	string NE_Training_Case_Path;
	string NE_InfoPath;
	string NE_Doc_Path;
	CString Catch_Input_Sentence;

	size_t m_CanditIte;
	size_t m_BoundIte;
	size_t m_nGrossEval;

	bool CEDT_Busy_Flag;
	bool ACE_Corpus_Training_and_Testing_Flag;
	bool Training_Models_by_Cases_Flag;
	bool Loading_Models_Flag;
	bool Documents_Recognization_Flag;
	bool Testing_Models_Flag;

	BOOL CEDT_Gen_Info_Flag;
	BOOL m_DetSTART_Flag;
	BOOL m_DetEND_Flag;
	BOOL NE_Detection_Flag;
	BOOL NE_Recogniation_Flag;
	BOOL NE_Extend_Flag;
	BOOL NE_Head_Flag;

	BOOL Gen_Words_Feature_Flag;
	BOOL Gen_Char_Feature_Flag;
	BOOL Gen_nGram_Feature_Flag;
	
	BOOL Auto_Load_Model_Flag;
	BOOL Forbid_Rewrite_Models_Flag;
	
	CRichEditCtrl m_DocPathEdit;

	
};
