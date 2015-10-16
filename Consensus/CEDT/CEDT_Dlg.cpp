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
// CEDT.cpp : implementation file
//

#include "stdafx.h"
#include "..\\Consensus.h"
#include "CEDT_Dlg.h"
#include "ConvertUTF.h"


DWORD WINAPI CEDT_Implement_Threaad(LPVOID pParam);
// CCEDT_Dlg dialog

IMPLEMENT_DYNAMIC(CCEDT_Dlg, CDialog)

CCEDT_Dlg::CCEDT_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCEDT_Dlg::IDD, pParent)
	, m_BoundIte(70)
	, m_CanditIte(90)
	, m_nGrossEval(5)
	, CEDT_Gen_Info_Flag(TRUE)
	, Gen_Words_Feature_Flag(TRUE)
	, Gen_Char_Feature_Flag(TRUE)
	, Gen_nGram_Feature_Flag(TRUE)
	, m_DetSTART_Flag(TRUE)
	, m_DetEND_Flag(TRUE)
	, NE_Detection_Flag(FALSE)
	, NE_Recogniation_Flag(TRUE)
	, NE_Extend_Flag(TRUE)
	, NE_Head_Flag(TRUE)
	, Auto_Load_Model_Flag(FALSE)
	, Forbid_Rewrite_Models_Flag(TRUE)
{
	CEDT_Busy_Flag = false;
	CEDT_Data_Floder = DATA_FOLDER;
	CEDT_Data_Floder += "CEDT\\";
	if(!NLPOP::FolderExist(NLPOP::string2CString(CEDT_Data_Floder))){
		_mkdir(CEDT_Data_Floder.c_str());
	}
	NE_Testing_Cases_Path = CEDT_Data_Floder + "Testing_Case.txt";
	NE_Training_Case_Path = CEDT_Data_Floder + "Training_Case.txt";
	NE_InfoPath = CEDT_Data_Floder + "Info.txt";
	
	ACE_Corpus_Training_and_Testing_Flag = false;
	Training_Models_by_Cases_Flag = false;
	Loading_Models_Flag = false;
	Documents_Recognization_Flag = false;
	Testing_Models_Flag = false;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	/*STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	CString SFile_path("C:\\Windows\\system32\\notepad.exe F:\\YPench\\CRF\\Testing.txt");
	TCHAR cmdline[1024];
	//_tcscpy_s(cmdline, 1024, SFile_path.GetBuffer());

	_tcscpy_s(cmdline, 1024, _T("F:\\YPench\\CRF\\crf_test.exe -m model Testing.txt"));

    //if(CreateProcess(NULL, _T("crf_test.exe -m model Testing.txt"),NULL,NULL,TRUE,NULL,NULL, _T("F:\\YPench\\CRF\\"),&s,&pi))  {
	if(CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, _T("F:\\YPench\\CRF\\"), &si,&pi))  { 
        WaitForSingleObject ( pi.hProcess ,INFINITE );  
        //等待进程执行完毕  
        CloseHandle ( pi.hProcess );  
        CloseHandle ( pi.hThread );  
        //关闭进程和主线程句柄  
    }  
   // CloseHandle ( hConsoleRedirect );
	DWORD nReturn = GetLastError();*/
}


CCEDT_Dlg::~CCEDT_Dlg()
{
}//IDC_CEDT_CANDIT_ITE

