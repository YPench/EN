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
// ENET_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\\Consensus.h"
#include "ENET_Dlg.h"
#include "ENCom.h"

// CENET_Dlg dialog

IMPLEMENT_DYNAMIC(CENET_Dlg, CDialog)

CENET_Dlg::CENET_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CENET_Dlg::IDD, pParent)
	, m_PersonCheck(TRUE)
	, m_LocationCheck(TRUE)
	, m_OrgCheck(TRUE)
	, m_RelTYPECheck(TRUE)
	, m_RelSubtypeCheck(FALSE)
	, m_SP_Check(TRUE)
	, m_NamedEntityWeight(0.1)
	, m_RelationWeight(0.1)
	, m_MaxVexNum(500)
	, m_Filter_PER_Flag(TRUE)
	, m_Filter_LOC_Flag(TRUE)
	, m_Filter_ORG_Flag(TRUE)
	, m_Filter_PER_SOC_Flag(TRUE)
	, m_Filter_GEN_AFF_Flag(TRUE)
	, m_Filter_ORG_AFF_Flag(TRUE)
	, m_Filter_PHYS_Flag(TRUE)
	, m_Filter_PART_WHOLE_Flag(TRUE)
{
	Opened_CDENer_Dlg_Flag = false;
	CENET_Busy_Flag = false;
	Running_Named_Entity_Flag = false;
	Running_Relation_Flag = false;
	Running_Document_Event_Flag = false;
	Collected_Doc_Event_Flag = false;
	Generating_Entity_Flag = false;
	Generating_Relation_Flag = false;
	Filtering_Vertex_and_Edges_Flag = false;
	Entity_Star_Layout_Flag = false;
	Short_Path_Flag = false;
	Loading_EN_Info_Flag = false;
	Collecting_Info_Flag = false;
	Deleting_Events_Organizing_Files_Flag = false;
	PLT_Analysis_Flag = false;
	Action_Analysis_Flag = false;
	m_Menu.LoadMenu(IDR_ANAL_ENABLE_BUTTON);
	ImpThread = NULL;
	m_CENET.p_m_CENET_Dlg = this;

	if(!NLPOP::FolderExist(_T(EVENT_ORGANIZATION_FOLDER))){
		_mkdir(EVENT_ORGANIZATION_FOLDER);
	}
	if(!NLPOP::FolderExist(_T(ENET_FOLDER))){
		_mkdir(ENET_FOLDER);
	}
}




CENET_Dlg::~CENET_Dlg()
{
	
}

