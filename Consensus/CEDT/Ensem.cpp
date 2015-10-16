#include "stdafx.h"
#include "Ensem.h"
#include "NECom.h"

CEnsem::CEnsem()
{
	Maximum_NE_Length = 8;
}
CEnsem::~CEnsem()
{

}
//--------Generate Training-------------------------------------------------------------------------
void CEnsem::Generate_ACE_NE_Training_Cases(string CaseFilePath, string TrainingCaseFilePath, string infopath)
{
	string LexiconPath = DATA_FOLDER;
	LexiconPath += "CVMC.dat";
	m_CMaxmatch.Init_Maximum_Matcher_Lexicon(LexiconPath.c_str());

//	if(CEDT_Gen_Info_Flag){
//		m_CNEnt.Output_CEDT_Info_File(infopath.c_str(), NE_Surround_v, true);
//	}
	multimap<string, vector<pair<string, unsigned long>>> Training_map;
	//m_CNEnt.Load_Named_Entity_Cases(CaseFilePath.c_str());
	//Extracting_Regex_Expression();
	//Named_Case_v.reserve(200000);
	for(vector<NE_Surround>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end(); lite++){
		
		for(vector<ACE_entity_mention>::iterator pvite = lite->entity_mention_v.begin(); pvite != lite->entity_mention_v.end(); pvite++){
			string Named_Entity_Type;
			
			map<string, unsigned long> WordsCnt_map;
			vector<pair<string, unsigned long>> loc_RCase_v;

			string leftstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(lite->insur.c_str(), 0, pvite->extent.START);
			string rigthstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(lite->insur.c_str(), pvite->extent.END + 1, -1);
			string instr = pvite->extent.charseq;

			ace_op::Delet_0AH_and_20H_in_string(leftstr);
			ace_op::Delet_0AH_and_20H_in_string(rigthstr);
			ace_op::Delet_0AH_and_20H_in_string(instr);

			vector<string> adjacent_feature_v;
			m_CMaxmatch.Adjacent_Left_Border_Words(leftstr.c_str(), true, 3, adjacent_feature_v);
			m_CMaxmatch.Adjacent_Right_Border_Words(rigthstr.c_str(), true, 3, adjacent_feature_v);
			m_CMaxmatch.Double_Side_Inner_Words(instr.c_str(), 3, adjacent_feature_v);
			
			for(vector<string>::iterator vite = adjacent_feature_v.begin(); vite != adjacent_feature_v.end(); vite++){
				WordsCnt_map.insert(make_pair(vite->data(), 1));
			}

			Named_Entity_Type = "Positive";
			for(map<string, unsigned long>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
				loc_RCase_v.push_back(make_pair(mite->first, mite->second));
			}
			Training_map.insert(make_pair(Named_Entity_Type, loc_RCase_v));
		}

		for(vector<ACE_extent>::iterator nvite = lite->Negetive_NE_v.begin(); nvite != lite->Negetive_NE_v.end(); nvite++){
			string Named_Entity_Type;
			
			map<string, unsigned long> WordsCnt_map;
			vector<pair<string, unsigned long>> loc_RCase_v;
			string leftstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(lite->insur.c_str(), 0, nvite->START);
			string rigthstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(lite->insur.c_str(), nvite->END + 1, -1);
			string instr = nvite->charseq;
			
			ace_op::Delet_0AH_and_20H_in_string(leftstr);
			ace_op::Delet_0AH_and_20H_in_string(rigthstr);
			ace_op::Delet_0AH_and_20H_in_string(instr);

			vector<string> adjacent_feature_v;
			m_CMaxmatch.Adjacent_Left_Border_Words(leftstr.c_str(), true, 3, adjacent_feature_v);
			m_CMaxmatch.Adjacent_Right_Border_Words(rigthstr.c_str(), true, 3, adjacent_feature_v);
			m_CMaxmatch.Double_Side_Inner_Words(instr.c_str(), 3, adjacent_feature_v);

			for(vector<string>::iterator vite = adjacent_feature_v.begin(); vite != adjacent_feature_v.end(); vite++){
				WordsCnt_map.insert(make_pair(vite->data(), 1));
			}
			
			Named_Entity_Type = "Negative";
			for(map<string, unsigned long>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
				loc_RCase_v.push_back(make_pair(mite->first, mite->second));
			}
			Training_map.insert(make_pair(Named_Entity_Type, loc_RCase_v));
		}
	}

	//ace_op::Output_Extract_ACE_Relation_Words_Context_For_Maxent(TrainingCaseFilePath.c_str(), Training_map);
}


