// Convention.cpp : implementation file
//

#include "stdafx.h"
#include "..\\Consensus.h"
#include "ConvertUTF.h"
#include "Convention_Dlg.h"
#include "igraph.h"

// CConvention_Dlg dialog

IMPLEMENT_DYNAMIC(CConvention_Dlg, CDialog)

CConvention_Dlg::CConvention_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvention_Dlg::IDD, pParent)
	, m_Cycle(1)
	, m_Duration(24)
{
	Convention_Data_Floder = DATA_FOLDER;
	Convention_Data_Floder += "Convention\\";
	if(!NLPOP::FolderExist(NLPOP::string2CString(Convention_Data_Floder))){
		_mkdir(Convention_Data_Floder.c_str());
	}
}

CConvention_Dlg::~CConvention_Dlg()
{
}


int CConvention_Dlg::igraph_Port(int argc, char* argv[])
{
  igraph_real_t avg_path;
  igraph_t graph;
  igraph_vector_t dimvector;
  igraph_vector_t edges;
  int i;
  
  igraph_vector_init(&dimvector, 2);
  VECTOR(dimvector)[0]=30;
  VECTOR(dimvector)[1]=30;
  igraph_lattice(&graph, &dimvector, 0, IGRAPH_UNDIRECTED, 0, 1);

  igraph_rng_seed(igraph_rng_default(), 42);
  igraph_vector_init(&edges, 20);
  for (i=0; i<igraph_vector_size(&edges); i++) {
    VECTOR(edges)[i] = rand() % (int)igraph_vcount(&graph);
  }

  igraph_average_path_length(&graph, &avg_path, IGRAPH_UNDIRECTED, 1);
  printf("Average path length (lattice):            %f\n", (double) avg_path);

  igraph_add_edges(&graph, &edges, 0);
  igraph_average_path_length(&graph, &avg_path, IGRAPH_UNDIRECTED, 1);
  printf("Average path length (randomized lattice): %f\n", (double) avg_path);
  
  igraph_vector_destroy(&dimvector);
  igraph_vector_destroy(&edges);
  igraph_destroy(&graph);

  return 0;
}


void CConvention_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENTITY_VIEW_BUTTON, m_EntityPadB);
	DDX_Control(pDX, IDC_FINGER_VIEWBUTTON, m_FingerPadB);
	DDX_Control(pDX, IDC_RELATION_VIEWBUTTON, m_RelationPadB);
	DDX_Control(pDX, IDC_TEMPER_VIEWBUTTON, m_HotPadB);
	DDX_Control(pDX, IDC_BUTTON7, m_IntegB);
	DDX_Control(pDX, IDC_INTEGRATION_VIEWBUTTON, m_enIntegB);
	DDX_Control(pDX, IDC_VISUALIZATION, m_VisIntegB);
	DDX_Control(pDX, IDC_ENTITY_VIEW_BUTTON3, m_VisEntityB);
	DDX_Control(pDX, IDC_ENTITY_VIEW_BUTTON7, m_VisFingerB);
	DDX_Control(pDX, IDC_ENTITY_VIEW_BUTTON8, m_VisRelationB);
	DDX_Control(pDX, IDC_ENTITY_VIEW_BUTTON9, m_VisHotB);
	DDX_Text(pDX, IDC_RICHEDIT21, m_Cycle);
	DDX_Text(pDX, IDC_RICHEDIT22, m_Duration);
}


