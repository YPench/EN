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
#include "ECCom.h"
#include "maxen.h"
#include "ConvertUTF.h"


void ECCOM::Mention_Space_to_Doc_Mention_Set(map<string, ACE_entity_mention*>& pmMentionSpace_m, map<string, set<string>>& pmDocMentionsSet_ms)
{


}

void ECCOM::Get_Mention_Space(map<string, ACE_entity>& ACE_Entity_Info_map, map<string, ACE_entity_mention*>& MentionSpace_m)
{
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(MentionSpace_m.find(vite->ID) != MentionSpace_m.end()){
				AppCall::Secretary_Message_Box("Duplicated ACE mentions in: ECCOM::Get_Mention_Space()");
			}
			MentionSpace_m.insert(make_pair(vite->ID, &*vite));
		}
	}
}
bool ECCOM::Nested_Entity_Mentions(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention)
{
	if(pmF_mention.extent.START >= pmS_mention.extent.START && pmF_mention.extent.END <= pmS_mention.extent.END){
		return true;
	}
	if(pmF_mention.extent.START <= pmS_mention.extent.START && pmF_mention.extent.END >= pmS_mention.extent.END){
		return true;
	}
	return false;
}

void ECCOM::Get_Acronym(vector<string>& words_v, size_t num, char* cAcronymBuf)
{
	char sChar[3];
	sChar[2]=0;
	strcpy_s(cAcronymBuf, num, "");
	for(size_t i = 0; i < words_v.size(); i++){
		if(words_v[i].length() == 0){
			continue;
		}
		sChar[0] = words_v[i].c_str()[0];
		sChar[1] = 0;
		if(sChar[0] < 0 ){
			sChar[1] = words_v[i].c_str()[1];
		}
		strcat_s(cAcronymBuf, num, sChar);
	}
}
char* ECCOM::ToLowerCase(char *str)
{
    register char *s = str;

    while(*s != '\0') {
	if(*s >= 'A' && *s <= 'Z')
	    *s += 32;
	s++;
    }
    return(str);
}
char* ECCOM::prestrcat_s(char *dest, size_t num, const char* src)
{
	size_t destlen = strlen(dest);
	size_t srclen = strlen(src);
	if((destlen+srclen) > num){
		AppCall::Secretary_Message_Box("Data Error in ECCOM::prestrcat_s()");
		return NULL;
	}
	dest[srclen+destlen+1] = 0;
	for(int i = destlen-1; i >=0; i--){
		dest[i+srclen+1] = dest[i];
	}
	dest[srclen] = ' '; 
	for(size_t i = 0; i < srclen; i++){
		dest[i] = src[i];
	}
	//string test = dest;
	return dest;
}
bool ECCOM::words_nested(vector<string>& pmFstr_v, vector<string>& pmSstr_v)
{
	string neststr;
	for(size_t i = 0; i < pmFstr_v.size(); i++){
		size_t j;
		for(j = 0; j < pmSstr_v.size() && i < pmFstr_v.size(); ){
			if(!strcmp(pmFstr_v[i].c_str(), pmSstr_v[j].c_str())){
				i++;
				j++;
			}
			else{
				break;
			}
		}
		if(j == pmSstr_v.size()){
			return true;
		}
	}
	return false;
}


char* ECCOM::ToUpperCase(char *str)
{
    register char *s = str;

    while(*s != '\0') {
	if(*s >= 'a' && *s <= 'z')
	    *s -= 32;
	s++;
    }
    return(str);
}

void ECCOM::English_Token(const char* cstr, vector<string>& output_v){

	size_t size = strlen(cstr);
	char SentBuf[512];
	char sChar[3];
	sChar[2]=0;

	strcpy_s(SentBuf, 512, "");
	for(size_t i = 0; i < size; ){
		sChar[0] = cstr[i++];
		sChar[1] = 0;
		if(sChar[0] < 0 ){
			sChar[1] = cstr[i++];
		}
		if(!strcmp(sChar, " ") || !strcmp(sChar, "\n") || !strcmp(sChar, "\r")){
			if(strlen(SentBuf) != 0){
				output_v.push_back(SentBuf);
				strcpy_s(SentBuf, 512, "");
				continue;
			}
		}
		strcat_s(SentBuf, 512, sChar);
	}
	if(strlen(SentBuf) != 0){
		output_v.push_back(SentBuf);
	}
}



