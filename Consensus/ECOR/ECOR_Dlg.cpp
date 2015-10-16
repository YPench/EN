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
// ECOR_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\\Consensus.h"
#include "ECOR_Dlg.h"


// ECOR_Dlg dialog

IMPLEMENT_DYNAMIC(ECOR_Dlg, CDialog)

ECOR_Dlg::ECOR_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(ECOR_Dlg::IDD, pParent)
	, m_nCross(2)
	, m_ECOR_ite(90)
	, m_ALoadCandits_Flag(FALSE)
	, m_UpdateCandits_Flag(TRUE)
	, m_PsAutoLoad_Flag(TRUE)
	, m_PsUpdate_Flag(FALSE)
	, m_SaveTested_Flag(TRUE)
{
	ECOR_Busy_Flag = false;
	ECOR_Training_and_Testing_Flag = false;
	ECOR_Stanford_Corpus_Parzing_Flag = false;
	ECOR_Coreference_Parting_Flag = false;
}

ECOR_Dlg::~ECOR_Dlg()
{
}

void ECOR_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ECOR_N_CROSS, m_nCross);
	DDX_Text(pDX, IDC_RICHEDIT_ECOR_ITE, m_ECOR_ite);
	DDX_Check(pDX, IDC_ECOR_LOAD_CANDIT, m_ALoadCandits_Flag);
	DDX_Check(pDX, IDC_ECOR_UPDATE_CANDIT, m_UpdateCandits_Flag);
	DDX_Check(pDX, IDC_ECOR_PS_A_LOA, m_PsAutoLoad_Flag);
	DDX_Check(pDX, IDC_CEOR_PS_UPDATE, m_PsUpdate_Flag);
	DDX_Check(pDX, IDC_ECOR_SAVE_TESTED, m_SaveTested_Flag);
}


BEGIN_MESSAGE_MAP(ECOR_Dlg, CDialog)
	ON_BN_CLICKED(IDC_ECOR_TRAIN_TEST, &ECOR_Dlg::OnBnClickedEcorTrainTest)
	ON_BN_CLICKED(IDC_ECOR_STANFORD_PARSING, &ECOR_Dlg::OnBnClickedEcorStanfordParsing)
	ON_BN_CLICKED(IDC_ECOR_UPDATE_CANDIT, &ECOR_Dlg::OnBnClickedEcorUpdateCandit)
	ON_BN_CLICKED(IDC_ECOR_PARTING, &ECOR_Dlg::OnBnClickedEcorParting)
END_MESSAGE_MAP()


// ECOR_Dlg message handlers
DWORD WINAPI ECOR_Implement_Threaad(LPVOID pParam)
{
	ECOR_Dlg& dlgRef = *(ECOR_Dlg*)pParam;
	CConsensus_Dlg& ParDlgRef = *dlgRef.p_CParentDlg;
	if(dlgRef.p_CParentDlg->XML_Parsing_Flag){
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_ACE_Folder_Files_Filter(ParDlgRef.ACECorpusFolder, ParDlgRef.m_ACE_Corpus);
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_ACE_Folder_For_sgm_Files_Filter(ParDlgRef.ACECorpusFolder, ParDlgRef.m_ACE_Corpus);
		dlgRef.p_CParentDlg->XML_Parsing_Flag = false;
		dlgRef.p_CParentDlg->ACE_Coupus_Loaded_Flag = true;
		dlgRef.p_CParentDlg->Output_MSG("\"XML Parse\" is complete...", 1);
	}
	if(dlgRef.ECOR_Training_and_Testing_Flag){
		dlgRef.p_CParentDlg->Output_MSG("\"CEOR Training and Testing\" is started...", 1);
		dlgRef.m_CECOR.CECOR_Training_and_Testing_Port(ParDlgRef.m_ACE_Corpus, ParDlgRef.ACECorpusFolder.c_str());
		dlgRef.p_CParentDlg->Output_MSG("\"CEOR Training and Testing\" is done...", 1);
	} 
	if(dlgRef.ECOR_Stanford_Corpus_Parzing_Flag){
		dlgRef.m_CECOR.ECOR_Stanford_Parsing_Port(ParDlgRef.m_ACE_Corpus, ParDlgRef.ACECorpusFolder.c_str());
		dlgRef.p_CParentDlg->Output_MSG("\"Stanford Parsing Corpus\" is done...", 1);
	}
	if(dlgRef.ECOR_Coreference_Parting_Flag){
		dlgRef.m_CECOR.ECOR_Coreference_Parting_Port(ParDlgRef.m_ACE_Corpus, ParDlgRef.ACECorpusFolder.c_str());
		dlgRef.p_CParentDlg->Output_MSG("\"Coreference Parting\" is done...", 1);
	}
	
	dlgRef.ECOR_Busy_Flag = false;
	dlgRef.ECOR_Training_and_Testing_Flag = false;
	dlgRef.ECOR_Stanford_Corpus_Parzing_Flag = false;
	dlgRef.ECOR_Coreference_Parting_Flag = false;

	dlgRef.Enable_Usable_Button_ECOR();
	ExitThread(0);
}