BEGIN_MESSAGE_MAP(CConvention_Dlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK12, &CConvention_Dlg::OnBnClickedCheck12)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON, &CConvention_Dlg::OnBnClickedEntityViewButton)
	ON_BN_CLICKED(IDC_FINGER_VIEWBUTTON, &CConvention_Dlg::OnBnClickedFingerViewButton)
	ON_BN_CLICKED(IDC_RELATION_VIEWBUTTON, &CConvention_Dlg::OnBnClickedRelationViewButton)
	ON_BN_CLICKED(IDC_TEMPER_VIEWBUTTON, &CConvention_Dlg::OnBnClickedTemperViewButton)
	ON_BN_CLICKED(IDC_INTEGRATION_VIEWBUTTON, &CConvention_Dlg::OnBnClickedIntergationViewButton)
	ON_BN_CLICKED(IDC_VISUALIZATION, &CConvention_Dlg::OnBnClickedVisualizationViewButton)
	ON_BN_CLICKED(IDC_BUTTON4, &CConvention_Dlg::OnBnClickedRunningButton)
	ON_BN_CLICKED(IDC_BUTTON7, &CConvention_Dlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_CONVENTION_RESET, &CConvention_Dlg::OnBnClickedResetButton)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON2, &CConvention_Dlg::OnBnClickedRunEntity)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON4, &CConvention_Dlg::OnBnClickedRunFinger)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON5, &CConvention_Dlg::OnBnClickedRunRelation)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON6, &CConvention_Dlg::OnBnClickedRunHot)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON3, &CConvention_Dlg::OnBnClickedEntityVisualize)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON7, &CConvention_Dlg::OnBnClickedFingerVisualize)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON8, &CConvention_Dlg::OnBnClickedRelationVisualize)
	ON_BN_CLICKED(IDC_ENTITY_VIEW_BUTTON9, &CConvention_Dlg::OnBnClickedHotVisualize)
	ON_BN_CLICKED(IDC_DOC_EVENTS_BUTTEN, &CConvention_Dlg::OnBnClickedDocEventsButten)
END_MESSAGE_MAP()


// CConvention_Dlg message handlers
void CConvention_Dlg::OnBnClickedRunningButton()
{
	m_EntityPadB.EnableWindow(TRUE);
	m_FingerPadB.EnableWindow(TRUE);
	m_RelationPadB.EnableWindow(TRUE);
	m_HotPadB.EnableWindow(TRUE);
	m_IntegB.EnableWindow(TRUE);
	m_enIntegB.EnableWindow(FALSE);
	m_VisEntityB.EnableWindow(TRUE);
	m_VisFingerB.EnableWindow(TRUE);
	m_VisRelationB.EnableWindow(TRUE);
	m_VisHotB.EnableWindow(TRUE);
}

void CConvention_Dlg::OnBnClickedButton7()
{
	m_enIntegB.EnableWindow(TRUE);
	m_VisIntegB.EnableWindow(TRUE);
}

//------------Clicked Run buttons
void CConvention_Dlg::OnBnClickedResetButton()
{
	m_EntityPadB.EnableWindow(FALSE);
	m_FingerPadB.EnableWindow(FALSE);
	m_RelationPadB.EnableWindow(FALSE);
	m_HotPadB.EnableWindow(FALSE);
	m_IntegB.EnableWindow(FALSE);
	m_enIntegB.EnableWindow(FALSE);
	m_VisIntegB.EnableWindow(FALSE);
	m_VisEntityB.EnableWindow(FALSE);
	m_VisFingerB.EnableWindow(FALSE);
	m_VisRelationB.EnableWindow(FALSE);
	m_VisHotB.EnableWindow(FALSE);
}
void CConvention_Dlg::Disable_Button_Convertion()
{
	m_EntityPadB.EnableWindow(FALSE);
	m_FingerPadB.EnableWindow(FALSE);
	m_RelationPadB.EnableWindow(FALSE);
	m_HotPadB.EnableWindow(FALSE);
	m_IntegB.EnableWindow(FALSE);
	m_enIntegB.EnableWindow(FALSE);
	m_VisIntegB.EnableWindow(FALSE);
	m_VisEntityB.EnableWindow(FALSE);
	m_VisFingerB.EnableWindow(FALSE);
	m_VisRelationB.EnableWindow(FALSE);
	m_VisHotB.EnableWindow(FALSE);
}

