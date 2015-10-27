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