void ECCOM::Document_Entity_Mention_Ording(vector<ACE_entity_mention*>& pmEntityMeniton_v)
{
	map<size_t, multimap<size_t, ACE_entity_mention*>> loc_Ordering_mm;
	char cDOCID[256];
	if(!pmEntityMeniton_v.empty()){
		strcpy_s(cDOCID, 256, (*pmEntityMeniton_v.begin())->DOCID.c_str());
	}
	for(size_t i = 0; i < pmEntityMeniton_v.size(); i++){
		if(strcmp(cDOCID, pmEntityMeniton_v[i]->DOCID.c_str())){
			AppCall::Secretary_Message_Box("Data Error in: ECCOM::Document_Entity_Mention_Ording()");
			return;
		}
		ACE_extent& ref_extent = pmEntityMeniton_v[i]->extent;
		if(loc_Ordering_mm.find(ref_extent.START) == loc_Ordering_mm.end()){
			loc_Ordering_mm[ref_extent.START];
		}
		loc_Ordering_mm[ref_extent.START].insert(make_pair(ref_extent.END, pmEntityMeniton_v[i]));

	}
	pmEntityMeniton_v.clear();
	for(map<size_t, multimap<size_t, ACE_entity_mention*>>::iterator mmite = loc_Ordering_mm.begin(); mmite != loc_Ordering_mm.end(); mmite++){
		for(multimap<size_t, ACE_entity_mention*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
			pmEntityMeniton_v.push_back(mite->second);
		}
	}
}

void ECCOM::ACE_Corpus_to_Document_Entity_Vector_Point(ACE_Corpus& pmACE_Corpus, map<string, vector<ACE_entity*>*>& DocEntityVectorP_m)
{
	map<string, ACE_entity>& ACE_Entity_Info_map = pmACE_Corpus.ACE_Entity_Info_map;
	map<string, ACE_DocInfo>& ACE_DocInfo_m = pmACE_Corpus.ACE_DocInfo_m;
	for(map<string, ACE_entity>::iterator mmite = ACE_Entity_Info_map.begin(); mmite != ACE_Entity_Info_map.end(); mmite++){
		if(DocEntityVectorP_m.find(mmite->second.DOCID) == DocEntityVectorP_m.end()){
			DocEntityVectorP_m.insert(make_pair(mmite->second.DOCID, new vector<ACE_entity*>));
		}
		DocEntityVectorP_m[mmite->second.DOCID]->push_back(&mmite->second);
	}
}
void ECCOM::Get_Docs_Entity_Mentions(ACE_Corpus& pmACE_Corpus, vector<string>& pmCanditDoc_v, map<string, vector<ACE_entity_mention*>*>& DocsMentons_v)
{
	map<string, ACE_entity>& ACE_Entity_Info_map = pmACE_Corpus.ACE_Entity_Info_map;
	map<string, ACE_DocInfo>& ACE_DocInfo_m = pmACE_Corpus.ACE_DocInfo_m;
	set<string> Doc_s;
	for(size_t i = 0; i < pmCanditDoc_v.size(); i++){
		Doc_s.insert(pmCanditDoc_v[i]);
	}

	for(map<string, ACE_entity>::iterator mmite = ACE_Entity_Info_map.begin(); mmite != ACE_Entity_Info_map.end(); mmite++){
		if((Doc_s.find(mmite->second.DOCID) == Doc_s.end()) && !Doc_s.empty()){
			continue;
		}

		if(DocsMentons_v.find(mmite->second.DOCID) == DocsMentons_v.end()){
			DocsMentons_v.insert(make_pair(mmite->second.DOCID, new vector<ACE_entity_mention*>));
		}
		for(vector<ACE_entity_mention>::iterator mite = mmite->second.entity_mention_v.begin(); mite != mmite->second.entity_mention_v.end(); mite++){
			DocsMentons_v[mmite->second.DOCID]->push_back(&(*mite));
		}
	}
}