//--------Generate Cases-------------------------------------------------------------------------
void CEnsem::Named_Entity_Recognization_Ensem(const char* NE_Cases_Path, ACE_Corpus& ACE_Corpus)
{
	//NECOM::Extracting_Named_Entity_Surrounding(NE_Cases_Path, ACE_Corpus, NE_Surround_v);

	Generate_Negetive_Case_Extend(NE_Surround_v);

	Delete_Negetive_Case_Without_Any_Char_In_Positive(NE_Surround_v);

	NECOM::Save_Named_Entity_Cases(NE_Cases_Path, NE_Surround_v);
}
void CEnsem::Delete_Negetive_Case_Without_Any_Char_In_Positive(vector<NE_Surround>& NE_Surround_v)
{
	char sChar[3];
	sChar[2]=0;
	set<string> neChar_s;
	for(vector<NE_Surround>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end();lite++){
		for(vector<ACE_entity_mention>::iterator pvite = lite->entity_mention_v.begin(); pvite != lite->entity_mention_v.end(); pvite++){
			for(size_t i = 0; i < pvite->extent.charseq.length(); ){
				sChar[0] = pvite->extent.charseq.c_str()[i++];
				sChar[1] = 0;	
				if(sChar[0] < 0 ){
					sChar[1] = pvite->extent.charseq.c_str()[i++];
				}
				neChar_s.insert(sChar);
			}
		}
	}
	bool Words_Hit_Flag;
	for(vector<NE_Surround>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end();lite++){
		for(vector<ACE_extent>::iterator nvite = lite->Negetive_NE_v.begin(); nvite != lite->Negetive_NE_v.end();  ){
			Words_Hit_Flag = false;
			for(size_t i = 0; i < nvite->charseq.length(); ){
				sChar[0] = nvite->charseq.c_str()[i++];
				sChar[1] = 0;	
				if(sChar[0] < 0 ){
					sChar[1] = nvite->charseq.c_str()[i++];
				}
				if(neChar_s.find(sChar) != neChar_s.end()){
					Words_Hit_Flag = true;
					break;
				}
			}
			if(!Words_Hit_Flag){
				nvite = lite->Negetive_NE_v.erase(nvite);
			}
			else{
				nvite++;
			}
		}
	}
}

void CEnsem::Generate_Negetive_Case_Extend(vector<NE_Surround>& NE_Surround_v)
{
	size_t offset;
	for(vector<NE_Surround>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end();lite++){
		offset = lite->START;
		lite->START = 0;
		lite->END = lite->END - offset;
		for(vector<ACE_entity_mention>::iterator pvite = lite->entity_mention_v.begin(); pvite != lite->entity_mention_v.end(); pvite++){
			pvite->extent.START = pvite->extent.START - offset;
			pvite->extent.END = pvite->extent.END - offset;
			pvite->head.START = pvite->head.START - offset;
			pvite->head.END = pvite->head.START - offset;
		}
	}
	char sChar[3];
	sChar[2]=0;
	size_t current_length;
	size_t Last_Position;
	size_t current_Position;
	string locstr;
	for(vector<NE_Surround>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end(); lite++){
		Last_Position = lite->START;
		for(size_t i = 0; i < lite->insur.length(); ){
			locstr = "";
			current_length = 0;
			current_Position = Last_Position;
			for(size_t j = i; (j < lite->insur.length()) && (current_length < Maximum_NE_Length); current_length++, current_Position++){
				sChar[0] = lite->insur.c_str()[j++];
				sChar[1] = 0;	
				if(sChar[0] < 0 ){
					sChar[1] = lite->insur.c_str()[j++];
				}
				locstr += sChar;
				ACE_extent negentent;
				negentent.charseq = locstr;
				negentent.START = Last_Position;
				negentent.END = current_Position;
				lite->Negetive_NE_v.push_back(negentent);
			}
			if(lite->insur.c_str()[i++] < 0){
				i++;
			}
			Last_Position++;
		}
	}
	for(vector<NE_Surround>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end();lite++){
		for(vector<ACE_entity_mention>::iterator pvite = lite->entity_mention_v.begin(); pvite != lite->entity_mention_v.end(); pvite++){
			for(vector<ACE_extent>::iterator nvite = lite->Negetive_NE_v.begin();  nvite != lite->Negetive_NE_v.end(); ){
				if((pvite->extent.START == nvite->START) && (pvite->extent.END == nvite->END)){
					nvite =	lite->Negetive_NE_v.erase(nvite);
				}
				else{
					nvite++;
				}
			}
		}
	}
}


