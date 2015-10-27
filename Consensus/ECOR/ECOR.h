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

#include "Config.h"
#include "..\\Include\\ace.h"
#include "maxen.h"
#include "CTrain.h"
#include "CTest.h"
#include "Exture.h"
#include "Candit.h"
#include "Parser.h"
#include "Psture.h"
#include "Partor.h"
#include "wordnet.h"

class CECOR
{

public:
	MaxentModel ECOR_Maxent;
	string m_ECOR_space;
	string m_CorpusTYPE;
	size_t m_nCross;
	size_t m_ECOR_ite;

	CTrain m_CTrain;
	CTest m_CTest;
	CExtrue m_CExtrue;
	CCandit m_CCandit;
	CParser m_CParser;
	CPstrue m_Pasture;
	CPartor m_CPartor;
	Cwordnet m_Cwordnet;

	map<string, ACE_entity_mention*> MentionSpace_m;

	BOOL m_ALoadCandits_Flag;
	BOOL m_UpdateCandits_Flag;
	BOOL m_PsAutoLoad_Flag;
	BOOL m_PsUpdate_Flag;
	BOOL m_SaveTested_Flag;

public:
	CECOR();
	~CECOR();

	void ECOR_Coreference_Parting_Port(ACE_Corpus& pmACE_Corpus, const char* cPath);
	void CECOR_Training_and_Testing_Port(ACE_Corpus& pmACE_Corpus, const char* cCoupusPath);

	void ECOR_Stanford_Parsing_Port(ACE_Corpus& pmACE_Corpus, const char* cCoupusPath);
	void ECOR_Init_Corpus_TYPE(const char* cCoupusPath);

	void EXCLUSIVE_Relation_Statistic_Information(ACE_Corpus& pmACE_Corpus);
};