void CCEDT_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACE_TRAIN_TEST, m_ACETTButton);

	DDX_Text(pDX, IDC_CEDT_CANDIT_ITE, m_CanditIte);
	DDX_Text(pDX, IDC_CEDT_BOUND_ITE, m_BoundIte);
	DDX_Text(pDX, IDC_CEDT_N_GRAM, m_nGrossEval);


	DDX_Check(pDX, IDC_CEDT_GEN_INFO, CEDT_Gen_Info_Flag);
	DDX_Check(pDX, IDC_CHECK_START, m_DetSTART_Flag);
	DDX_Check(pDX, IDC_CHECK_END, m_DetEND_Flag);
	DDX_Check(pDX, IDC_RADIO_NE_DETECTION, NE_Detection_Flag);
	DDX_Check(pDX, IDC_NE_RECOGNITION_RADIO, NE_Recogniation_Flag);
	DDX_Check(pDX, IDC_CEDT_WORDS_F, Gen_Words_Feature_Flag);
	DDX_Check(pDX, IDC_CEDT_CHAR_CHECK, Gen_Char_Feature_Flag);
	DDX_Check(pDX, IDC_CEDT_N_GRAM_CHECK, Gen_nGram_Feature_Flag);
	DDX_Check(pDX, IDC_CEDT_HEAD, NE_Head_Flag);
	DDX_Check(pDX, IDC_CEDT_EXTEND, NE_Extend_Flag);
	DDX_Check(pDX, IDC_CEDT_AUTO_LOAD_MODEL, Auto_Load_Model_Flag);
	DDX_Check(pDX, IDC_CEDT_FORBID_MODEL, Forbid_Rewrite_Models_Flag);

	DDX_Control(pDX, IDC_CEDT_TRAININT_MODELS, m_TrainingModelsButton);
	DDX_Control(pDX, IDC_OPEN_RET_HISTORY, m_OHistoryB);
	DDX_Control(pDX, IDC_CEDT_SENT_INPUT, m_SINputEditCtrl);
	DDX_Control(pDX, IDC_CEDT_DOC_RECOG, m_DocRecogButton);
	DDX_Control(pDX, IDC_CEDT_LOAD_MODELS, m_LoadModelButton);
	DDX_Control(pDX, IDC_CEDT_SENT_DET, m_SentDetButton);
	DDX_Control(pDX, IDC_CEDT_SENT_RECOG, m_SentRecogButton);
	DDX_Control(pDX, IDC_CEDT_TESTING_MODEL, m_TestModelButton);
	DDX_Control(pDX, IDC_RICHEDIT22, m_TestFilePath);
	DDX_Control(pDX, IDC_RICHEDIT21, m_TrainFilePath);
	DDX_Control(pDX, IDC_CEDT_DOC_PATH, m_DocPathEdit);
}

BEGIN_MESSAGE_MAP(CCEDT_Dlg, CDialog)
	ON_BN_CLICKED(IDC_RESET_CEDT, &CCEDT_Dlg::OnBnClickedResetCEDT)
	ON_BN_CLICKED(IDC_ACE_TRAIN_TEST, &CCEDT_Dlg::OnBnClickedACETrainingAndTesting)
	ON_BN_CLICKED(IDC_CEDT_TRAININT_MODELS, &CCEDT_Dlg::OnBnClickedTrainingModelsButton)

	ON_BN_CLICKED(IDC_BUTTON_ANALYSIZE_BOUNDARY_RET, &CCEDT_Dlg::OnBnClickedButtonAnalysizeBoundaryRet)
	ON_BN_CLICKED(IDC_OPEN_RET_HISTORY, &CCEDT_Dlg::OnBnClickedOpenRetHistory)
	ON_BN_CLICKED(IDC_CEDT_DOC_RECOG, &CCEDT_Dlg::OnBnClickedCedtDocRecog)
	ON_BN_CLICKED(IDC_CEDT_LOAD_MODELS, &CCEDT_Dlg::OnBnClickedCedtLoadModels)
	ON_BN_CLICKED(IDC_CEDT_SENT_DET, &CCEDT_Dlg::OnBnClickedCedtSentDet)
	ON_BN_CLICKED(IDC_CEDT_SENT_RECOG, &CCEDT_Dlg::OnBnClickedCedtSentRecog)
	ON_BN_CLICKED(IDC_CEDT_TESTING_MODEL, &CCEDT_Dlg::OnBnClickedCedtTestingModel)
	ON_BN_CLICKED(IDC_CEDT_SET_TRAININT_PATH, &CCEDT_Dlg::OnBnClickedCedtSetTrainintPath)
	ON_BN_CLICKED(IDC_CEDT_SET_TEST_PATH, &CCEDT_Dlg::OnBnClickedCedtSetTestPath)
	ON_BN_CLICKED(IDC_CEDT_SET_RECOG_DOC, &CCEDT_Dlg::OnBnClickedCedtSetRecogDoc)
	ON_BN_CLICKED(IDC_CEDT_SENT_CLEAR, &CCEDT_Dlg::OnBnClickedCedtSentClear)
END_MESSAGE_MAP()