void CEnsem::Case_Generation_Per_EN_Per_Sentence(const char* NE_Cases_Path, ACE_Corpus& ACE_Corpus, list<Named_Case>& Named_Case_l)
{
	map<string, ACE_entity>& ACE_Entity_Info_map = ACE_Corpus.ACE_Entity_Info_map;
	map<string, ACE_sgm>& ACE_sgm_mmap = ACE_Corpus.ACE_sgm_mmap;

	map<string, list<pair<string, pair<size_t,size_t>>>> ACE_DocSentence_map;//<DOCID, list_pair: <Sentence, (START, ENd);>
	map<string, vector<pair<ACE_extent, ACE_entity_mention*>>> EntityMentionInfo_map;//Save each entity mention info in doc.

	Named_Case_l.clear();
	//------------------------------------------------------------------------
	ace_op::Segment_ACE_sgm_to_Sentence(ACE_sgm_mmap, ACE_DocSentence_map);
	//------------------------------------------------------------------------
	//Extract each entity mention info in doc.
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(EntityMentionInfo_map.find(vite->DOCID) == EntityMentionInfo_map.end()){
				EntityMentionInfo_map[vite->DOCID];
			}
			EntityMentionInfo_map[vite->DOCID].push_back(make_pair(vite->extent, &(*vite)));
		}
	}
	//------------------------------------------------------------------------
	//<DOCID, list_pair: <Sentence, <START, ENd> >
	ACE_entity_mention* ne_Enitity;
	pair<size_t, size_t> FirstMentionExtent;
	for(map<string, list<pair<string, pair<size_t,size_t>>>>::iterator mite = ACE_DocSentence_map.begin(); mite != ACE_DocSentence_map.end(); mite++){

		string PrixSentStr = "";
		string ProxSentStr = "";

		list<pair<string, pair<size_t,size_t>>>::iterator next_ite;
		for(list<pair<string, pair<size_t,size_t>>>::iterator sgmlite = mite->second.begin(); sgmlite != mite->second.end(); sgmlite++){
			
			vector<pair<ACE_extent, ACE_entity_mention*>>& entityref_v = EntityMentionInfo_map[mite->first];//Entity mention vectior in doc;
			for(vector<pair<ACE_extent, ACE_entity_mention*>>::iterator ne_ite = entityref_v.begin(); ne_ite != entityref_v.end(); ne_ite++){
				//Check wether the entity set is accordance with original doc;
				if(strcmp(ne_ite->first.charseq.c_str(), Sentop::Get_Substr_by_Chinese_Character_Cnt(ACE_sgm_mmap[mite->first].DOC.c_str(), ne_ite->first.START, ne_ite->first.END + 1 - ne_ite->first.START).c_str())){
					ostringstream ostream;
					ostream << "文档：" << mite->first << endl << "位置：" << ne_ite->first.START<< endl<< "命名实体：" << ne_ite->first.charseq << endl;
					ostream << "文档字符：" << Sentop::Get_Substr_by_Chinese_Character_Cnt(ACE_sgm_mmap[mite->first].DOC.c_str(), ne_ite->first.START, ne_ite->first.END + 1 - ne_ite->first.START) << endl;
					if(IDYES != AppCall::Secretary_Message_Box(ostream.str(), MB_YESNOCANCEL)){
						return;
					}
					continue;
				}
				if((sgmlite->second.first  <= ne_ite->first.START) && (sgmlite->second.second >= ne_ite->first.END)){
					ne_Enitity = ne_ite->second;
					
					Named_Case loc_Named_Case;
					loc_Named_Case.DOCID = mite->first;
					//Extracting adjacent Sentence;
					loc_Named_Case.InSentStr = sgmlite->first;
					if(sgmlite != mite->second.end()){
						next_ite = ++sgmlite;
						ProxSentStr = next_ite->first;
						sgmlite--;
					}
					else{
						ProxSentStr = "";
					}
					if(sgmlite != mite->second.begin()){
						next_ite = --sgmlite;
						PrixSentStr = next_ite->first;
						sgmlite++;
					}
					else{
						PrixSentStr = "";
					}
					loc_Named_Case.PrixSentStr = PrixSentStr;
					loc_Named_Case.ProxSentStr = ProxSentStr;
					loc_Named_Case.entity_mention = *(ne_ite->second);

					//Extracting Adjacent Fragment;
					loc_Named_Case.InFragStr = ne_ite->second->extent.charseq;
					loc_Named_Case.PrixFragStr = Sentop::Get_Substr_by_Chinese_Character_Cnt(loc_Named_Case.InSentStr.c_str(), 0, ne_ite->second->extent.START - sgmlite->second.first);
					loc_Named_Case.ProxFragStr = Sentop::Get_Substr_by_Chinese_Character_Cnt(loc_Named_Case.InSentStr.c_str(), ne_ite->second->extent.END - sgmlite->second.first + 1, -1);

					loc_Named_Case.Sent_START = sgmlite->second.first;
					loc_Named_Case.Sent_END = sgmlite->second.second;
					//Delet_0AH_and_20H_in_Named_Entity_Cases(loc_Named_Case);
					Named_Case_l.push_back(loc_Named_Case);
				}
			}//ne_ite
		}//for(list<pair
	}//for(map<string,
	//Save_Named_Entity_Cases(NE_Cases_Path);
	Named_Case_l.clear();
}
