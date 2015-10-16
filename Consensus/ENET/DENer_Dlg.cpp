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
// DEver_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "..//Consensus.h"
#include "DENer_Dlg.h"


// CDENer_Dlg dialog


IMPLEMENT_DYNAMIC(CDENer_Dlg, CDialog)

CDENer_Dlg::CDENer_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDENer_Dlg::IDD, pParent)
{
	m_Collected_nItem = -1;
	m_TotalHierarchy = 0;
	m_hierarchy = 0;
}

CDENer_Dlg::~CDENer_Dlg()
{
}
void CDENer_Dlg::Init_Reviewer()
{
	m_Collected_nItem = -1;
	m_TotalHierarchy = 0;
	m_hierarchy = 0;
	m_CurnItem2ID_map.clear();
	m_DocEventList.DeleteAllItems();
	m_TotalID2EventName_map.clear();
	m_TotalID2EventPath_map.clear();
	m_TotalID2Keywords_map.clear();
	m_TotalDocInfoOfTimestamp_map.clear();

	m_TotalEventName2IDmap.clear();
	m_TotalChild2Father_map.clear();
	
	m_CurnItem2ID_map.clear();
	m_Timestamp2Path_map.clear();
	m_TimetampEventTree.clear();
	Enable_Usable_Button_DEVER();
}

void CDENer_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DOC_EVENT_LIST, m_DocEventList);
	DDX_Control(pDX, IDC_VIEWER_FOLDER_PATH, m_TimestampEdit);
	DDX_Control(pDX, IDC_VIEWER_DOC_NUM, m_DocNumEdit);
	DDX_Control(pDX, IDC_VIEWER_EVENT_NUM, m_EventNumEdit);
	DDX_Control(pDX, IDC_VIEWER_SELE_EVENT, m_SelEventEdit);
	DDX_Control(pDX, IDC_VIEWER_COLLECT_DOC_E, m_ActiveAnalysisButton);
	DDX_Control(pDX, IDC_REVIER_PAGES, m_HierarchyEdit);
	DDX_Control(pDX, IDC_REVIEW_NEXT, m_ChildrenButton);
	DDX_Control(pDX, IDC_REVIEW_FRONT, m_FatherButton);
	DDX_Control(pDX, IDC_VIEWER_FRONT_TIME, m_FrontTimeButton);
	DDX_Control(pDX, IDC_VIEWER_NEXT_TIME, m_NextTimeButton);
}


BEGIN_MESSAGE_MAP(CDENer_Dlg, CDialog)
	ON_BN_CLICKED(IDC_ANAL_LOAD_DOC, &CDENer_Dlg::OnBnClickedAnalLoadDoc)
	ON_NOTIFY(NM_CLICK,IDC_DOC_EVENT_LIST,OnItemClick)
	ON_BN_CLICKED(IDC_VIEWER_COLLECT_DOC_E, &CDENer_Dlg::OnBnClickedViewerCollectDocE)
	ON_BN_CLICKED(IDC_REVIEW_NEXT, &CDENer_Dlg::OnBnClickedChildren)
	ON_BN_CLICKED(IDC_REVIEW_FRONT, &CDENer_Dlg::OnBnClickedFather)
	ON_BN_CLICKED(IDC_VIEWER_FRONT_TIME, &CDENer_Dlg::OnBnClickedViewerFrontTime)
	ON_BN_CLICKED(IDC_VIEWER_NEXT_TIME, &CDENer_Dlg::OnBnClickedViewerNextTime)
END_MESSAGE_MAP()


// CDENer_Dlg message handlers
/*BOOL CENET_Dlg::PreTranslateMessage(MSG* pMsg)
{
	if(WM_KEYDOWN == pMsg->message ){ //keyboard buttondown
		UINT nKey = (int) pMsg->wParam;
		if( VK_RETURN == WM_LBUTTONDBLCLK){

			return TRUE ; 
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}*/