void CENET_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ANAL_PERSON, m_PersonCheck);
	DDX_Check(pDX, IDC_ANAL_LOCATION, m_LocationCheck);
	DDX_Check(pDX, IDC_ANAL_ORG, m_OrgCheck);
	DDX_Control(pDX, IDC_ANAL_PICTURE, m_AnalPicture);
	DDX_Control(pDX, IDC_ANAL_ENTITY_RUN, m_RunEntityButton);
	DDX_Control(pDX, IDC_ANAL_DEWER_RUN, m_DocRunButton);
	DDX_Control(pDX, IDC_ANAL_REL_RUN, m_RunRelationButton);
	DDX_Control(pDX, IDC_ANAL_RUN_ALL, m_RunAll);
	DDX_Control(pDX, IDC_ANA_EVENT_NAME, m_EventNameEdit);
	DDX_Control(pDX, IDC_ANAL_KEY_WORDS, m_KeyWordsEdit);
	DDX_Control(pDX, IDC_ANAL_ENTITY_RESULTS, m_EntityResultButton);
	DDX_Control(pDX, IDC_ANAL_ENTITY_VIEWER, m_EntityReviewButton);
	DDX_Control(pDX, IDC_ANAL_REL_RESULTS, m_RelResultButton);
	DDX_Control(pDX, IDC_ANAL_REL_VIEWER, m_RelReviewButton);
	DDX_Control(pDX, IDC_ANAL_GEN_EN, m_GenViewENButton);
	DDX_Check(pDX, IDC_ENET_SP_CHECK, m_SP_Check);
	DDX_Control(pDX, IDC_ENET_CENT_COMBO, m_CentboBox);
	DDX_Control(pDX, IDC_ENET_SPA1_COMBO, m_SPArg1boBox);
	DDX_Control(pDX, IDC_ENET_SPA2_COMBO, m_SPArg2boBox);
	DDX_Control(pDX, IDC_ENET_BOTTON_EDIT, m_BottonEdit);
	DDX_Text(pDX, IDC_ENET_NE_WEIGHT, m_NamedEntityWeight);
	DDX_Text(pDX, IDC_ENET_REL_WEITHG, m_RelationWeight);
	DDX_Text(pDX, IDC_ENET_MAX_VEX, m_MaxVexNum);
	DDX_Control(pDX, IDC_ENET_LOAD_INFO, m_LoadInfoButton);
	DDX_Control(pDX, IDC_ANAL_RESET, m_ENETResetButton);
	DDX_Check(pDX, IDC_ANAL_REL_TYPE, m_RelTYPECheck);
	DDX_Check(pDX, IDC_ANAL_REL_SUBTYPE, m_RelSubtypeCheck);

	DDX_Check(pDX, IDC_ENET_FILTER_PER, m_Filter_PER_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_LOC, m_Filter_LOC_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_ORG, m_Filter_ORG_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_PER_SOC, m_Filter_PER_SOC_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_GEN_AFF, m_Filter_GEN_AFF_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_ORG_AFF, m_Filter_ORG_AFF_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_PHYS, m_Filter_PHYS_Flag);
	DDX_Check(pDX, IDC_ENET_FILTER_PART_WHOLE, m_Filter_PART_WHOLE_Flag);
}

BEGIN_MESSAGE_MAP(CENET_Dlg, CDialog)
	ON_BN_CLICKED(IDC_ANAL_DEWER, &CENET_Dlg::OnBnClickedAnalDewer)
	ON_BN_CLICKED(IDC_ANAL_DEWER_RUN, &CENET_Dlg::OnBnClickedENETDocEventOrg)
	ON_BN_CLICKED(IDC_ANAL_ENTITY_RUN, &CENET_Dlg::OnBnClickedAnalEntityRun)
	ON_BN_CLICKED(IDC_ANAL_REL_RUN, &CENET_Dlg::OnBnClickedAnalRelRun)
	ON_BN_CLICKED(IDC_ANAL_RUN_ALL, &CENET_Dlg::OnBnClickedAnalRunAll)
	ON_BN_CLICKED(IDC_ANAL_GEN_EN, &CENET_Dlg::OnBnClickedAnalGenEn)
	ON_BN_CLICKED(IDC_ANAL_ENTITY_RESULTS, &CENET_Dlg::OnBnClickedAnalEntityGen)
	ON_BN_CLICKED(IDC_ANAL_REL_RESULTS, &CENET_Dlg::OnBnClickedAnalRelGen)
	ON_BN_CLICKED(IDC_ANAL_ENTITY_VIEWER, &CENET_Dlg::OnBnClickedAnalEntityViewer)
	ON_BN_CLICKED(IDC_ANAL_REL_VIEWER, &CENET_Dlg::OnBnClickedAnalRelViewer)
	ON_BN_CLICKED(IDC_ANAL_EXAMPLE, &CENET_Dlg::OnBnClickedAnalExample)
	ON_MESSAGE(WM_ENET_MESSAGE, ENET_Message_Processing)
	ON_BN_CLICKED(IDC_ANAL_RESET, &CENET_Dlg::OnBnClickedAnalReset)
	ON_BN_CLICKED(IDC_ENET_SP_CHECK, &CENET_Dlg::OnBnClickedEnetSpCheck)
END_MESSAGE_MAP()


