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
