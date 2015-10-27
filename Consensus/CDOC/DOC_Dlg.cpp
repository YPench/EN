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
// DOC_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\\Consensus.h"
#include "DOC_Dlg.h"
#include "DOCCOM.h"

// CDOC_Dlg dialog

IMPLEMENT_DYNAMIC(CDOC_Dlg, CDialog)

CDOC_Dlg::CDOC_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDOC_Dlg::IDD, pParent)
	, LCWCC_Flag(TRUE)
	, ACE_ENTITY_Flag(FALSE)
	, ICTCLAS_Flag(FALSE)
	, m_Events(max_event_NUM)
	, m_alpha(0)
	, m_beta((float)0.1)
	, m_twords(100)
	, m_niters(2000)
	, m_HighRatio(5)
	, m_LowRatio(5)
	, m_LowFreq(10)
	, m_DebugDocNum(10)
	, m_DebugDoc_Flag(TRUE)
	, m_ActiveACE_Flag(FALSE)
	, m_ActiveGigaWord_Flag(TRUE)
	, m_Hierarchies(2)
	, m_TimeStep(5)
	, m_AcitveOrg_Flag(TRUE)
{
	CDOC_Busy_Flag = false;
	Parsing_Gigaword_Flag = false;
	Generating_Doc_Matrix_Flag = false;
	Parsing_ACE_Corpus_Flag = false;
	LDA_Training_Flag = false;
	Filtering_Term_Flag = false;
	Documents_Clustering_Flag = false;
	Document_Event_Organizing_Flag = false;
	m_DocMatrix = CDOC_FOLDER;
	m_DocMatrix += "DocMatrix";
	m_alpha = (float)(1.0*50/m_Events);
	//UpdateData(FALSE);
	//OnPaint();
}

CDOC_Dlg::~CDOC_Dlg()
{
}

void CDOC_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CDOC_SPARSING, m_ParsingGigaButton);
	DDX_Control(pDX, IDC_CDOC_MATRIX, m_DocMatrixButton);
	DDX_Control(pDX, IDC_DOC_PARSE_ACE, m_ParsingACEButton);
	DDX_Check(pDX, IDC_DOC_LCWCC, LCWCC_Flag);
	DDX_Check(pDX, IDC_DOC_ACE_ENTITY, ACE_ENTITY_Flag);
	DDX_Check(pDX, IDC_DOC_ICTCLAS, ICTCLAS_Flag);
	DDX_Text(pDX, IDC_CDOC_TOPICS, m_Events);
	DDX_Text(pDX, IDC_CDOC_ALPHA, m_alpha);
	DDX_Text(pDX, IDC_CDOC_BETA, m_beta);
	DDX_Text(pDX, IDC_CDOC_TWORDS, m_twords);
	DDX_Text(pDX, IDC_CDOC_NITERS, m_niters);
	DDX_Control(pDX, IDC_CDOC_LDA_TRAINING, m_LDAButton);
	DDX_Text(pDX, IDC_CDOC_HIGH_RATION, m_HighRatio);
	DDX_Text(pDX, IDC_CDOC_LOW_RATION, m_LowRatio);
	DDX_Text(pDX, IDC_CDOC_LOW_FREQ, m_LowFreq);
	DDX_Control(pDX, IDC_DOC_FILTERING_TERMS, m_FilteringTermButton);
	DDX_Text(pDX, IDC_CDOC_DUBEG_NUM, m_DebugDocNum);
	DDX_Check(pDX, IDC_CDOC_DEGUG_DOC, m_DebugDoc_Flag);
	DDX_Check(pDX, IDC_CDOC_ACTIVE_ACE, m_ActiveACE_Flag);
	DDX_Check(pDX, IDC_CDOC_ACTIVE_GIGAWORD, m_ActiveGigaWord_Flag);
	//DDX_Control(pDX, IDC_CDOC_LDA_TWORDS, m_LDATopwordsButton);
	DDX_Control(pDX, IDC_CDOC_ORGANIZING, m_EventOrganizingButton);
	DDX_Text(pDX, IDC_CDOC_HIERARCHIES, m_Hierarchies);
	DDX_Text(pDX, IDC_CDOC_TIMESTEP, m_TimeStep);
	DDX_Check(pDX, IDC_CDOC_ACTIVE_ORG, m_AcitveOrg_Flag);
	DDX_Control(pDX, IDC_CDOC_CLUSTERING, m_EventClusteringButton);
}