// CENET_Dlg message handlers
LRESULT CENET_Dlg::ENET_Message_Processing( WPARAM wParam, LPARAM lParam )
{
	UpdateData(FALSE);
	Enable_Usable_Button_ENET();
	//string Collected_EventName;
	//string Collected_EventPath;
	//string Collected_KeyWords;
	m_EventNameEdit.SetWindowTextW(NLPOP::string2CString(Collected_EventName));
	m_KeyWordsEdit.SetWindowTextW(NLPOP::string2CString(Collected_KeyWords));
	OnPaint();
	return 1;
}

BOOL CENET_Dlg::PreTranslateMessage(MSG* pMsg)
{
	if(WM_KEYDOWN == pMsg->message ){   //keyboard buttondown
		UINT nKey = (int) pMsg->wParam;
		if( VK_RETURN == nKey || VK_ESCAPE == nKey ){
			return TRUE ; 
		}
	}
	else if(WM_RBUTTONDOWN == pMsg->message) //mouse right buttondown
	{ 
		CWnd* pWnd = (CWnd*) GetDlgItem(IDD_ANALYZER);   
        if(pWnd == GetFocus()) 
			{//richedit上发生了right   button   down
			//-----------------------copied codes
			LPPOINT lpoint=new tagPOINT; //标示menu显示位置
			::GetCursorPos(lpoint);//得到鼠标位置

			SetForegroundWindow();//本行代码作用：如果用户未选择子菜单则自动隐藏该menu
			m_Menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTALIGN,lpoint->x,lpoint->y,this); //显示menu
			PostMessage(WM_NULL,0,0);//
			delete lpoint; //回收资源
		}
		Enable_Usable_Button_ENET();
		return TRUE ; 
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CENET_Dlg::OnBnClickedAnalDewer()
{
	Enable_Usable_Button_ENET();
	if(Opened_CDENer_Dlg_Flag){
		p_m_CDENer_Dlg->ShowWindow(SW_SHOW);
		return;
	}
	p_m_CDENer_Dlg = new CDENer_Dlg;
	p_m_CDENer_Dlg->Create(IDD_DOC_EVENT_REVIEWER, NULL);
	Opened_CDENer_Dlg_Flag = true;
	p_m_CDENer_Dlg->Init_Document_Event_Viewer(GigawordFolder.c_str());
	p_m_CDENer_Dlg->p_CParentDlg = this;
}

void CENET_Dlg::OnCancel()
{
	CDialog::OnCancel();
}




DWORD WINAPI ENET_Implement_Threaad(LPVOID pParam)
{
	CENET_Dlg& dlgRef = *(CENET_Dlg*)pParam;
	dlgRef.p_CParentDlg->System_Busy_Flag = true;
	dlgRef.m_ENETResetButton.EnableWindow(TRUE);
	if(dlgRef.Running_Document_Event_Flag){
		CDOC* p_loc_CDOC = &dlgRef.p_CParentDlg->m_CDOC_Dlg.m_CDOC;
		p_loc_CDOC->m_DebugDoc_Flag = 0;
		p_loc_CDOC->GigawordFolder = dlgRef.p_CParentDlg->GigawordFolder;
		p_loc_CDOC->Document_Events_Organizing_Port();
	}
	if(dlgRef.Collecting_Info_Flag){
		dlgRef.m_CENET.Collecting_Events_Organizing_Info();
	}
	if(dlgRef.Running_Named_Entity_Flag){
		CCEDT* p_loc_CCEDT = &dlgRef.p_CParentDlg->m_CCEDT_Dlg.m_CCEDT;
		dlgRef.m_CENET.ENET_Incremental_Flag = true;
		dlgRef.m_CENET.Run_Named_Entity_Port(dlgRef.GigawordFolder.c_str(), p_loc_CCEDT);
	}
	if(dlgRef.Running_Relation_Flag){
		CRDC* p_loc_CRDCT = &dlgRef.p_CParentDlg->m_CCRDC_Dlg.m_CRDC;
		dlgRef.m_CENET.ENET_Incremental_Flag = true;
		dlgRef.m_CENET.Run_Entity_Relation_Port(dlgRef.GigawordFolder.c_str(), p_loc_CRDCT);
	}
	//-----------------------------------------
	if(dlgRef.Generating_Entity_Flag){
		dlgRef.m_CENET.Generating_Entity_of_Event_Network_Port();
	}
	if(dlgRef.Generating_Relation_Flag){
		dlgRef.m_CENET.Generating_Relation_of_Event_Network_Port();
	}
	if(dlgRef.Filtering_Vertex_and_Edges_Flag){
		if(dlgRef.Entity_Star_Layout_Flag){
			dlgRef.m_CENET.Cellected_Entity_Star_Layout_Port();
		}
		else if(dlgRef.Short_Path_Flag){
			dlgRef.m_CENET.Cellected_Entity_Short_Path_Port();
		}
		else{
			dlgRef.m_CENET.Generating_and_Viewing_Event_Network_Port();
		}
	}
	if(dlgRef.Loading_EN_Info_Flag){
		dlgRef.m_CENET.Generating_and_Viewing_Event_Network_Port();
	}
	if(dlgRef.Deleting_Events_Organizing_Files_Flag){
		dlgRef.m_CENET.Deleting_Events_Organizing_Files(dlgRef.DeletingFileProx.c_str());
	}
	if(dlgRef.PLT_Analysis_Flag){
		dlgRef.m_CENET.Person_Location_Time_Analysis_Port();
	}
	if(dlgRef.Action_Analysis_Flag){
		dlgRef.ACECorpusFolder = dlgRef.p_CParentDlg->ACECorpusFolder;
		dlgRef.m_CENET.m_CAct.Action_Analysis_Port((CENET_Dlg*)pParam);
	}

	dlgRef.m_ENETResetButton.EnableWindow(FALSE);
	dlgRef.Running_Document_Event_Flag = false;
	dlgRef.Running_Named_Entity_Flag = false;
	dlgRef.Running_Relation_Flag = false;
	dlgRef.Generating_Entity_Flag = false;
	dlgRef.Generating_Relation_Flag = false;
	dlgRef.Filtering_Vertex_and_Edges_Flag = false;
	dlgRef.Loading_EN_Info_Flag = false;
	dlgRef.Collecting_Info_Flag = false;
	dlgRef.Deleting_Events_Organizing_Files_Flag = false;
	dlgRef.PLT_Analysis_Flag = false;
	dlgRef.Action_Analysis_Flag = false;

	dlgRef.Reckon_By_Time(false);
	dlgRef.p_CParentDlg->System_Busy_Flag = false;
	dlgRef.CENET_Busy_Flag = false;
	dlgRef.ImpThread = NULL;

	::SendMessage(dlgRef.m_hWnd, WM_ENET_MESSAGE, (WPARAM)0, 0);
	ExitThread(0);
}
void CENET_Dlg::Reckon_By_Time(bool START_Flag)
{
	static bool Time_Started_Flag = false;
	if(START_Flag && !Time_Started_Flag){
		AppCall::Maxen_Responce_Message("====================Begin of New Command Timekeeping\n");
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
		AppCall::Maxen_Responce_Message("====================End of New Command Timekeeping\n");
	}
}
void CENET_Dlg::Updata_ENET_Configurations()
{
	UpdateData(TRUE);
	if(m_PersonCheck){
		m_NE_TYPE_s.insert("PER");
	}
	if(m_LocationCheck){
		m_NE_TYPE_s.insert("LOC");
	}
	if(m_OrgCheck){
		m_NE_TYPE_s.insert("ORG");
	}
	m_REL_TYPE_s.insert("PER-SOC");
	m_REL_TYPE_s.insert("GEN-AFF");
	m_REL_TYPE_s.insert("ORG-AFF");
	m_REL_TYPE_s.insert("PART-WHOLE");
	m_REL_TYPE_s.insert("PHYS");

	m_FilterEntity_s.clear();
	m_FilterRelation_s.clear();
	if(m_Filter_PER_Flag){
		m_FilterEntity_s.insert("PER");
	}
	if(m_Filter_LOC_Flag){
		m_FilterEntity_s.insert("LOC");
	}
	if(m_Filter_ORG_Flag){
		m_FilterEntity_s.insert("ORG");
	}
	if(m_Filter_PER_SOC_Flag){
		m_FilterRelation_s.insert("PER-SOC");
	}
	if(m_Filter_GEN_AFF_Flag){
		m_FilterRelation_s.insert("GEN-AFF");
	}
	if(m_Filter_ORG_AFF_Flag){
		m_FilterRelation_s.insert("ORG-AFF");
	}
	if(m_Filter_PART_WHOLE_Flag){
		m_FilterRelation_s.insert("PART-WHOLE");
	}
	if(m_Filter_PHYS_Flag){
		m_FilterRelation_s.insert("PHYS");
	}
	m_CENET.Collected_Doc_Event_Flag = Collected_Doc_Event_Flag;
	m_CENET.Collected_EventName = Collected_EventName;
	m_CENET.Collected_EventPath = Collected_EventPath;
	m_CENET.Collected_KeyWords = Collected_KeyWords;
	m_CENET.Collected_FilePrix = Collected_FilePrix;

//-----------------------------------
	UpdateData(0);
	if(Collected_Doc_Event_Flag){
		CString CboBoxText;

		//-----------center filtering
		if(CB_ERR == m_CentboBox.GetCurSel()){
			m_CentboBox.AddString(_T("Null"));
			m_CentboBox.SetCurSel(0);
		}
		if(m_SP_Check){
			m_CentboBox.GetLBText(m_CentboBox.GetCurSel(),CboBoxText);
			if(CboBoxText != _T("Null")){
				Entity_Star_Layout_Flag = true;
			}
			else{
				Entity_Star_Layout_Flag = false;
			}
		}
		//-----------Short Path Filtering
		if(CB_ERR == m_SPArg1boBox.GetCurSel()){
			m_SPArg1boBox.AddString(_T("Null"));
			m_SPArg1boBox.SetCurSel(0);
		}
		if(CB_ERR == m_SPArg2boBox.GetCurSel()){
			m_SPArg2boBox.AddString(_T("Null"));
			m_SPArg2boBox.SetCurSel(0);
		}
		if(!m_SP_Check){
			CString CArg1, CArg2;
			m_SPArg1boBox.GetLBText(m_SPArg1boBox.GetCurSel(),CArg1);
			m_SPArg2boBox.GetLBText(m_SPArg1boBox.GetCurSel(),CArg2);
			if(CArg1 != _T("Null") && CArg2 != _T("Null")){
				Short_Path_Flag = true;
			}
			else{
				Short_Path_Flag = false;
			}
		}
	}
}


void CENET_Dlg::Disable_Button_ENET()
{
	m_DocRunButton.EnableWindow(FALSE);
	m_RunEntityButton.EnableWindow(FALSE);
	m_RunRelationButton.EnableWindow(FALSE);
	m_RunAll.EnableWindow(FALSE);
	m_EntityResultButton.EnableWindow(FALSE);
	m_EntityReviewButton.EnableWindow(FALSE);
	m_RelResultButton.EnableWindow(FALSE);
	m_RelReviewButton.EnableWindow(FALSE);
	m_GenViewENButton.EnableWindow(FALSE);
	m_LoadInfoButton.EnableWindow(FALSE);

	m_CentboBox.EnableWindow(FALSE);
	m_SPArg1boBox.EnableWindow(FALSE);
	m_SPArg2boBox.EnableWindow(FALSE);
	m_ENETResetButton.EnableWindow(FALSE);
}
void CENET_Dlg::Enable_Usable_Button_ENET()
{
	Updata_ENET_Configurations();
	Disable_Button_ENET();

	string FileName;
	if(p_CParentDlg->m_CDOC_Dlg.m_EventOrganizingButton.IsWindowEnabled()){
		m_DocRunButton.EnableWindow(TRUE);
	}

	if(p_CParentDlg->m_CCEDT_Dlg.m_LoadModelButton.IsWindowEnabled()){
		m_RunEntityButton.EnableWindow(TRUE);
	}
	if(p_CParentDlg->m_CCRDC_Dlg.m_LoadModelButton.IsWindowEnabled()){
		m_RunRelationButton.EnableWindow(TRUE);
	}
	if(m_DocRunButton.IsWindowEnabled() && m_RunEntityButton.IsWindowEnabled() && m_RunRelationButton.IsWindowEnabled()){
		m_RunAll.EnableWindow(TRUE);
	}

	if(Collected_Doc_Event_Flag){
		FileName = Collected_FilePrix;
		FileName += ".ENTITY";
		if(NLPOP::Exist_of_This_File(FileName)){
			m_EntityResultButton.EnableWindow(TRUE);
		}
		FileName = Collected_FilePrix;
		FileName += "_entity.paj";
		if(NLPOP::Exist_of_This_File(FileName)){
			m_EntityReviewButton.EnableWindow(TRUE);
		}
		
		FileName = Collected_FilePrix;
		FileName += ".RELATION";
		if(NLPOP::Exist_of_This_File(FileName)){
			m_RelResultButton.EnableWindow(TRUE);
		}
		FileName = Collected_FilePrix;
		FileName += "_relation.paj";
		if(NLPOP::Exist_of_This_File(FileName)){
			m_RelReviewButton.EnableWindow(TRUE);
		}
		if(m_EntityReviewButton.IsWindowEnabled() && m_RelReviewButton.IsWindowEnabled()){
			m_GenViewENButton.EnableWindow(TRUE);
		}
		FileName = Collected_FilePrix;
		FileName += ".paj";
		if(NLPOP::Exist_of_This_File(FileName)){
			m_LoadInfoButton.EnableWindow(TRUE);
		}
		if(!m_SP_Check && m_GenViewENButton.IsWindowEnabled() && m_SPArg1boBox.GetCount() > 1){
			m_SPArg1boBox.EnableWindow(TRUE);
		}
		if(!m_SP_Check && m_GenViewENButton.IsWindowEnabled() && m_SPArg2boBox.GetCount() > 1){
			m_SPArg2boBox.EnableWindow(TRUE);
		}
		if(m_SP_Check && m_GenViewENButton.IsWindowEnabled() && m_CentboBox.GetCount() > 1){
			m_CentboBox.EnableWindow(TRUE);
		}
	}

}
void CENET_Dlg::OnBnClickedENETDocEventOrg()
{
	int rtn = AfxMessageBox(_T("Incremental Model will be Used, Are your sure?"), MB_YESNOCANCEL);
	if(rtn == IDCANCEL){
		return;
	}
	if(rtn == IDYES){
		p_CParentDlg->m_CDOC_Dlg.m_CDOC.CDOC_Incremental_Flag = true;
	}
	else{
		p_CParentDlg->m_CDOC_Dlg.m_CDOC.CDOC_Incremental_Flag = false;
	}

	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Run Named Entity Extracting\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;

	if(!p_CParentDlg->m_CDOC_Dlg.m_CDOC.CDOC_Incremental_Flag){
		rtn = AfxMessageBox(_T("Warning: All File in \"Events Organizing\" Will Be Erased?, Are You Sure?"), MB_YESNO);
		if(rtn == IDYES){
			NLPOP::Delete_Files_In_Dir(_T(EVENT_ORGANIZATION_FOLDER));
		}
		else{
			CENET_Busy_Flag = false;
			return;
		}
	}
	p_CParentDlg->m_CDOC_Dlg.Updata_CDOC_Configurations();

	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Run Named Entity Extracting\" is started ...", 1);

	Running_Document_Event_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
	
}
void CENET_Dlg::OnBnClickedAnalEntityRun()
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Run Named Entity Extracting\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;

	if(Collected_Doc_Event_Flag){
		int rtn = AfxMessageBox(_T("This commond will erase related documents, Are your sure?"), MB_YESNO);
			if(rtn != IDYES){
				return;
			}
		string FileName = Collected_FilePrix;
		FileName += ".ENTITY";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
		FileName = Collected_FilePrix;
		FileName += "_entity.paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
		FileName = Collected_FilePrix;
		FileName += ".RELATION";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
		FileName = Collected_FilePrix;
		FileName += "_relation.paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
		FileName = Collected_FilePrix;
		FileName += ".paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
	}

	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Run Named Entity Extracting\" is started ...", 1);

	Running_Named_Entity_Flag = true;


	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	Enable_Usable_Button_ENET();
	OnPaint();
}


