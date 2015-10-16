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

class CCEDT;
class CRDC;
class CENET_Dlg;

typedef struct _ACE_Action{
	string DOCID;
	size_t CASID;
	size_t SENID;
	string TYPE;
	double value;
	string charseq;
	list<string> entity_mention_l;
} ACE_Action, *pACE_Action;


class CAct
{
public:
	CAct();
	~CAct();
	void Generating_Training_Data(const char* cPath, CENET_Dlg* p_m_CENET_Dlg);
	void Init_Lexicon(const char* leciconfolder, set<string>& m_WordSpace, size_t& Max_Lexicon_Size);
	void Action_Analysis_Port(CENET_Dlg* p_m_CENET_Dlg);
	void Generating_Negetive_Cases(ACE_Corpus& pmACE_Corpus, map<string, list<pair<string, pair<size_t,size_t>>>>& ACE_DocSentence_map);
	void Generating_Training_Cases(ACE_Corpus& pmACE_Corpus, map<string, list<pair<string, pair<size_t,size_t>>>>& ACE_DocSentence_map, vector<pair<string, string>>& EventCases_v);

	void Generating_Maxen_Model(CENET_Dlg* p_m_CENET_Dlg);
	void Generating_ACTION_Documents_in_Events (CENET_Dlg* p_m_CENET_Dlg);
	void Load_Action_Documents(const char* cPath, list<ACE_Action*>& Actions_l);
	void Generating_Action_Analysis_File(const char* cLoadPath, const char* cSavePath);
	void Generating_Action_Analysis_RELATION_pajek(const char* cLoadPath, const char* cSavePath, CENET_Dlg* p_m_CENET_Dlg);
	void Generating_Action_Analysis_ENTITY_pajek(const char* cENTITYPath, const char* cLoadPath, const char* cSavePath, CENET_Dlg* p_m_CENET_Dlg);
};