void ECCOM::Get_Docs_Related_Mentions(ACE_Corpus& pmACE_Corpus, map<string, map<string, set<string>>>& DocsRelatedMentions_mms)
{
	vector<ACE_relation>& ACE_Relation_Info_v = pmACE_Corpus.ACE_Relation_Info_v;

	for(size_t i = 0; i < ACE_Relation_Info_v.size(); i++){
		vector<ACE_relation_mention>& relation_mention_v = ACE_Relation_Info_v[i].relation_mention_v;
		if(DocsRelatedMentions_mms.find(ACE_Relation_Info_v[i].DOCID) == DocsRelatedMentions_mms.end()){
			DocsRelatedMentions_mms[ACE_Relation_Info_v[i].DOCID];
		}
		map<string, set<string>>& RelMention_ms = DocsRelatedMentions_mms[ACE_Relation_Info_v[i].DOCID];
		for(size_t j = 0; j < relation_mention_v.size(); j++){
			multimap<string, ACE_relation_mention_arg>& arg_mention_mmap = relation_mention_v[j].arg_mention_mmap;
			string REFID_1 = "";
			string REFID_2 = "";
			for(multimap<string, ACE_relation_mention_arg>::iterator emvite = arg_mention_mmap.begin();  emvite != arg_mention_mmap.end(); emvite++){
				if(!strcmp("Arg-1", emvite->first.c_str())){
					REFID_1 = emvite->second.REFID;
				}
				if(!strcmp("Arg-2", emvite->first.c_str())){
					REFID_2 = emvite->second.REFID;
				}
			}
			if(strlen(REFID_1.c_str()) == 0 || strlen(REFID_2.c_str()) == 0){
				AppCall::Secretary_Message_Box("Data Error in: ECCOM::Get_Docs_Related_Mentions()");
			}
			if(RelMention_ms.find(REFID_1) == RelMention_ms.end()){
				RelMention_ms[REFID_1];
			}
			if(RelMention_ms[REFID_1].find(REFID_2) == RelMention_ms[REFID_1].end()){
				RelMention_ms[REFID_1].insert(REFID_2);
			}
			if(RelMention_ms.find(REFID_2) == RelMention_ms.end()){
				RelMention_ms[REFID_2];
			}
			if(RelMention_ms[REFID_2].find(REFID_1) == RelMention_ms[REFID_2].end()){
				RelMention_ms[REFID_2].insert(REFID_1);
			}
		}
	}
}