void CDENer_Dlg::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)   
{  
   // TODO: Add your control notification handler code here
	if(m_TotalID2EventName_map.empty()){
		return;
	}
	if(m_TotalID2EventName_map.find(m_EventTreeIter->first) == m_TotalID2EventName_map.end()){
		AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnItemClick(a0");
		return;
	}
	map<string, set<string>>& m_EventTree = m_EventTreeIter->second;
	map<size_t, string>& m_ID2EventName_map = m_TotalID2EventName_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2EventPath_map = m_TotalID2EventPath_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2Keywords_map = m_TotalID2Keywords_map[m_EventTreeIter->first];
	map<string, size_t>& m_EventName2IDmap = m_TotalEventName2IDmap[m_EventTreeIter->first];

   int nItem = -1;
	m_ActiveAnalysisButton.EnableWindow(FALSE);
   LPNMITEMACTIVATE lpNMItemActivate = (LPNMITEMACTIVATE)pNMHDR;  
   if(lpNMItemActivate != NULL){  
      nItem = lpNMItemActivate->iItem;  
   }
   if(-1 == nItem){
	return;
   }
   if(m_CurnItem2ID_map.empty()){
	return;
   }
   if(m_CurnItem2ID_map.find(nItem) == m_CurnItem2ID_map.end()){
	   AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnItemClick()");
   }
   m_Collected_nItem = m_CurnItem2ID_map[nItem];
	if(m_ID2EventName_map.find(m_Collected_nItem) == m_ID2EventName_map.end()){
		AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnItemClick()");
	}
	string EventName = m_ID2EventName_map[m_Collected_nItem];
	m_SelEventEdit.SetWindowTextW(NLPOP::string2CString(EventName));
	
	Enable_Usable_Button_DEVER();
}
void CDENer_Dlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}
void CDENer_Dlg::Disable_Button_DEVER()
{
	m_ActiveAnalysisButton.EnableWindow(FALSE);
	m_ChildrenButton.EnableWindow(FALSE);
	m_FatherButton.EnableWindow(FALSE);
	m_FrontTimeButton.EnableWindow(FALSE);
	m_NextTimeButton.EnableWindow(FALSE);
}

void CDENer_Dlg::Enable_Usable_Button_DEVER()
{
	Disable_Button_DEVER();
	if(m_TotalID2EventName_map.empty()){
		return;
	}
	if(m_TotalID2EventName_map.find(m_EventTreeIter->first) == m_TotalID2EventName_map.end()){
		AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::Enable_Usable_Button_DEVER()");
		return;
	}
	map<string, set<string>>& m_EventTree = m_EventTreeIter->second;
	map<size_t, string>& m_ID2EventName_map = m_TotalID2EventName_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2EventPath_map = m_TotalID2EventPath_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2Keywords_map = m_TotalID2Keywords_map[m_EventTreeIter->first];
	map<string, size_t>& m_EventName2IDmap = m_TotalEventName2IDmap[m_EventTreeIter->first];
	map<string, pair<size_t, size_t>>& m_DocInfoOfTimestamp = m_TotalDocInfoOfTimestamp_map[m_EventTreeIter->first];

	//---------Father and children Button
	string EventName = m_ID2EventName_map[m_Collected_nItem];
	if(-1 != m_Collected_nItem){
		m_ActiveAnalysisButton.EnableWindow(TRUE);

		m_ChildrenButton.EnableWindow(FALSE);
		if(m_EventTree.find(EventName) != m_EventTree.end()){
			if(!m_EventTree[EventName].empty()){
				m_ChildrenButton.EnableWindow(TRUE);
			}
		}
		if(1 == m_hierarchy){
			m_FatherButton.EnableWindow(FALSE);
		}
		else{
			m_FatherButton.EnableWindow(TRUE);
		}
	}
	//---------Timestamp Button
	if(m_EventTreeIter == m_TimetampEventTree.begin()){
		m_FrontTimeButton.EnableWindow(FALSE);
	}
	else{
		m_FrontTimeButton.EnableWindow(TRUE);
	}
	if(m_EventTreeIter != m_TimetampEventTree.end()){
		map<string, map<string, set<string>>>::iterator loc_mmite = m_EventTreeIter;
		loc_mmite++;
		if(loc_mmite != m_TimetampEventTree.end()){
			m_NextTimeButton.EnableWindow(TRUE);
		}
	}
	ostringstream ostream;
	ostream.str("");
	ostream << m_hierarchy << "/" << m_TotalHierarchy;
	m_HierarchyEdit.SetWindowTextW(NLPOP::string2CString(ostream.str()));

	if((1 == m_hierarchy) && (-1 == m_Collected_nItem)){
		EventName = "Event-0";
	}
	ostream.str("");
	ostream << m_DocInfoOfTimestamp[EventName].first;
	m_DocNumEdit.SetWindowTextW(NLPOP::string2CString(ostream.str()));
	ostream.str("");
	ostream << m_DocInfoOfTimestamp[EventName].second;
	m_EventNumEdit.SetWindowTextW(NLPOP::string2CString(ostream.str()));

	//------------------------------------------
}