void CConvention_Dlg::Enable_Usable_Button_Convertion()
{
	m_EntityPadB.EnableWindow(TRUE);
	m_FingerPadB.EnableWindow(TRUE);
	m_RelationPadB.EnableWindow(TRUE);
	m_HotPadB.EnableWindow(TRUE);
	m_IntegB.EnableWindow(TRUE);
	m_enIntegB.EnableWindow(TRUE);
	m_VisIntegB.EnableWindow(TRUE);
}

void CConvention_Dlg::OnBnClickedRunEntity()
{
	m_VisEntityB.EnableWindow(TRUE);
	m_EntityPadB.EnableWindow(TRUE);
}
void CConvention_Dlg::OnBnClickedRunFinger()
{
	m_VisFingerB.EnableWindow(TRUE);
	m_FingerPadB.EnableWindow(TRUE);
}
void CConvention_Dlg::OnBnClickedRunRelation()
{
	m_VisRelationB.EnableWindow(TRUE);
	m_RelationPadB.EnableWindow(TRUE);
}
void CConvention_Dlg::OnBnClickedRunHot()
{
	m_VisHotB.EnableWindow(TRUE);
	m_HotPadB.EnableWindow(TRUE);
}
void CConvention_Dlg::OnBnClickedCheck12()
{

}

//-------View result files
void CConvention_Dlg::OnBnClickedEntityViewButton()
{
	string FilePath = Convention_Data_Floder + "networks.net";
	AppCall::Consensus_Open_Process(FilePath, Notepad_Path);
}
void CConvention_Dlg::OnBnClickedFingerViewButton()
{
	string FilePath = Convention_Data_Floder + "Partition.clu";
	AppCall::Consensus_Open_Process(FilePath, Notepad_Path);
}
void CConvention_Dlg::OnBnClickedRelationViewButton()
{
	string FilePath = Convention_Data_Floder + "Relation.dat";
	AppCall::Consensus_Open_Process(FilePath, Notepad_Path);
}
void CConvention_Dlg::OnBnClickedTemperViewButton()
{
	string FilePath = Convention_Data_Floder + "Vector.vec";
	AppCall::Consensus_Open_Process(FilePath, Notepad_Path);
}
void CConvention_Dlg::OnBnClickedIntergationViewButton()
{
	string FilePath = Convention_Data_Floder + "Analysis.paj";
	AppCall::Consensus_Open_Process(FilePath, Notepad_Path);
}

//-----Visualize---
void CConvention_Dlg::OnBnClickedVisualizationViewButton()
{
	string VisualFilePath = Convention_Data_Floder + "Analysis.paj";
	AppCall::Consensus_Open_Process(VisualFilePath, Pajet_Path);
}
void CConvention_Dlg::OnBnClickedEntityVisualize()
{
	string VisualFilePath = Convention_Data_Floder + "Enity.paj";
	AppCall::Consensus_Open_Process(VisualFilePath, Pajet_Path);
}

void CConvention_Dlg::OnBnClickedFingerVisualize()
{
	string VisualFilePath = Convention_Data_Floder + "Finger.paj";
	AppCall::Consensus_Open_Process(VisualFilePath, Pajet_Path);
}

void CConvention_Dlg::OnBnClickedRelationVisualize()
{
	string VisualFilePath = Convention_Data_Floder + "Relation.paj";
	AppCall::Consensus_Open_Process(VisualFilePath, Pajet_Path);
}
void CConvention_Dlg::OnBnClickedHotVisualize()
{
	string VisualFilePath = Convention_Data_Floder + "Hot.paj";
	AppCall::Consensus_Open_Process(VisualFilePath, Pajet_Path);
}


	
void CConvention_Dlg::OnBnClickedDocEventsButten()
{
	p_m_CDENer_Dlg = new CDENer_Dlg;
	p_m_CDENer_Dlg->Create(IDD_DOC_EVENT_REVIEWER, NULL);
	p_m_CDENer_Dlg->ShowWindow(SW_SHOW);
}