void CENET_Dlg::OnBnClickedAnalRelRun()
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Running Relation Extracting\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Running Relation Extracting\" is started ...", 1);

	Running_Relation_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	Enable_Usable_Button_ENET();
	OnPaint();
}



void CENET_Dlg::OnBnClickedAnalRunAll()
{
	//int rtn = AfxMessageBox(_T("All Generated Documents will be erased, Are your sure?"), MB_YESNO);
	//if(rtn != IDYES){
	//	return;
	//}
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Run All\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Run All\" is started ...", 1);

	p_CParentDlg->m_CDOC_Dlg.Updata_CDOC_Configurations();

	// In order to avoid misoperation, this command is annotated;
	/*int rtn = AfxMessageBox(_T("Warning: All File in \"Events Organizing\" Will Be Erased?, Are Sure?"), MB_YESNO);
	if(rtn == IDYES){
		NLPOP::Delete_Files_In_Dir(_T(EVENT_ORGANIZATION_FOLDER));
		Running_Document_Event_Flag = true;
	}*/
	
	Running_Named_Entity_Flag = true;
	Running_Relation_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	Enable_Usable_Button_ENET();
	OnPaint();
}

void CENET_Dlg::OnBnClickedAnalGenEn()
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Running Relation Extracting\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Running Relation Extracting\" is started ...", 1);

	Filtering_Vertex_and_Edges_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	Enable_Usable_Button_ENET();
	OnPaint();
}