BEGIN_MESSAGE_MAP(CDOC_Dlg, CDialog)
	ON_BN_CLICKED(IDC_CDOC_SPARSING, &CDOC_Dlg::OnBnClickedCdocSparsing)
	ON_BN_CLICKED(IDC_CDOC_MATRIX, &CDOC_Dlg::OnBnClickedCdocMatrix)
	ON_BN_CLICKED(IDC_DOC_PARSE_ACE, &CDOC_Dlg::OnBnClickedDocParseAce)
	ON_BN_CLICKED(IDC_CDOC_LDA_TRAINING, &CDOC_Dlg::OnBnClickedCdocLdaTraining)
	ON_BN_CLICKED(IDC_DOC_FILTERING_TERMS, &CDOC_Dlg::OnBnClickedDocFilteringTerms)
	ON_BN_CLICKED(IDC_CDOC_ACTIVE_ACE, &CDOC_Dlg::OnBnClickedCdocActiveAce)
	ON_BN_CLICKED(IDC_CDOC_ACTIVE_GIGAWORD, &CDOC_Dlg::OnBnClickedCdocActiveGigaword)
	ON_BN_CLICKED(IDC_CDOC_DO_ALL, &CDOC_Dlg::OnBnClickedCdocDoAll)
	ON_BN_CLICKED(IDC_CDOC_CLUSTERING, &CDOC_Dlg::OnBnClickedCdocClustering)
	ON_BN_CLICKED(IDC_CDOC_ACTIVE_ORG, &CDOC_Dlg::OnBnClickedCdocActiveOrg)
	ON_BN_CLICKED(IDC_CDOC_ORGANIZING, &CDOC_Dlg::OnBnClickedCdocOrganizing)
	ON_BN_CLICKED(IDC_CDOC_BREAKPOINT, &CDOC_Dlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CDOC_Dlg message handlers



DWORD WINAPI CDOC_Implement_Threaad(LPVOID pParam)
{
	CDOC_Dlg& dlgRef = *(CDOC_Dlg*)pParam;
	dlgRef.p_CParentDlg->System_Busy_Flag = true;
	if(dlgRef.p_CParentDlg->XML_Parsing_Flag){
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_ACE_Folder_Files_Filter(dlgRef.ACECorpusFolder, dlgRef.p_CParentDlg->m_ACE_Corpus);
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_ACE_Folder_For_sgm_Files_Filter(dlgRef.ACECorpusFolder, dlgRef.p_CParentDlg->m_ACE_Corpus);
		dlgRef.p_CParentDlg->XML_Parsing_Flag = false;
		dlgRef.p_CParentDlg->ACE_Coupus_Loaded_Flag = true;
	}
	if(!dlgRef.p_CParentDlg->XML_Cigaword_Parsing_Flag && dlgRef.Parsing_Gigaword_Flag){
		if(dlgRef.m_DebugDoc_Flag){
			dlgRef.GigawordFolder = Gigawrod_CORPUS_FOLDER;
		}
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_Gigaword_Content_Extractor(dlgRef.GigawordFolder.c_str(), dlgRef.p_CParentDlg->m_GigaDOC_v, "");
		dlgRef.p_CParentDlg->XML_Cigaword_Parsing_Flag = true;
	}
	if(dlgRef.Generating_Doc_Matrix_Flag){
		dlgRef.m_CDOC.Generating_Document_Matrix_Port(dlgRef.m_DocMatrix.c_str(), dlgRef.p_CParentDlg->m_GigaDOC_v);
	}
	if(dlgRef.Filtering_Term_Flag){
		dlgRef.m_CDOC.Filtering_DocMatrix_to_Terms_Port(dlgRef.m_DocMatrix.c_str());
	}
	if(dlgRef.LDA_Training_Flag){
		dlgRef.m_CDOC.LDA_Training_Port(dlgRef.m_DocMatrix.c_str());
	}
	if(dlgRef.Documents_Clustering_Flag ){
		dlgRef.m_CDOC.Documents_Clustering_to_Event_Port(dlgRef.m_DocMatrix.c_str());
	}
	if(dlgRef.Document_Event_Organizing_Flag){
		dlgRef.m_CDOC.Document_Events_Organizing_Port();
	}
	
	dlgRef.Enable_Usable_Button_CDOC();
	dlgRef.CDOC_Busy_Flag = false;
	dlgRef.Generating_Doc_Matrix_Flag = false;
	dlgRef.LDA_Training_Flag = false;
	dlgRef.Reckon_By_Time(false);
	dlgRef.Filtering_Term_Flag = false;
	dlgRef.Documents_Clustering_Flag = false;
	dlgRef.p_CParentDlg->System_Busy_Flag = false;
	dlgRef.Document_Event_Organizing_Flag = false;
	ExitThread(0);
}

void CDOC_Dlg::Enable_Usable_Button_CDOC()
{
	Disable_Button_CDOC();
	if(!p_CParentDlg->XML_Parsing_Flag && m_ActiveACE_Flag){
		m_ParsingACEButton.EnableWindow(TRUE);
	}
	if(p_CParentDlg->m_GigaDOC_v.empty() && m_ActiveGigaWord_Flag){
		m_ParsingGigaButton.EnableWindow(TRUE);
	}
	if(!p_CParentDlg->m_GigaDOC_v.empty()){
		m_DocMatrixButton.EnableWindow(TRUE);
	}
	string FileName = m_DocMatrix;
	FileName += ".DAT";

	if(NLPOP::Exist_of_This_File(FileName.c_str())){
		m_FilteringTermButton.EnableWindow(TRUE);
	}
	FileName = m_DocMatrix;
	FileName += ".DATFILT";
	if(NLPOP::Exist_of_This_File(FileName.c_str())){
		m_LDAButton.EnableWindow(TRUE);
	}
	if(	m_AcitveOrg_Flag && NLPOP::FolderExist(NLPOP::string2CString(ACECorpusFolder))){
		m_EventOrganizingButton.EnableWindow(TRUE);
	}
	FileName = m_DocMatrix.substr(0, m_DocMatrix.rfind('\\')+1);;
	FileName += "Root\\model-final.phi";
	if(NLPOP::Exist_of_This_File(FileName.c_str())){
		m_EventClusteringButton.EnableWindow(TRUE);
	}

}
void CDOC_Dlg::Disable_Button_CDOC()
{
	m_ParsingGigaButton.EnableWindow(FALSE);
	m_DocMatrixButton.EnableWindow(FALSE);
	m_ParsingACEButton.EnableWindow(FALSE);
	m_LDAButton.EnableWindow(FALSE);
	m_FilteringTermButton.EnableWindow(FALSE);
	m_EventOrganizingButton.EnableWindow(FALSE);
	m_EventClusteringButton.EnableWindow(FALSE);
}

void CDOC_Dlg::Updata_CDOC_Configurations()
{
	UpdateData(TRUE);
	ACECorpusFolder = p_CParentDlg->ACECorpusFolder;
	GigawordFolder = p_CParentDlg->GigawordFolder;
	m_CDOC.LCWCC_Flag = LCWCC_Flag;
	m_CDOC.ACE_ENTITY_Flag = ACE_ENTITY_Flag;
	m_CDOC.ICTCLAS_Flag = ICTCLAS_Flag;
	m_CDOC.m_HighRatio = m_HighRatio;
	m_CDOC.m_LowRatio = m_LowRatio;
	m_CDOC.m_LowFreq = m_LowFreq;
	m_CDOC.m_DebugDocNum = m_DebugDocNum;
	m_CDOC.m_DebugDoc_Flag = m_DebugDoc_Flag;
	m_CDOC.m_Events = m_Events;
	m_CDOC.m_alpha = m_alpha;
	m_CDOC.m_beta = m_beta;
	m_CDOC.m_twords = m_twords;
	m_CDOC.m_niters = m_niters;
	m_CDOC.m_Hierarchies = m_Hierarchies;
	m_CDOC.m_TimeStep = m_TimeStep;
	m_CDOC.ACECorpusFolder = ACECorpusFolder;
	m_CDOC.GigawordFolder = GigawordFolder;
}


void CDOC_Dlg::OnBnClickedCdocSparsing()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Parsing Gigaword\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;

	int rtn = AfxMessageBox(_T("All Documents will be deleted in \"CDOC\" Folder.\nAre your sure?"), MB_YESNO);
	if(rtn != IDYES){
		return;
	}
	NLPOP::Delete_Files_In_Dir(_T(CDOC_FOLDER));

	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"Parsing Gigaword\" is started ...", 1);

	Reckon_By_Time(true);
	Parsing_Gigaword_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CDOC_Dlg::OnBnClickedCdocMatrix()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Generating Document Matrix\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;
	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"Generating Document Matrix\" is started ...", 1);

	Reckon_By_Time(true);
	Generating_Doc_Matrix_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CDOC_Dlg::OnBnClickedDocParseAce()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Parsing ACE Corpus\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;
	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"Parsing ACE Corpus\" is started ...", 1);

	Reckon_By_Time(true);
	Parsing_ACE_Corpus_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}
