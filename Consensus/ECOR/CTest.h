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

class CECOR;

class CTest
{
public:
	CECOR* m_pCECOR;
	string m_TestFolder;

public:
	CTest();
	~CTest();
	void Generating_Testing_Reference_Documents(ACE_Corpus& pmACE_Corpus, vector<string>& RefTest_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);
	void ECOR_Model_Testing_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v, MaxentModel& ECOR_Maxent);
	void Output_Coference_Mention_Set_with_Erasing(const char* parthchar, map<string, ACE_DocInfo>& DocInfo_m, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);
	void Output_Mentions_Documents_List(const char* clistpath, const char* outputfolder, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);

	void Generate_Reference_Coreference_Data(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v);

	void ECOR_Model_Testing_Parting_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmTesting_v);
};
