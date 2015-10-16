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

#include "..\\Include\\ace.h"
#include "maxen.h"

class CCEDT;


typedef struct _BountRet{
	bool START_Flag;
	bool END_Flag;
	string mention;
	string insur;
}BountRet, *pBountRet;

class CAntical
{
public:
	CCEDT* pCCEDT;


public:
	CAntical();
	~CAntical();

	void ACE_Evaluation_Toolkit_Call(const char* toolkitfolder);

	void Corpus_Entity_and_Entity_Mention_Information(vector<NE_Surround*>& Surround_v, bool Output_Flag);
	void ACE_Named_Entity_Recognition_Evaluation_Port(vector<NE_Surround*>& Surround_v, vector<DismCase*>& pmDismCase_v);
	void Output_For_ACE_Named_Entity_Recognition_Evaluation(vector<NE_Surround*>& Surround_v, vector<DismCase*>& pmDismCase_v);

	void Candit_Performance_Evaluation_Output_Result(vector<CanditCase*>& pmCandit_v);
	void Analysizing_Performance_Evaluation_on_Extend_or_Head_Only_Recognition(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
	void Analysizing_Performance_Evaluation_on_Extend_and_Head_Matching(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
	void ACE_Named_Entity_Recognition_Output_Result(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v);
	void Analysizing_Performance_Evaluation_on_Mention(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
	void Detect_Boundary_Result_Analyser();
	void Named_Entity_Result_Analyser(vector<DismCase>& pmCanditCase_v, vector<bool>& pmNE_TestRet_v);

	void Candidates_Performance_Analysis_Single_Extend_or_Head_Mention(vector<NE_Surround*>& testing_Surround_v, bool CEDT_Head_Flag, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m);
};
