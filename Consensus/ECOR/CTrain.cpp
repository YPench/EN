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

#include "stdafx.h"
#include <algorithm>
#include "CTrain.h"
#include "ConvertUTF.h"
#include "ECOR.h"
#include "ECCom.h"

CTrain::CTrain()
{

}
CTrain::~CTrain()
{

}


void CTrain::ECOR_Model_Training_Port(ACE_Corpus& pmACE_Corpus, vector<string>& pmtraining_v, MaxentModel& ECOR_Maxent)
{
	AppCall::Maxen_Responce_Message("Generating Training Candidates...\n\n");

	TrainFolder = m_pCECOR->m_ECOR_space;
	TrainFolder += "Training\\";
	NLPOP::Checking_Dir_if_none_then_Greate(TrainFolder);

	CoMentionPairVector CoCandit_v;
	CoValueVector CoValue_v;
	CoCandit_v.reserve(700000);
	CoValue_v.reserve(700000);
	//---------Candidate pair Generating
	m_pCECOR->m_CCandit.Generate_Coreference_Candidate_Port(TrainFolder.c_str(), pmACE_Corpus, pmtraining_v, CoValue_v, CoCandit_v);

	if(CoValue_v.size() != CoCandit_v.size()){
		AppCall::Secretary_Message_Box("Data Error in: CTrain::ECOR_Model_Training_Port()");
		return;
	}

	AppCall::Maxen_Responce_Message("Extracting Training Feature...\n\n");
	FeatureVctor Training_v;
	Training_v.reserve(700000);
	for(size_t i = 0; i < CoCandit_v.size(); i++){
		if(!strcmp(CoValue_v[i].first.c_str(), EXCLUSIVE)){
			continue;
		}
		/*if(i%10000 == 0){
			ostringstream ostream;
			ostream << "Processing: " << i << "/" << CoCandit_v.size() << endl << endl;
			AppCall::Maxen_Responce_Message(ostream.str().c_str());
		}*/
		//---------Generate Coreference Features;
		vector<string> pair_feature_v;
		pair<ACE_entity_mention*, ACE_entity_mention*>& loc_pair = CoCandit_v[i];
		m_pCECOR->m_CExtrue.Extracting_Mention_Pair_Feature_Port(loc_pair, pair_feature_v);
		MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(Training_v, pair_feature_v, !strcmp(loc_pair.first->Entity_ID.c_str(), loc_pair.second->Entity_ID.c_str())?POSITIVE:NEGETIVE);
	}

	AppCall::Maxen_Responce_Message("Training Model...\n\n");

	MAXEN::Maxen_Training_with_Erasing(Training_v, ECOR_Maxent, m_pCECOR->m_ECOR_ite);
}



