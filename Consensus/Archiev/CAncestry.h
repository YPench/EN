#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

//#include "Ctruss.h"
#include "..\\Include\\CGCom.h"
class CAcina;
typedef struct _MultidichMultidichTaskInfo
{
	DWORD (WINAPI *pCopeThread)(LPVOID);
	vector<string> sContent_v;
	vector<LPVOID> pParam_v;
	CAcina*	pCAcina;
}MultidichTaskInfo, *pMultidichTaskInfo;

#define Ance_FUN(fun) (string (CAncestry::*)(string, vector<string>, vector<LPVOID>))(&fun)


//-------------------CAncestry-------------------------------------------
class CAncestry 
{
public:
	//map<std::string, ANCESTRYFUNCTION> m_AncestryFun;


public:
	CAncestry();
	~CAncestry();

};