// CCEDT_Dlg message handlers
void CCEDT_Dlg::OnBnClickedResetCEDT()
{
	Disable_Button_CEDT();
	if(CEDT_Busy_Flag){
		GetExitCodeThread(ImpThread, &ImpphreadId);
		TerminateThread(ImpThread, ImpphreadId);
	}
	p_CParentDlg->m_ACE_Corpus.ACE_Relation_Info_v.clear();
	p_CParentDlg->m_ACE_Corpus.ACE_Entity_Info_map.clear();
	p_CParentDlg->m_ACE_Corpus.ACE_sgm_mmap.clear();

	p_CParentDlg->Output_MSG("CEDT Reset...", 1);
	
	p_CParentDlg->XML_Parsing_Flag = false;
	ACE_Corpus_Training_and_Testing_Flag = false;
	Training_Models_by_Cases_Flag = false;
	Loading_Models_Flag = false;
	Documents_Recognization_Flag = false;
	Testing_Models_Flag = false;

	CEDT_Busy_Flag = false;

	//NLPOP::Delete_Files_In_Dir(NLPOP::string2CString(CEDT_Data_Floder));
	Enable_Usable_Button_CEDT();
}

/*void CCEDT_Dlg::OnBnClickedAceCorpusParse()
{
	if(CEDT_Busy_Flag){
		p_CParentDlg->Output_MSG("\"XML Parse\" is not usable, please wait...", 1);
		return;
	}
	if(IDYES != AppCall::Secretary_Message_Box("Warning: This command may delete related file.\n Are you sure?", MB_YESNOCANCEL)){
			return;
	}
	Disable_Button_CEDT();
	CEDT_Busy_Flag = true;
	//NLPOP::Delete_Files_In_Dir(NLPOP::string2CString(CEDT_Data_Floder));
	Enable_Usable_Button_CEDT();
	p_CParentDlg->Output_MSG("\"XML Parse\" is started ...", 1);

	if(!p_CParentDlg->m_ACE_Corpus.ACE_Entity_Info_map.empty() || !p_CParentDlg->m_ACE_Corpus.ACE_Relation_Info_v.empty() || !p_CParentDlg->m_ACE_Corpus.ACE_sgm_mmap.empty()){
		p_CParentDlg->m_ACE_Corpus.ACE_Entity_Info_map.clear();
		p_CParentDlg->m_ACE_Corpus.ACE_Relation_Info_v.clear();
		p_CParentDlg->m_ACE_Corpus.ACE_sgm_mmap.clear();
		Enable_Usable_Button_CEDT();
	}
	if(!NLPOP::FolderExist(NLPOP::string2CString(p_CParentDlg->ACECorpusFolder))){
		string outstr = "Please put ACE corpus into fold: " + p_CParentDlg->ACECorpusFolder;
		p_CParentDlg->Output_MSG(outstr.c_str(), 1);
		return;
	}
	p_CParentDlg->XML_Parsing_Flag = true;
	ImpThread = CreateThread(NULL, 0, CEDT_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}*/