void CDOC_Dlg::Reckon_By_Time(bool START_Flag)
{
	static bool Time_Started_Flag = false;
	if(START_Flag && !Time_Started_Flag){
		AppCall::Maxen_Responce_Message("====================Begin of New Command timekeeping\n");
		StartTime = CTime::GetCurrentTime();
		Time_Started_Flag = true;
	}
	else if(Time_Started_Flag){
		ostringstream ostrsteam;
		ostrsteam.str("");
		EndTime = CTime::GetCurrentTime();
		CTimeSpan ts = EndTime - StartTime;
		ostrsteam << "开始时间：" << StartTime.GetHour() << ":" << StartTime.GetMinute() << ":" << StartTime.GetSecond() << endl;
		ostrsteam << "结束时间：" << EndTime.GetHour() << ":" << EndTime.GetMinute() << ":" << EndTime.GetSecond() << endl;
		ostrsteam << "用时：" << ts.GetDays() << "天" << ts.GetHours() << "时" << ts.GetMinutes() <<  "分" << ts.GetSeconds() << "秒..." << endl;
		AppCall::Maxen_Responce_Message(ostrsteam.str().c_str());
		Time_Started_Flag = false;
		AppCall::Maxen_Responce_Message("====================End of New Command timekeeping\n");
	}
}
void CDOC_Dlg::OnBnClickedCdocLdaTraining()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"LDA Training\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;
	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"LDA Training\" is started ...", 1);

	Reckon_By_Time(true);
	LDA_Training_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CDOC_Dlg::OnBnClickedDocFilteringTerms()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Filtering Terms\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;

	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"Filtering Terms\" is started ...", 1);

	Reckon_By_Time(true);
	Filtering_Term_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CDOC_Dlg::OnBnClickedCdocActiveAce()
{
	UpdateData(TRUE);
	Enable_Usable_Button_CDOC();
}

