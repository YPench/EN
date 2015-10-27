/*
 * Copyright (C) 2015 by Yanping Chen <ypench@gmail.com>
 * Begin       : 01-Oct-2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser GPL (LGPL) as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
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