DWORD WINAPI CEDT_Implement_Threaad(LPVOID pParam)
{
	CCEDT_Dlg& dlgRef = *(CCEDT_Dlg*)pParam;
	if(dlgRef.p_CParentDlg->XML_Parsing_Flag){
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_ACE_Folder_Files_Filter(dlgRef.p_CParentDlg->ACECorpusFolder, dlgRef.p_CParentDlg->m_ACE_Corpus);
		dlgRef.p_CParentDlg->m_SXMLer.Xercesc_ACE_Folder_For_sgm_Files_Filter(dlgRef.p_CParentDlg->ACECorpusFolder, dlgRef.p_CParentDlg->m_ACE_Corpus);
		dlgRef.p_CParentDlg->ACE_Coupus_Loaded_Flag = true;
		dlgRef.p_CParentDlg->XML_Parsing_Flag = false;
		dlgRef.p_CParentDlg->Output_MSG("\"XML Parse\" is complete...", 1);
		dlgRef.m_ACETTButton.EnableWindow(TRUE);
	}
	if(dlgRef.ACE_Corpus_Training_and_Testing_Flag){
		dlgRef.m_CCEDT.ACE_Corpus_Training_and_Testing_Port("", dlgRef.p_CParentDlg->m_ACE_Corpus);
		dlgRef.p_CParentDlg->Output_MSG("\"Generating Named Entity Case\" is complete...", 1);
		dlgRef.m_TrainingModelsButton.EnableWindow(TRUE);
	}
	if(dlgRef.Training_Models_by_Cases_Flag){
		dlgRef.m_CCEDT.Training_Models_by_Cases(dlgRef.NE_Training_Case_Path.c_str());
		dlgRef.p_CParentDlg->Output_MSG("\"Training Models by Cases\" is complete...", 1);
	}
	if(dlgRef.Loading_Models_Flag){
		dlgRef.m_CCEDT.Loading_Named_Entity_Models();
		dlgRef.p_CParentDlg->Output_MSG("\"Loading Models\" is complete...", 1);
	}
	if(dlgRef.Documents_Recognization_Flag){
		dlgRef.m_CCEDT.Documents_Recognization(dlgRef.NE_Doc_Path.c_str());
		dlgRef.p_CParentDlg->Output_MSG("\"Documents Recognization\" is complete...", 1);
	}
	if(dlgRef.Testing_Models_Flag){
		dlgRef.m_CCEDT.Testing_Models_by_Cases(dlgRef.NE_Testing_Cases_Path.c_str());
		dlgRef.p_CParentDlg->Output_MSG("\"Testing Models\" is complete...", 1);
	}
	dlgRef.p_CParentDlg->XML_Parsing_Flag = false;
	dlgRef.ACE_Corpus_Training_and_Testing_Flag = false;
	dlgRef.Training_Models_by_Cases_Flag = false;
	dlgRef.Loading_Models_Flag = false;
	dlgRef.Documents_Recognization_Flag =false;
	dlgRef.Testing_Models_Flag = false;

	dlgRef.CEDT_Busy_Flag = false;
	dlgRef.Enable_Usable_Button_CEDT();

	//::SendMessage(dlgRef.p_CParentDlg->->m_hWnd, WM_ENET_MESSAGE, (WPARAM)0, 0);
	ExitThread(0);
}

void CCEDT_Dlg::Disable_Button_CEDT()
{
	m_ACETTButton.EnableWindow(FALSE);
	m_TrainingModelsButton.EnableWindow(FALSE);
}

void CCEDT_Dlg::Enable_Usable_Button_CEDT()
{
	if(NLPOP::FileExist(NLPOP::string2CString(p_CParentDlg->ACECorpusFolder+"apf.v5.1.1.dtd"))){
		m_ACETTButton.EnableWindow(TRUE);
	}
	else{
		m_ACETTButton.EnableWindow(FALSE);
	}
	if(NLPOP::FileExist(NLPOP::string2CString(NE_Training_Case_Path)) || NLPOP::FileExist(NLPOP::string2CString(p_CParentDlg->ACECorpusFolder+"apf.v5.1.1.dtd"))){
		m_TrainingModelsButton.EnableWindow(TRUE);
	}
	else{
		m_TrainingModelsButton.EnableWindow(FALSE);
	}
	if(NLPOP::FileExist(_T(CEDT_LOG_PATH))){
		m_OHistoryB.EnableWindow(TRUE);
	}
	else{
		m_OHistoryB.EnableWindow(FALSE);
	}
	if(NLPOP::FileExist(NLPOP::string2CString(m_CCEDT.m_Modelspace+"Head_Detec_Dism")) || NLPOP::FileExist(NLPOP::string2CString(m_CCEDT.m_Modelspace+"Head_Recog_Dism"))){
		m_LoadModelButton.EnableWindow(TRUE);
	}
	else{
		m_LoadModelButton.EnableWindow(FALSE);
	}
	if(m_CCEDT.Model_Loaded_Flag){
		m_DocRecogButton.EnableWindow(TRUE);
		m_SentDetButton.EnableWindow(TRUE);
		m_SentRecogButton.EnableWindow(TRUE);
	}
	else{
		m_DocRecogButton.EnableWindow(FALSE);
		m_SentDetButton.EnableWindow(FALSE);
		m_SentRecogButton.EnableWindow(FALSE);
	}
	if(NLPOP::FileExist(NLPOP::string2CString(NE_Testing_Cases_Path)) || m_CCEDT.Model_Loaded_Flag){
		m_TestModelButton.EnableWindow(TRUE);
	}
	else{
		m_TestModelButton.EnableWindow(FALSE);
	}
	m_TrainFilePath.SetWindowTextW(NLPOP::string2CString(NE_Training_Case_Path));
	m_TestFilePath.SetWindowTextW(NLPOP::string2CString(NE_Testing_Cases_Path));

}
void CCEDT_Dlg::Updata_CEDT_Configurations()
{
	m_CCEDT.CEDT_Gen_Info_Flag = CEDT_Gen_Info_Flag;
	m_CCEDT.m_DetSTART_Flag = m_DetSTART_Flag;
	m_CCEDT.m_DetEND_Flag = m_DetEND_Flag;
	m_CCEDT.NE_Detection_Flag = NE_Detection_Flag;
	m_CCEDT.NE_Recogniation_Flag = NE_Recogniation_Flag;
	m_CCEDT.NE_Extend_Flag = NE_Extend_Flag;
	m_CCEDT.NE_Head_Flag = NE_Head_Flag;
	m_CCEDT.Gen_Words_Feature_Flag = Gen_Words_Feature_Flag;
	m_CCEDT.Gen_Char_Feature_Flag = Gen_Char_Feature_Flag;
	m_CCEDT.Gen_nGram_Feature_Flag = Gen_nGram_Feature_Flag;
	m_CCEDT.Forbid_Rewrite_Models_Flag = Forbid_Rewrite_Models_Flag;
	m_CCEDT.Auto_Load_Model_Flag = Auto_Load_Model_Flag;

	m_CCEDT.m_CanditIte = m_CanditIte;
	m_CCEDT.m_BoundIte = m_BoundIte;
	m_CCEDT.m_nGrossEval = m_nGrossEval;
	
	
}

