// Fingerprint.cpp : implementation file
//

#include "stdafx.h"
#include "Consensus.h"
#include "FPrint_Dlg.h"


// CFPrint_Dlg dialog

IMPLEMENT_DYNAMIC(CFPrint_Dlg, CDialog)

CFPrint_Dlg::CFPrint_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFPrint_Dlg::IDD, pParent)
{

}

CFPrint_Dlg::~CFPrint_Dlg()
{
}

void CFPrint_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFPrint_Dlg, CDialog)
END_MESSAGE_MAP()


// CFPrint_Dlg message handlers