void ECOR_Dlg::Enable_Usable_Button_ECOR()
{


}
void ECOR_Dlg::Updata_ECOR_Configurations()
{
	UpdateData(TRUE);
	m_CECOR.m_nCross = m_nCross;
	m_CECOR.m_ECOR_ite = m_ECOR_ite;
	m_CECOR.m_ALoadCandits_Flag = m_ALoadCandits_Flag;
	m_CECOR.m_UpdateCandits_Flag = m_UpdateCandits_Flag;
	m_CECOR.m_PsAutoLoad_Flag = m_PsAutoLoad_Flag;
	m_CECOR.m_PsUpdate_Flag = m_PsUpdate_Flag;
	m_CECOR.m_SaveTested_Flag = m_SaveTested_Flag;
	OnBnClickedEcorUpdateCandit();
}



void ECOR_Dlg::OnBnClickedEcorTrainTest()
{
	if(ECOR_Busy_Flag){
		p_CParentDlg->Output_MSG("\"ACE Corpus Training and Testing\" is not usable, please wait...", 1);
		return;
	}
	ECOR_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"ACE Corpus Training and Testing\" is started ...", 1);

	string DataFolder = DATA_FOLDER;
	if(!NLPOP::FolderExist(NLPOP::string2CString(DataFolder))){
		_mkdir(DATA_FOLDER);
	}

	ECOR_Training_and_Testing_Flag = true;
	//---------------
	if(!p_CParentDlg->ACE_Coupus_Loaded_Flag){
		p_CParentDlg->XML_Parsing_Flag = true;
	}
	UpdateData(TRUE);
	Updata_ECOR_Configurations();

	ImpThread = CreateThread(NULL, 0, ECOR_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}



void ECOR_Dlg::OnBnClickedEcorStanfordParsing()
{
	if(ECOR_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Stanford Parsing Corpus\" is not usable, please wait...", 1);
		return;
	}
	ECOR_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Stanford Parsing Corpus\" is started ...", 1);

	string DataFolder = DATA_FOLDER;
	if(!NLPOP::FolderExist(NLPOP::string2CString(DataFolder))){
		_mkdir(DATA_FOLDER);
	}
	if(!p_CParentDlg->ACE_Coupus_Loaded_Flag){
		p_CParentDlg->XML_Parsing_Flag = true;
	}
	ECOR_Stanford_Corpus_Parzing_Flag = true;
	//---------------

	UpdateData(TRUE);
	Updata_ECOR_Configurations();

	ImpThread = CreateThread(NULL, 0, ECOR_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void ECOR_Dlg::OnBnClickedEcorUpdateCandit()
{
	UpdateData(TRUE);
	if(m_UpdateCandits_Flag){
		m_ALoadCandits_Flag = FALSE;
	}
	else{
		m_ALoadCandits_Flag = TRUE;
	}
	UpdateData(FALSE);
}

void ECOR_Dlg::OnBnClickedEcorParting()
{
	if(ECOR_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Coreference Parting\" is not usable, please wait...", 1);
		return;
	}
	ECOR_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Coreference Parting\" is started ...", 1);

	string DataFolder = DATA_FOLDER;
	if(!NLPOP::FolderExist(NLPOP::string2CString(DataFolder))){
		_mkdir(DATA_FOLDER);
	}
	if(!p_CParentDlg->ACE_Coupus_Loaded_Flag){
		p_CParentDlg->XML_Parsing_Flag = true;
	}
	ECOR_Coreference_Parting_Flag = true;
	//---------------

	UpdateData(TRUE);
	Updata_ECOR_Configurations();

	ImpThread = CreateThread(NULL, 0, ECOR_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}
