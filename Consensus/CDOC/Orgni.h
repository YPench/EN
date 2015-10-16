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
#include "CGCom.h"
#include "ace.h"

class CDOC;
class COrgni
{
public:
	CDOC* m_p_CDOC;
	//map<string, size_t> m_FeatureCoords;
public:
	COrgni();
	~COrgni();
	bool Checking_Iterativ_Condition(int hierarchy,  vector<bool>& pmDocFilter_v);
	bool Checking_and_Save_Document_Event_Info(int hierarchy, const char* docmatrix, const char* cWorkSpace, vector<bool>& pmDocFilter_v);
	void Documents_Organizing_Segmenation(const char* cOrganizingFolder, vector<string>& TimeSetpFolder_v,  int pm_TimeStep,  BOOL pCDOC_Incremental_Flag);
	bool Checking_and_Generating_Events_Iterative_Filter_Mirror_Vector(map<string, size_t>& pmDocID2Event_m, map<size_t, vector<bool>*>& pmIteFilter_mv, vector<bool>& pmDocFilter_v);
};