void CCEDT_Dlg::OnBnClickedACETrainingAndTesting()
{
	if(CEDT_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Generate Named Entity Case\" is not usable, please wait...", 1);
		return;
	}
	CEDT_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Generate Named Entity Case\" is started ...", 1);

	string DataFolder = DATA_FOLDER;
	if(!NLPOP::FolderExist(NLPOP::string2CString(DataFolder))){
		_mkdir(DATA_FOLDER);
	}
	ACE_Corpus_Training_and_Testing_Flag = true;
	//---------------
	if(p_CParentDlg->m_ACE_Corpus.ACE_Entity_Info_map.empty() && p_CParentDlg->m_ACE_Corpus.ACE_sgm_mmap.empty()){
		p_CParentDlg->XML_Parsing_Flag = true;
	}
	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	ImpThread = CreateThread(NULL, 0, CEDT_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CCEDT_Dlg::OnBnClickedTrainingModelsButton()
{
	if(NLPOP::FileExist(NLPOP::string2CString(p_CParentDlg->ACECorpusFolder+"apf.v5.1.1.dtd"))){
		UpdateData(TRUE);
		m_nGrossEval = 0;
		UpdateData(FALSE);
		p_CParentDlg->Output_MSG("ACE Data Corpus is Selected...", 1);
		OnBnClickedACETrainingAndTesting();
		return;
	}
	if(!NLPOP::FileExist(NLPOP::string2CString(NE_Training_Case_Path))){
		p_CParentDlg->Output_MSG("Training Data Cases is not Missing...", 1);
		return;
	}
	if(CEDT_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Training Models by Cases\" is not usable, please wait...", 1);
		return;
	}
	CEDT_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Training Models by Cases\" is started ...", 1);

	Training_Models_by_Cases_Flag = true;

	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	ImpThread = CreateThread(NULL, 0, CEDT_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}


void CCEDT_Dlg::OnBnClickedButtonAnalysizeBoundaryRet()
{
//	m_CCEDT.m_CDetBound.Detect_Boundary_Result_Analyser();
}

void CCEDT_Dlg::OnBnClickedOpenRetHistory()
{
	if(!NLPOP::FileExist(_T(CEDT_LOG_PATH))){
		AppCall::Secretary_Message_Box(CEDT_LOG_PATH, MB_OK);
	}
	AppCall::Consensus_Open_Process(CEDT_LOG_PATH, Notepad_Path);
}

void CCEDT_Dlg::OnBnClickedCedtDocRecog()
{
	if(CEDT_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Documents Recognization\" is not usable, please wait...", 1);
		return;
	}
	CEDT_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Documents Recognization\" is started ...", 1);

	Documents_Recognization_Flag = true;

	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	ImpThread = CreateThread(NULL, 0, CEDT_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CCEDT_Dlg::OnBnClickedCedtLoadModels()
{
	if(CEDT_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Loading Models\" is not usable, please wait...", 1);
		return;
	}
	CEDT_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Loading Models\" is started ...", 1);

	Loading_Models_Flag = true;

	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	ImpThread = CreateThread(NULL, 0, CEDT_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();

	m_SINputEditCtrl.SetWindowTextW(_T("第三届台北艺术节今天晚上在台北大安森林公园登场，台北市长 马英久和文化局长龙映台共同开锣接开序幕。"));
}