void CENET_Dlg::OnBnClickedAnalEntityGen()
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Review Entities Extracting Result\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Review Entities Extracting Result\" is started ...", 1);

	if(Collected_Doc_Event_Flag){
		string FileName = Collected_FilePrix;
		FileName += "_entity.paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
		FileName = Collected_FilePrix;
		FileName += "_relation.paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
		FileName = Collected_FilePrix;
		FileName += ".paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
	}

	Generating_Entity_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	Enable_Usable_Button_ENET();
	OnPaint();
}

void CENET_Dlg::OnBnClickedAnalRelGen()
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Review Relation Extracting Result\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Review Relation Extracting Result\" is started ...", 1);

	if(Collected_Doc_Event_Flag){
		string FileName = Collected_FilePrix;
		FileName += ".paj";
		DeleteFile(NLPOP::string2CString(FileName.c_str()));
	}
	Generating_Relation_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	Enable_Usable_Button_ENET();
	OnPaint();

}

void CENET_Dlg::OnBnClickedAnalEntityViewer()
{
	string FileName = Collected_FilePrix;
	FileName += "_entity.paj";
	AppCall::Consensus_Open_Process(FileName.c_str(), Pajet_Path);
}
void CENET_Dlg::OnBnClickedAnalRelViewer()
{
	string FileName = Collected_FilePrix;
	FileName += "_relation.paj";
	AppCall::Consensus_Open_Process(FileName.c_str(), Pajet_Path);
}
void CENET_Dlg::Dispaly_Event_Network_Pictures()
{
	CRect   rect; 
	GetDlgItem(IDC_ANAL_PICTURE)->GetClientRect(&rect); 

   	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_ANAL_PICTURE); // 得到 Picture Control 句柄  
	pWnd->ModifyStyle(0, SS_BITMAP); // 修改它的属性为位图  
	CString CS_PicutrePath(PICTURE_PATH);
	CS_PicutrePath += "Event Network.bmp";
	pWnd->SetBitmap((HBITMAP)::LoadImage(NULL, CS_PicutrePath,   
                IMAGE_BITMAP,   470,   240, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE));
	OnPaint();
}