void CDOC_Dlg::OnBnClickedCdocActiveGigaword()
{
	UpdateData(TRUE);
	Enable_Usable_Button_CDOC();
}

void CDOC_Dlg::OnBnClickedCdocDoAll()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Do All\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;

	int rtn = AfxMessageBox(_T("All Documents will be deleted in \"CDOC\" Folder.\nAre your sure?"), MB_YESNO);
	if(rtn != IDYES){
		return;
	}
	NLPOP::Delete_Files_In_Dir(_T(CDOC_FOLDER));

	Reckon_By_Time(true);
	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"Do All\" is started ...", 1);

	Parsing_Gigaword_Flag = true;
	Generating_Doc_Matrix_Flag = true;
	Filtering_Term_Flag = true;
	LDA_Training_Flag = true;
	Documents_Clustering_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}



void CDOC_Dlg::OnBnClickedCdocClustering()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"LDA Twords Visua\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;
	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"LDA Twords Visua\" is started ...", 1);
	
	Reckon_By_Time(true);
	Documents_Clustering_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CDOC_Dlg::OnBnClickedCdocActiveOrg()
{
	UpdateData(TRUE);
	Enable_Usable_Button_CDOC();
}
void CDOC_Dlg::OnBnClickedCheck1()
{
	UpdateData(TRUE);
	Enable_Usable_Button_CDOC();
}

void CDOC_Dlg::OnBnClickedCdocOrganizing()
{
	if(CDOC_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Document Event Organizing\" is not usable, please wait...", 1);
		return;
	}
	CDOC_Busy_Flag = true;
	Updata_CDOC_Configurations();
	p_CParentDlg->Output_MSG("\"Document Event Organizing\" is started ...", 1);
	
	Reckon_By_Time(true);
	Document_Event_Organizing_Flag = true;

	ImpThread = CreateThread(NULL, 0, CDOC_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