void CCEDT_Dlg::OnBnClickedCedtSentDet()
{
	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	CString CInputstr;
	m_SINputEditCtrl.GetWindowText(CInputstr);
	Catch_Input_Sentence = CInputstr;
	string ouputstr = m_CCEDT.Sentence_Named_Entity_Deteceion_Port(NLPOP::CString2string(CInputstr).c_str());
	CInputstr = NLPOP::string2CString(ouputstr);
	m_SINputEditCtrl.SetWindowTextW(CInputstr);
}

void CCEDT_Dlg::OnBnClickedCedtSentRecog()
{
	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	CString CInputstr;
	m_SINputEditCtrl.GetWindowText(CInputstr);
	Catch_Input_Sentence = CInputstr;
	string ouputstr = m_CCEDT.Sentence_Named_Entity_Recognization_Port(NLPOP::CString2string(CInputstr).c_str());
	CInputstr = NLPOP::string2CString(ouputstr);
	m_SINputEditCtrl.SetWindowTextW(CInputstr);
	
}

void CCEDT_Dlg::OnBnClickedCedtTestingModel()
{
	if(CEDT_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Testing Models\" is not usable, please wait...", 1);
		return;
	}
	CEDT_Busy_Flag = true;
	p_CParentDlg->Output_MSG("\"Testing Models\" is started ...", 1);

	Testing_Models_Flag = true;

	UpdateData(TRUE);
	Updata_CEDT_Configurations();

	ImpThread = CreateThread(NULL, 0, CEDT_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CCEDT_Dlg::OnBnClickedCedtSetTrainintPath()
{
	CString Fname;
	CString Fpath;
	CFileDialog OpenDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, _T(""), NULL);
	int nModal = OpenDialog.DoModal();
	if (nModal == IDOK){
		Fpath = OpenDialog.GetPathName();
		//Fpath = Fpath.Left(Fpath.ReverseFind('\\') + 1);
		Fname = OpenDialog.GetFileName();
		m_TrainFilePath.SetWindowTextW(Fpath);
		NE_Training_Case_Path = NLPOP::CString2string(Fpath);
		string outstr = "\"" + NLPOP::CString2string(Fname);
		outstr += "\" is Selected...";
		p_CParentDlg->Output_MSG(outstr.c_str(), 1);
	}
}

void CCEDT_Dlg::OnBnClickedCedtSetTestPath()
{
	CString Fname;
	CString Fpath;
	CFileDialog OpenDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, _T(""), NULL);
	int nModal = OpenDialog.DoModal();
	if (nModal == IDOK){
		Fpath = OpenDialog.GetPathName();
		//Fpath = Fpath.Left(Fpath.ReverseFind('\\') + 1);
		Fname = OpenDialog.GetFileName();
		m_TestFilePath.SetWindowTextW(Fpath);
		NE_Testing_Cases_Path = NLPOP::CString2string(Fpath);
		string outstr = "\"" + NLPOP::CString2string(Fname);
		outstr += "\" is Selected...";
		p_CParentDlg->Output_MSG(outstr.c_str(), 1);
	}
}

void CCEDT_Dlg::OnBnClickedCedtSetRecogDoc()
{
	CString Fname;
	CString Fpath;
	CFileDialog OpenDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, _T(""), NULL);
	int nModal = OpenDialog.DoModal();
	if (nModal == IDOK){
		Fpath = OpenDialog.GetPathName();
		//Fpath = Fpath.Left(Fpath.ReverseFind('\\') + 1);
		Fname = OpenDialog.GetFileName();
		m_DocPathEdit.SetWindowTextW(Fpath);
		NE_Doc_Path = NLPOP::CString2string(Fpath);
		string outstr = "\"" + NLPOP::CString2string(Fname);
		outstr += "\" is Selected...";
		p_CParentDlg->Output_MSG(outstr.c_str(), 1);
	}
}

void CCEDT_Dlg::OnBnClickedCedtSentClear()
{
	m_SINputEditCtrl.SetWindowTextW(Catch_Input_Sentence);
}