void CENET_Dlg::OnBnClickedAnalExample()
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Review Relation Extracting Result\" is not usable, please wait...", 1);
		return;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Review Relation Extracting Result\" is started ...", 1);

	Loading_EN_Info_Flag = true;

	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();
}

void CENET_Dlg::OnBnClickedAnalReset()
{
	int rtn = AfxMessageBox(_T("Reset ENET, Are your sure?"), MB_YESNO);
	if(rtn != IDYES){
		return;
	}
	if(ImpThread != NULL){   
		DWORD   dw;   
		GetExitCodeThread(ImpThread,&dw);   
		ExitThread(dw);   
	}
	Running_Document_Event_Flag = false;
	Running_Named_Entity_Flag = false;
	Running_Relation_Flag = false;
	Reckon_By_Time(false);
	p_CParentDlg->System_Busy_Flag = false;
	CENET_Busy_Flag = false;
	Collected_EventName = "";
	Collected_EventPath = "";
	Collected_KeyWords = "";
	Collected_Doc_Event_Flag = false;
	if(Opened_CDENer_Dlg_Flag){
		delete p_m_CDENer_Dlg;
	}
	Opened_CDENer_Dlg_Flag = false;
	m_EventNameEdit.SetWindowTextW(NLPOP::string2CString(Collected_EventName));
	m_KeyWordsEdit.SetWindowTextW(NLPOP::string2CString(Collected_KeyWords));
	Enable_Usable_Button_ENET();
}
bool CENET_Dlg::Command_From_Notification(const char* command)
{
	if(CENET_Busy_Flag){
		p_CParentDlg->Output_MSG("\"Command From Notification\" is not usable, please wait...", 1);
		return false;
	}
	CENET_Busy_Flag = true;
	Reckon_By_Time(true);
	Updata_ENET_Configurations();
	p_CParentDlg->Output_MSG("\"Command From Notification\" is started ...", 1);

	char InputBuf[MAX_CLAUSE_LENGTH];
	istringstream instream(command);
	instream.getline(InputBuf, MAX_CLAUSE_LENGTH, ' ');

	if(!strcmp("info", InputBuf)){
		Collecting_Info_Flag = true;
	}
	else if(!strcmp("delete", InputBuf)){
		instream.getline(InputBuf, MAX_CLAUSE_LENGTH, '\n');
		Deleting_Events_Organizing_Files_Flag = true;
		DeletingFileProx = InputBuf;
	}
	else if(!strcmp("PLT", InputBuf)){
		PLT_Analysis_Flag = true;
	}
	else if(!strcmp("act", InputBuf)){
		Action_Analysis_Flag = true;
	}
	ImpThread = CreateThread(NULL, 0, ENET_Implement_Threaad, (LPVOID)this, 0, &ImpphreadId);
	OnPaint();

	bool Return_Flag = false;
	Return_Flag = Collecting_Info_Flag || Deleting_Events_Organizing_Files_Flag || PLT_Analysis_Flag;
	return Return_Flag;
}
void CENET_Dlg::OnBnClickedEnetSpCheck()
{
	UpdateData(TRUE);
	Enable_Usable_Button_ENET();
}