void CDENer_Dlg::Init_Document_Event_Viewer(const char* cWorkSpace)
{
	CRect rect;
	m_DocEventList.GetClientRect(&rect);
	int ColumnLength_1 = rect.Width()/18;
	int ColumnLength_3 = rect.Width()/16;
	int ColumnLength_2 = rect.Width() - ColumnLength_1 - ColumnLength_3;
	m_DocEventList.SetExtendedStyle(m_DocEventList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_DocEventList.InsertColumn(0, _T("ID"), LVCFMT_CENTER, ColumnLength_1, 0);   
	m_DocEventList.InsertColumn(1, _T("Key Words"), LVCFMT_LEFT, ColumnLength_2, 1);
	m_DocEventList.InsertColumn(2, _T("Sub"), LVCFMT_CENTER, ColumnLength_3, 2);
	m_DocEventFolder = NLPOP::string2CString(cWorkSpace);
	m_ActiveAnalysisButton.EnableWindow(FALSE);
	m_ChildrenButton.EnableWindow(FALSE);
	m_FatherButton.EnableWindow(FALSE);
	m_FrontTimeButton.EnableWindow(FALSE);
	m_NextTimeButton.EnableWindow(FALSE);
	ShowWindow(SW_SHOW);
	ostringstream ostream;
	ostream << 0 << "/" << 0;
	m_HierarchyEdit.SetWindowTextW(NLPOP::string2CString(ostream.str()));
}


void CDENer_Dlg::OnBnClickedAnalLoadDoc()
{
	vector<string> TimestampPath_v;
	NLPOP::Get_Child_Folders(NLPOP::CString2string(m_DocEventFolder).c_str(), TimestampPath_v);
	string EventName;
	if(TimestampPath_v.empty()){
		return;
	}
	for(size_t i = 0; i < TimestampPath_v.size(); i++){
		EventName = TimestampPath_v[i];
		EventName = EventName.substr(0, EventName.rfind('\\'));
		EventName = EventName.substr(EventName.rfind('\\')+1, EventName.length()-EventName.rfind('\\')+1);
		m_TimetampEventTree[EventName];
		m_Timestamp2Path_map.insert(make_pair(EventName, TimestampPath_v[i]));
	}
	m_EventTreeIter = m_TimetampEventTree.begin();
	m_TimestampEdit.SetWindowTextW(NLPOP::string2CString(m_EventTreeIter->first.c_str()));
	Load_and_Display_Timestamp_inner_EventTree(m_Timestamp2Path_map[m_EventTreeIter->first].c_str());
}

void CDENer_Dlg::Load_and_Display_Timestamp_inner_EventTree(const char* cTimestampPath)
{
	map<string, set<string>>& m_EventTree = m_EventTreeIter->second;
	map<size_t, string>& m_ID2EventName_map = m_TotalID2EventName_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2EventPath_map = m_TotalID2EventPath_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2Keywords_map = m_TotalID2Keywords_map[m_EventTreeIter->first];
	map<string, size_t>& m_EventName2IDmap = m_TotalEventName2IDmap[m_EventTreeIter->first];
	map<string, pair<size_t, size_t>>& m_DocInfoOfTimestamp = m_TotalDocInfoOfTimestamp_map[m_EventTreeIter->first];
	map<string, string>& m_Child2Father_map = m_TotalChild2Father_map[m_EventTreeIter->first];

	ostringstream ostream;
	if(m_EventTree.empty()){
		vector<string> EventsPath_v;
		size_t EventNum = 0;
		size_t DocMum = 0;
		size_t ItemID = 0;
		NLPOP::Get_Specified_Files(cTimestampPath, EventsPath_v, ".EVENT");
		if(EventsPath_v.empty()){
			return;
		}
		string loc_temp = EventsPath_v[EventsPath_v.size()-1];
		EventsPath_v[EventsPath_v.size()-1] = EventsPath_v[0];
		EventsPath_v[0] = loc_temp;
		for(size_t i = 0; i < EventsPath_v.size(); i++){
			map<string, size_t> DocID2Event_m;
			vector<vector<string>*> EventTopTerm_vv;
			string EventName;
			string Keywords;
			EventName = EventsPath_v[i];
			AppCall::Load_Document_Events_Info(EventName.c_str(), DocID2Event_m, EventTopTerm_vv);
			EventName = EventName.substr(0, EventName.rfind('\\'));
			EventName = EventName.substr(EventName.rfind('\\')+1, EventName.length()-EventName.rfind('\\')+1);
			//------------------------
			if(m_DocInfoOfTimestamp.find(EventName) == m_DocInfoOfTimestamp.end()){
				m_DocInfoOfTimestamp.insert(make_pair(EventName, make_pair(DocID2Event_m.size(), 0)));
			}
			set<size_t> Event_s;
			for(map<string, size_t>::iterator mite = DocID2Event_m.begin(); mite != DocID2Event_m.end(); mite++){
				if(Event_s.find(mite->second) == Event_s.end()){
					Event_s.insert(mite->second);
				}
				ostream.str("");
				ostream << EventName << "-" << mite->second;
				if(m_DocInfoOfTimestamp.find(ostream.str()) == m_DocInfoOfTimestamp.end()){
					m_DocInfoOfTimestamp.insert(make_pair(ostream.str(), make_pair(0, 0)));
				}
				m_DocInfoOfTimestamp[ostream.str()].first++;
			}
			m_DocInfoOfTimestamp[EventName].second = Event_s.size();
			if(m_EventTree.find(EventName) == m_EventTree.end()){
				m_EventTree[EventName];
			}
			set<string>& ChildEvent_s = m_EventTree[EventName];
			for(size_t j = 0; j < EventTopTerm_vv.size(); j++){
				if(Event_s.find(j) != Event_s.end()){
					vector<string>& loc_EventTerm_v = *EventTopTerm_vv[j];
					size_t TermNum = 0;
					string candittopterm;
					for(size_t k = 0; k < loc_EventTerm_v.size(); k++){
						if(loc_EventTerm_v[k].length() < 3){
							candittopterm += loc_EventTerm_v[k];
							candittopterm += ";";
							continue;
						}
						Keywords += loc_EventTerm_v[k];
						Keywords += ";";
						if(!(TermNum++ < MAX_TERM_NUM)){
							break;
						}
					}
					if(TermNum < MAX_TERM_NUM){
						char getlineBuf[MAX_SENTENCE];
						istringstream instream(candittopterm.c_str());
						while(instream.peek() != EOF && TermNum < MAX_TERM_NUM){
							instream.getline(getlineBuf, MAX_SENTENCE, ';');
							Keywords += getlineBuf;
							Keywords += ";";
							TermNum++;
						}
					}
					char prev;
					ostringstream formatedostream;
					prev = formatedostream.fill('0');
					formatedostream.width(3);
					formatedostream  << j;
					formatedostream.fill(prev);
					ostream.str("");
					ostream << EventName << "-" << formatedostream.str();
					ChildEvent_s.insert(ostream.str());
					m_ID2EventName_map.insert(make_pair(ItemID, ostream.str()));
					m_ID2EventPath_map.insert(make_pair(ItemID, EventsPath_v[i]));
					m_ID2Keywords_map.insert(make_pair(ItemID, Keywords));
					m_EventName2IDmap.insert(make_pair(ostream.str(), ItemID));
					if(m_Child2Father_map.find(ostream.str()) == m_Child2Father_map.end()){
						m_Child2Father_map.insert(make_pair(ostream.str(), EventName));
					}
					Keywords = "";
					ItemID++;
				}
				delete EventTopTerm_vv[j];
			}
			//-------------------------------------------
			//if(m_ID2Keywords_map.size() > 55){
			//	break;
			//}
		}
	}
	size_t ItemID = 0;
	if(m_EventTree.find("Event-0") == m_EventTree.end()){
		AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnBnClickedAnalLoadDoc()");
	}
	size_t EventnItemID;
	string HasChildren;
	m_CurnItem2ID_map.clear();
	m_DocEventList.DeleteAllItems();
	for(set<string>::iterator site = m_EventTree["Event-0"].begin(); site != m_EventTree["Event-0"].end(); site++){
		if(m_EventName2IDmap.find(*site) == m_EventName2IDmap.end()){
			AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnBnClickedAnalLoadDoc()");
		}
		EventnItemID = m_EventName2IDmap[*site];
		ostream.str("");
		ostream << EventnItemID;
		m_DocEventList.InsertItem(ItemID, NLPOP::string2CString(ostream.str()));
		m_DocEventList.SetItemText(ItemID, 1, NLPOP::string2CString(m_ID2Keywords_map[EventnItemID]));
		HasChildren = "F";
		if(m_EventTree.find(*site) != m_EventTree.end()){
			if(!m_EventTree[*site].empty()){
				HasChildren = "T";
			}
		}
		m_DocEventList.SetItemText(ItemID, 2, NLPOP::string2CString(HasChildren));
		m_CurnItem2ID_map.insert(make_pair(ItemID, EventnItemID));
		ItemID++;
		if(ItemID >= max_event_NUM){
			break;
		}
	}
	m_hierarchy = 1;

	m_TotalHierarchy = Iteration_Finding_Hierarchy_Number(cTimestampPath, 0);

	Enable_Usable_Button_DEVER();
}
int CDENer_Dlg::Iteration_Finding_Hierarchy_Number(const char* cTimestampPath, int Num)
{
	vector<string> EventPath_v;
	NLPOP::Get_Child_Folders(cTimestampPath, EventPath_v);
	if(EventPath_v.empty()){
		return Num;
	}
	Num++;
	int Max_Num = Num;
	for(size_t i = 0; i < EventPath_v.size(); i++){
		int child_num = Iteration_Finding_Hierarchy_Number(EventPath_v[i].c_str(), Num);
		if( child_num > Max_Num){
			Max_Num = child_num;
		}
	}
	return Max_Num;
}


void CDENer_Dlg::OnBnClickedViewerCollectDocE()
{
	if(m_TotalID2EventName_map.empty()){
		return;
	}
	map<size_t, string>& m_ID2EventName_map = m_TotalID2EventName_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2EventPath_map = m_TotalID2EventPath_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2Keywords_map = m_TotalID2Keywords_map[m_EventTreeIter->first];

	if((-1 == m_Collected_nItem) || (m_Collected_nItem >= (int)m_ID2EventPath_map.size())){
		return;
	}
	p_CParentDlg->Collected_EventName = m_ID2EventName_map[m_Collected_nItem];
	p_CParentDlg->Collected_EventPath = m_ID2EventPath_map[m_Collected_nItem];
	p_CParentDlg->Collected_KeyWords = m_ID2Keywords_map[m_Collected_nItem];
	string FileName = p_CParentDlg->Collected_EventPath;
	FileName = FileName.substr(0, FileName.rfind('\\')+1);
	FileName += p_CParentDlg->Collected_EventName;
	FileName += "\\";
	FileName += p_CParentDlg->Collected_EventName;
	p_CParentDlg->Collected_FilePrix = FileName;

	ShowWindow(SW_HIDE);
	p_CParentDlg->Collected_Doc_Event_Flag = true;
	
	::SendMessage(p_CParentDlg->m_hWnd, WM_ENET_MESSAGE, (WPARAM)0, 0);
}

void CDENer_Dlg::OnBnClickedChildren()
{
	if(m_TotalID2EventName_map.empty()){
		return;
	}
	if(-1 == m_Collected_nItem){
		return;
	}
	map<size_t, string>& m_ID2EventName_map = m_TotalID2EventName_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2Keywords_map = m_TotalID2Keywords_map[m_EventTreeIter->first];
	map<string, size_t>& m_EventName2IDmap = m_TotalEventName2IDmap[m_EventTreeIter->first];

	map<string, set<string>>& m_EventTree = m_EventTreeIter->second;
	string EventName = m_ID2EventName_map[m_Collected_nItem];
	if(m_EventTree.find(EventName) == m_EventTree.end()){
		return;
	}
	if(m_EventTree[EventName].empty()){
		return;
	}
	set<string>& loc_Event_s = m_EventTree[EventName];
	m_DocEventList.DeleteAllItems();
	size_t ItemID = 0;
	size_t EventnItemID;
	ostringstream ostream;
	string HasChildren;
	m_CurnItem2ID_map.clear();
	for(set<string>::iterator site = loc_Event_s.begin(); site != loc_Event_s.end(); site++){
		if(m_EventName2IDmap.find(*site) == m_EventName2IDmap.end()){
			AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnBnClickedAnalLoadDoc()");
		}
		EventnItemID = m_EventName2IDmap[*site];
		ostream.str("");
		ostream << EventnItemID;
		m_DocEventList.InsertItem(ItemID, NLPOP::string2CString(ostream.str()));
		m_DocEventList.SetItemText(ItemID, 1, NLPOP::string2CString(m_ID2Keywords_map[EventnItemID]));
		HasChildren = "F";
		if(m_EventTree.find(*site) != m_EventTree.end()){
			if(!m_EventTree[*site].empty()){
				HasChildren = "T";
			}
		}
		m_DocEventList.SetItemText(ItemID, 2, NLPOP::string2CString(HasChildren));
		m_CurnItem2ID_map.insert(make_pair(ItemID, EventnItemID));
		ItemID++;
		if(ItemID >= max_event_NUM){
			break;
		}
	}
	m_hierarchy += 1;
	m_Collected_nItem = -1;
	Enable_Usable_Button_DEVER();
}

void CDENer_Dlg::OnBnClickedFather()
{
	if(m_TotalID2EventName_map.empty()){
		return;
	}
	if(-1 == m_Collected_nItem){
		return;
	}
	map<size_t, string>& m_ID2EventName_map = m_TotalID2EventName_map[m_EventTreeIter->first];
	map<size_t, string>& m_ID2Keywords_map = m_TotalID2Keywords_map[m_EventTreeIter->first];
	map<string, size_t>& m_EventName2IDmap = m_TotalEventName2IDmap[m_EventTreeIter->first];
	map<string, string>& m_Child2Father_map = m_TotalChild2Father_map[m_EventTreeIter->first];

	map<string, set<string>>& m_EventTree = m_EventTreeIter->second;
	string EventName = m_ID2EventName_map[m_Collected_nItem];
	if(m_Child2Father_map.find(EventName) == m_Child2Father_map.end()){
		AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnBnClickedFather()");
		return;
	}
	EventName = m_Child2Father_map[EventName];
	if(m_Child2Father_map.find(EventName) == m_Child2Father_map.end()){
		AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnBnClickedFather()");
		return;
	}
	EventName = m_Child2Father_map[EventName];

	set<string>& loc_Event_s = m_EventTree[EventName];
	m_DocEventList.DeleteAllItems();
	size_t ItemID = 0;
	size_t EventnItemID;
	ostringstream ostream;
	string HasChildren;
	m_CurnItem2ID_map.clear();
	for(set<string>::iterator site = loc_Event_s.begin(); site != loc_Event_s.end(); site++){
		if(m_EventName2IDmap.find(*site) == m_EventName2IDmap.end()){
			AppCall::Secretary_Message_Box("Data Error in: CDENer_Dlg::OnBnClickedAnalLoadDoc()");
		}
		EventnItemID = m_EventName2IDmap[*site];
		ostream.str("");
		ostream << EventnItemID;
		m_DocEventList.InsertItem(ItemID, NLPOP::string2CString(ostream.str()));
		m_DocEventList.SetItemText(ItemID, 1, NLPOP::string2CString(m_ID2Keywords_map[EventnItemID]));
		HasChildren = "F";
		if(m_EventTree.find(*site) != m_EventTree.end()){
			if(!m_EventTree[*site].empty()){
				HasChildren = "T";
			}
		}
		m_DocEventList.SetItemText(ItemID, 2, NLPOP::string2CString(HasChildren));
		m_CurnItem2ID_map.insert(make_pair(ItemID, EventnItemID));
		ItemID++;
		if(ItemID >= max_event_NUM){
			break;
		}
	}
	m_hierarchy -= 1;
	m_Collected_nItem = -1;
	Enable_Usable_Button_DEVER();
}

void CDENer_Dlg::OnBnClickedViewerFrontTime()
{
	if(m_EventTreeIter != m_TimetampEventTree.begin()){
		m_EventTreeIter--;
	}
	else{
		Enable_Usable_Button_DEVER();
		return;
	}
	m_DocEventList.DeleteAllItems();
	m_CurnItem2ID_map.clear();
	m_Collected_nItem = -1;
	m_TimestampEdit.SetWindowTextW(NLPOP::string2CString(m_EventTreeIter->first.c_str()));
	Load_and_Display_Timestamp_inner_EventTree(m_Timestamp2Path_map[m_EventTreeIter->first].c_str());
	Enable_Usable_Button_DEVER();
}

void CDENer_Dlg::OnBnClickedViewerNextTime()
{
	if(m_EventTreeIter != m_TimetampEventTree.end()){
		m_EventTreeIter++;
	}
	else{
		Enable_Usable_Button_DEVER();
		return;
	}

	m_DocEventList.DeleteAllItems();
	m_CurnItem2ID_map.clear();
	m_Collected_nItem = -1;
	m_TimestampEdit.SetWindowTextW(NLPOP::string2CString(m_EventTreeIter->first.c_str()));
	Load_and_Display_Timestamp_inner_EventTree(m_Timestamp2Path_map[m_EventTreeIter->first].c_str());
	Enable_Usable_Button_DEVER();
}