void ECCOM::Output_Coreference_Doc(const char* FilePath, vector<ACE_entity*>& DocEntity_v, ACE_DocInfo& pmDocInfo)
{

	ofstream out(FilePath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	//ostream.str("");
	out << "<?xml version=\"1.0\"?>" << endl;
	out << "<!DOCTYPE source_file SYSTEM \"apf.v5.1.1.dtd\">" << endl;
	out << "<source_file URI=\"" << pmDocInfo.URI << "\" SOURCE=\"" << pmDocInfo.SOURCE << "\" TYPE=\"" << pmDocInfo.TYPE;
	out << "\" AUTHOR=\"" << pmDocInfo.AUTHOR << "\" ENCODING=\"" << pmDocInfo.ENCODING << "\">" << endl;
	out << "<document DOCID=\"" << pmDocInfo.DOCID << "\">" << endl;

	//ASCstr = ostream.str();
	//ASCwstr = SCONVERT::StringToWString(ASCstr);
	//out << SCONVERT::ToUtf8(ASCwstr);


	for(vector<ACE_entity*>::iterator envite = DocEntity_v.begin(); envite != DocEntity_v.end(); envite++){
		ACE_entity& ref_entity = **envite;
		out << "<entity ID=\"" << ref_entity.ID << "\" TYPE=\"" << ref_entity.TYPE << "\" SUBTYPE=\"" << ref_entity.SUBTYPE << "\" CLASS=\"" << ref_entity.CLASS << "\">" << endl;
		for(vector<ACE_entity_mention>::iterator menmite = ref_entity.entity_mention_v.begin(); menmite != ref_entity.entity_mention_v.end(); menmite++){	
			out << "  <entity_mention ID=\"" << menmite->ID << "\" TYPE=\"" << menmite->TYPE <<  "\" LDCTYPE=\"" << menmite->LDCTYPE;
			if(!menmite->LDCATR.empty()){
				out << "\" LDCATR=\"" << menmite->LDCATR;
			}
			out << "\">" << endl;
			out << "    <extent>" << endl;
			out << "      <charseq START=\"" << menmite->extent.START << "\" END=\"" << menmite->extent.END << "\">" << menmite->extent.charseq << "</charseq>" << endl;
			out << "    </extent>" << endl;
			out << "    <head>" << endl;
			out << "      <charseq START=\"" << menmite->head.START << "\" END=\"" << menmite->head.END << "\">" << menmite->head.charseq << "</charseq>" << endl;
			out << "    </head>" << endl;
			out << "  </entity_mention>" << endl;
		}
		out << "</entity>" << endl;
	}
	out << "</document>" << endl;
	out << "</source_file>" << endl;

	out.close();
}

void ECCOM::ACE_Perl_Evaluation(const char* cparameters){

	ostringstream parameterstream;
	parameterstream << "F:\\YPench\\ConsensusGraph\\EXE\\Perl_Eval\\Eval.bat";

	wchar_t awParameters[256];
	SCONVERT::AnsiToUnicode(parameterstream.str().c_str(), awParameters);

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpDirectory = _T(ACE_Eval_FOLDER);
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShExecInfo.lpFile = _T("cmd.exe");

	ShExecInfo.lpParameters = awParameters;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

void ECCOM::Greedy_SubTYPE_Matching_Method(map<string, ACE_entity_mention*>& MentionSpace_m, CoMentionPairVector& CoCandit_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm)
{
	map<string, map<size_t, map<size_t, ACE_entity_mention*>>> GlobalMentionInfo_mm;
	for(CoMPairItor vite = CoCandit_v.begin(); vite != CoCandit_v.end(); vite++){
		//First Mention
		ACE_entity_mention& F_mention = *vite->first;
		ACE_entity_mention& S_mention = *vite->second;
		if(strcmp(F_mention.DOCID.c_str(), S_mention.DOCID.c_str())){
			AppCall::Secretary_Message_Box("Data Error in: Greedy_SubTYPE_Matching_Method(1)...", MB_OK);
			continue;
		}
		string DOCID = F_mention.DOCID.c_str();
		if(GlobalMentionInfo_mm.find(DOCID) == GlobalMentionInfo_mm.end()){
			GlobalMentionInfo_mm[DOCID];
		}
		map<size_t, map<size_t, ACE_entity_mention*>>& Loc_Coord_mm = GlobalMentionInfo_mm[DOCID];
		ACE_extent& F_extend = F_mention.extent;
		ACE_extent& S_extend = S_mention.extent; 

		if(Loc_Coord_mm.find(F_extend.START) == Loc_Coord_mm.end()){
			Loc_Coord_mm[F_extend.START];
		}
		if(Loc_Coord_mm[F_extend.START].find(F_extend.END) == Loc_Coord_mm[F_extend.START].end()){
			Loc_Coord_mm[F_extend.START].insert(make_pair(F_extend.END, vite->first));
		}
		//Second Extend
		if(Loc_Coord_mm.find(S_extend.START) == Loc_Coord_mm.end()){
			Loc_Coord_mm[S_extend.START];
		}
		if(Loc_Coord_mm[S_extend.START].find(S_extend.END) == Loc_Coord_mm[S_extend.START].end()){
			Loc_Coord_mm[S_extend.START].insert(make_pair(S_extend.END, vite->second));
		}
	}
	//-----------------------------Gready Matching-------------------------------
	vector<bool> CoreCanditTYPE_v;
	for(CoMPairItor vite = CoCandit_v.begin(); vite != CoCandit_v.end(); vite++){
		if(strcmp(vite->first->DOCID.c_str(), vite->second->DOCID.c_str())){
			CoreCanditTYPE_v.push_back(false);
			continue;
		}
		if(vite->first->extent.START > vite->second->extent.START){
			ACE_entity_mention* ptemp = vite->first;
			vite->first = vite->second;
			vite->second = ptemp;
		}
		bool Existing_Closer_Flag  = false;
		bool Out_of_Subsequent_Flag = false;
		ACE_entity_mention& F_mention = *vite->first;
		ACE_entity_mention& S_mention = *vite->second;
		map<size_t, map<size_t, ACE_entity_mention*>>& DocMention = GlobalMentionInfo_mm[vite->first->DOCID];
		map<size_t, map<size_t, ACE_entity_mention*>>::iterator fmmite = DocMention.find(vite->first->extent.START);
		map<size_t, map<size_t, ACE_entity_mention*>>::iterator cmmite = DocMention.find(vite->first->extent.START);
		map<size_t, map<size_t, ACE_entity_mention*>>::iterator smmite = DocMention.find(vite->second->extent.START);
		if(smmite == DocMention.end()){
			AppCall::Secretary_Message_Box("Data Error in: Greedy_SubTYPE_Matching_Method(2)..", MB_OK);
		}
		ACE_entity_mention& first_mention = *fmmite->second.begin()->second;
		for( ; true; cmmite++){
			map<size_t, ACE_entity_mention*>::iterator smite = smmite->second.find(S_mention.extent.END);
			for(map<size_t, ACE_entity_mention*>::iterator cmite = cmmite->second.begin(); cmite != cmmite->second.end(); cmite++){
				if(!strcmp(first_mention.ID.c_str(), cmite->second->ID.c_str())){
					continue;
				}
				ACE_entity_mention& loc_mention = *cmite->second;
				//----------found himself------------
				if((cmmite->first == smmite->first) && (cmite->first == smite->first)){
					Out_of_Subsequent_Flag = true;
					break;
				}
				//------------Existing Closer Mention-----------------------
				if(!strcmp(loc_mention.Entity_SUBSTYPE.c_str(),S_mention.Entity_SUBSTYPE.c_str())){
					Existing_Closer_Flag = true;
					break;
				}
			}
			if(Existing_Closer_Flag || Out_of_Subsequent_Flag){
				break;
			}
			if(cmmite == DocMention.end()){
				AppCall::Secretary_Message_Box("Data Error in: Greedy_SubTYPE_Matching_Method(3)..", MB_OK);
				break;
			}
		}
		if(Existing_Closer_Flag){
			CoreCanditTYPE_v.push_back(false);
		}
		else{
			CoreCanditTYPE_v.push_back(true);
		}
	}
	//------------------Partition Coreference------------------------------
	if(CoreCanditTYPE_v.size() != CoCandit_v.size()){
		AppCall::Secretary_Message_Box("Data Error: CoreCanditTYPE_v.size() != CoCandit_v.size(4)...", MB_OK);
	}
	map<string, map<size_t, map<size_t, pair<string, set<ACE_entity_mention*>*>>>> CoferenceSetCoord_mm;
	size_t EntityID = 0;
	char intchar[64];
	set<string> CollectedMention_s;
	for(size_t i = 0; i < CoreCanditTYPE_v.size(); i++){
		if(!CoreCanditTYPE_v[i]){
			continue;
		}
		string DOCID = CoCandit_v[i].first->DOCID;
		ACE_extent& FExtend = CoCandit_v[i].first->extent;
		ACE_extent& SExtend = CoCandit_v[i].second->extent;
		if(CoferenceSetCoord_mm.find(DOCID) == CoferenceSetCoord_mm.end()){
			CoferenceSetCoord_mm[DOCID];
		}
		map<size_t, map<size_t, pair<string, set<ACE_entity_mention*>*>>>& CoordSet_mm = CoferenceSetCoord_mm[DOCID];
		if(CoordSet_mm.find(FExtend.START) == CoordSet_mm.end()){
			CoordSet_mm[FExtend.START];
		}
		if(CoordSet_mm[FExtend.START].find(FExtend.END) == CoordSet_mm[FExtend.START].end()){
			CoordSet_mm[FExtend.START][FExtend.END];
			_itoa_s(EntityID++,intchar,64,10);
			CoordSet_mm[FExtend.START][FExtend.END].first = intchar;
			CoordSet_mm[FExtend.START][FExtend.END].second = new set<ACE_entity_mention*>;
			CoordSet_mm[FExtend.START][FExtend.END].second->insert(CoCandit_v[i].first);
			CollectedMention_s.insert(CoCandit_v[i].first->ID);
		}
		pair<string, set<ACE_entity_mention*>*>& RefEntitySet = CoordSet_mm[FExtend.START][FExtend.END];
		//set<ACE_entity_mention*>& FormerMentionSet = *CoordSet_mm[FExtend.START][FExtend.END].second;
		//-------------Insert Second Mention;
		if(CoordSet_mm.find(SExtend.START) == CoordSet_mm.end()){
			CoordSet_mm[SExtend.START];
		}
		if(CoordSet_mm[SExtend.START].find(SExtend.END) == CoordSet_mm[SExtend.START].end()){
			CoordSet_mm[SExtend.START][SExtend.END];
			CoordSet_mm[SExtend.START][SExtend.END].first = RefEntitySet.first;
			CoordSet_mm[SExtend.START][SExtend.END].second = RefEntitySet.second;
		}
		RefEntitySet.second->insert(CoCandit_v[i].second);
		CollectedMention_s.insert(CoCandit_v[i].second->ID);
	}
	for(set<string>::iterator site = CollectedMention_s.begin(); site != CollectedMention_s.end(); site++){
		if(MentionSpace_m.find(site->data()) != MentionSpace_m.end()){
			MentionSpace_m.erase(site->data());
		}
	}
	if(!MentionSpace_m.empty()){
		for(map<string, ACE_entity_mention*>::iterator mite = MentionSpace_m.begin(); mite != MentionSpace_m.end(); mite++){
			if(CoferenceSetCoord_mm.find(mite->second->DOCID) == CoferenceSetCoord_mm.end()){
				CoferenceSetCoord_mm[mite->second->DOCID];
			}
			map<size_t, map<size_t, pair<string, set<ACE_entity_mention*>*>>>& CoordSet_mm = CoferenceSetCoord_mm[mite->second->DOCID];
			ACE_extent& loc_extent = mite->second->extent;
			if(CoordSet_mm.find(loc_extent.START) == CoordSet_mm.end()){
				CoordSet_mm[loc_extent.START];
			}
			if(CoordSet_mm[loc_extent.START].find(loc_extent.END) == CoordSet_mm[loc_extent.START].end()){
				CoordSet_mm[loc_extent.START][loc_extent.END];
			}
			else{
				AppCall::Secretary_Message_Box("Data Error in: Greedy_SubTYPE_Matching_Method(5)..");
			}
			_itoa_s(EntityID++,intchar,64,10);
			CoordSet_mm[loc_extent.START][loc_extent.END].first = intchar;
			CoordSet_mm[loc_extent.START][loc_extent.END].second = new set<ACE_entity_mention*>;
			CoordSet_mm[loc_extent.START][loc_extent.END].second->insert(mite->second);
		}
	}

	map<string, map<size_t, map<size_t, pair<string, set<ACE_entity_mention*>*>>>>::iterator mmmpsite = CoferenceSetCoord_mm.begin();
	for( ; mmmpsite != CoferenceSetCoord_mm.end(); mmmpsite++){
		if(CoferenceSet_mm.find(mmmpsite->first) == CoferenceSet_mm.end()){
			CoferenceSet_mm[mmmpsite->first];
		}
		else{
			AppCall::Secretary_Message_Box("Data Error in: Greedy_SubTYPE_Matching_Method(6)..");
		}
		for(map<size_t, map<size_t, pair<string, set<ACE_entity_mention*>*>>>::iterator mmpsite = mmmpsite->second.begin(); mmpsite != mmmpsite->second.end(); mmpsite++){
			for(map<size_t, pair<string, set<ACE_entity_mention*>*>>::iterator mpsite = mmpsite->second.begin(); mpsite != mmpsite->second.end(); mpsite++){
				if(CoferenceSet_mm[mmmpsite->first].find(mpsite->second.first) == CoferenceSet_mm[mmmpsite->first].end()){
					CoferenceSet_mm[mmmpsite->first].insert(make_pair(mpsite->second.first, mpsite->second.second));
				}
			}
		}
	}
}

