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
#include "Antical.h"
#include "NECom.h"
#include <algorithm>
#include "CEDT.h"
#include "ConvertUTF.h"

//#include "EXTERN.h"     //引用外部的头文件
//#include "perl.h"       //perl头文件

extern bool CEDT_Head_Flag;
extern bool CEDT_Detection_Flag;
extern bool CEDT_Extend_Done_Flag;
extern bool CEDT_Head_Done_Flag;

CAntical::CAntical()
{
}
CAntical::~CAntical()
{
}
void CAntical::ACE_Named_Entity_Recognition_Evaluation_Port(vector<NE_Surround*>& Surround_v, vector<DismCase*>& pmDismCase_v)
{

	Output_For_ACE_Named_Entity_Recognition_Evaluation(Surround_v, pmDismCase_v);

	//string toolkitfolder = DATA_FOLDER;
	//toolkitfolder += "CEDT\\";
	//ACE_Evaluation_Toolkit_Call(toolkitfolder.c_str());
}
void CAntical::Corpus_Entity_and_Entity_Mention_Information(vector<NE_Surround*>& Surround_v, bool Output_Flag)
{
	static size_t EnityCnt = 0;
	static size_t MentionCnt = 0;
	ostringstream ostream;

	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		NE_Surround& loc_Surround = **ne_vite;
		for(size_t j = 0; j < loc_Surround.entity_mention_v.size(); j++){
			ACE_extent& locExtend= loc_Surround.entity_mention_v[j].extent;
			map<size_t, set<size_t>> Traveled_m;
			if(Traveled_m.find(locExtend.START) == Traveled_m.end()){
				Traveled_m[locExtend.START];
			}
			if(Traveled_m[locExtend.START].find(locExtend.END) == Traveled_m[locExtend.START].end()){
				MentionCnt++;
			}
		}
	}
	if(Output_Flag){
		ostream << "mention number: " << MentionCnt << endl;
		AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
	}
}

void CAntical::Output_For_ACE_Named_Entity_Recognition_Evaluation(vector<NE_Surround*>& Surround_v, vector<DismCase*>& pmDismCase_v)
{
	if(!CEDT_Extend_Done_Flag || !CEDT_Head_Done_Flag){
		//AppCall::Secretary_Message_Box("Missing data, CAntical::Output_For_ACE_Named_Entity_Recognition_Evaluation() can not be done.", MB_OK);
		return;
	}

	ostringstream ostream;
	wchar_t wContent[MAX_SENTENCE];
	char UTF8Buffer[MAX_SENTENCE];
	string savepath;
	size_t EntityID;
	map<size_t, size_t> SENID_offset_m;

	size_t OffsetCnt = 0;
	map<size_t, vector<ACE_entity_mention>*> EMVector_m;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		OffsetCnt += NLPOP::Get_Chinese_Sentence_Length_Counter((*ne_vite)->insur.c_str());
		SENID_offset_m.insert(make_pair(SENID, OffsetCnt));
		EMVector_m.insert(make_pair(SENID, &(*ne_vite)->entity_mention_v));
	}

	savepath = DATA_FOLDER;
	savepath += "CEDT\\Perl_Eval\\input.xml";
	SCONVERT::AnsiToUnicode(savepath.c_str(), wContent);
	CFile OutputFile(wContent, CFile::modeCreate | CFile::modeReadWrite);  
    ASSERT(OutputFile.m_hFile!=NULL);

	EntityID = 0;
	ostream.str("");
	ostream << "<?xml version=\"1.0\"?>" << endl;
	ostream << "<!DOCTYPE source_file SYSTEM \"apf.v5.1.1.dtd\">" << endl;
	ostream << "<source_file URI=\"000000.sgm\" SOURCE=\"000000\" TYPE=\"text\" AUTHOR=\"LDC\" ENCODING=\"UTF-8\">" << endl;
	ostream << "<document DOCID=\"000000\">" << endl;
	
	SCONVERT::AnsiToUnicode(ostream.str().c_str(), wContent);
	SCONVERT::UnicodeToUTF8(wContent, UTF8Buffer);
	OutputFile.Write(UTF8Buffer, strlen(UTF8Buffer));

	for(map<size_t, vector<ACE_entity_mention>*>::iterator mvite = EMVector_m.begin(); mvite != EMVector_m.end(); mvite++){
		size_t offset = 0;
		if(SENID_offset_m.find(mvite->first) == SENID_offset_m.end()){
			AppCall::Secretary_Message_Box("Unmatched data in CAntical::ACE_Named_Entity_Recognition_Output_Result()...", MB_OK);
		}
		else{
			offset = SENID_offset_m[mvite->first];
		}
		for(size_t i = 0; i < mvite->second->size(); i++){
			ACE_entity_mention& mentionref = (*mvite->second)[i];
			ACE_extent& ExtentRef = mentionref.extent;
			ACE_entity_head& HeadRef = mentionref.head;
			//if(strcmp(mentionref.Entity_CLASS.c_str(), "SPC")){
			//	continue;
			//}
			ostream.str("");
			ostream << "<entity ID=\"" << "E" << EntityID << "\" TYPE=\"" << mentionref.Entity_TYPE << "\" SUBTYPE=\"" << mentionref.Entity_SUBSTYPE <<  "\" CLASS=\"" << mentionref.Entity_CLASS << "\">" << endl;
			ostream << "  <entity_mention ID=\"" << "E" << EntityID << "-1" << "\" TYPE=\"" << mentionref.TYPE << "\">" << endl;
			ostream << "    <extent>" << endl;
			ostream << "      <charseq START=\"" << ExtentRef.START+offset << "\" END=\"" << ExtentRef.END+offset << "\">" << ExtentRef.charseq << "</charseq>" << endl;
			ostream << "    </extent>" << endl;
			ostream << "    <head>" << endl;
			ostream << "      <charseq START=\"" << HeadRef.START+offset << "\" END=\"" << HeadRef.END+offset << "\">" << HeadRef.charseq << "</charseq>" << endl;
			ostream << "    </head>" << endl;
			ostream << "  </entity_mention>" << endl;
			ostream << "</entity>" << endl;

			SCONVERT::AnsiToUnicode(ostream.str().c_str(), wContent);
			SCONVERT::UnicodeToUTF8(wContent, UTF8Buffer);
			OutputFile.Write(UTF8Buffer, strlen(UTF8Buffer));
			EntityID++;
		}
	}
	ostream.str("");
	ostream << "</document>" << endl;
	ostream << "</source_file>" << endl;

	SCONVERT::AnsiToUnicode(ostream.str().c_str(), wContent);
	SCONVERT::UnicodeToUTF8(wContent, UTF8Buffer);
	OutputFile.Write(UTF8Buffer, strlen(UTF8Buffer));

	OutputFile.Close();

//-------------------------------------------------------------------------------------------------------------------------------
	savepath = DATA_FOLDER;
	savepath += "CEDT\\Perl_Eval\\output.xml";
	SCONVERT::AnsiToUnicode(savepath.c_str(), wContent);
	OutputFile.Open(wContent, CFile::modeCreate | CFile::modeReadWrite);  
    ASSERT(OutputFile.m_hFile!=NULL);

	EntityID = 0;
	ostream.str("");
	ostream << "<?xml version=\"1.0\"?>" << endl;
	ostream << "<!DOCTYPE source_file SYSTEM \"apf.v5.1.1.dtd\">" << endl;
	ostream << "<source_file URI=\"000000.sgm\" SOURCE=\"000000\" TYPE=\"text\" AUTHOR=\"LDC\" ENCODING=\"UTF-8\">" << endl;
	ostream << "<document DOCID=\"000000\">" << endl;
	
	SCONVERT::AnsiToUnicode(ostream.str().c_str(), wContent);
	SCONVERT::UnicodeToUTF8(wContent, UTF8Buffer);
	OutputFile.Write(UTF8Buffer, strlen(UTF8Buffer));

	string Entity_SUBTYPE;
	string Enitiy_CLASS;
	string Mention_TYPE;

	for(size_t i = 0; i < pmDismCase_v.size(); i++){
		DismCase& locDism = *pmDismCase_v[i];
		CanditCase& locHead= *locDism.pHead;
		CanditCase& locExtend = *locDism.pExtend;

		size_t offset = 0;
		if(SENID_offset_m.find(locDism.SENID) == SENID_offset_m.end()){
			AppCall::Secretary_Message_Box("Unmatched data in CAntical::ACE_Named_Entity_Recognition_Output_Result()...", MB_OK);
		}
		else{
			offset = SENID_offset_m[locDism.SENID];
		}
		Enitiy_CLASS = locDism.Entity_CLASS.second.first;//"SPC";//

		ostream.str("");
		ostream << "<entity ID=\"" << "E" << EntityID << "\" TYPE=\"" << locDism.Entity_TYPE.second.first << "\" SUBTYPE=\"" << locDism.Entity_SUBTYPE.second.first << "\" CLASS=\"" << Enitiy_CLASS  << "\">" << endl;
		ostream << "  <entity_mention ID=\"" << "E" << EntityID << "-1" << "\" TYPE=\"" << locDism.TYPE.second.first << "\">" << endl;
		ostream << "    <extent>" << endl;
		ostream << "      <charseq START=\"" << locExtend.START+offset << "\" END=\"" << locExtend.END+offset << "\">" << locExtend.mention << "</charseq>" << endl;
		ostream << "    </extent>" << endl;
		ostream << "    <head>" << endl;
		ostream << "      <charseq START=\"" << locHead.START+offset << "\" END=\"" << locHead.END+offset << "\">" << locHead.mention << "</charseq>" << endl;
		ostream << "    </head>" << endl;
		ostream << "  </entity_mention>" << endl;
		ostream << "</entity>" << endl;

		SCONVERT::AnsiToUnicode(ostream.str().c_str(), wContent);
		SCONVERT::UnicodeToUTF8(wContent, UTF8Buffer);
		OutputFile.Write(UTF8Buffer, strlen(UTF8Buffer));

		EntityID++;
	}
	ostream.str("");
	ostream << "</document>" << endl;
	ostream << "</source_file>" << endl;
	SCONVERT::AnsiToUnicode(ostream.str().c_str(), wContent);
	SCONVERT::UnicodeToUTF8(wContent, UTF8Buffer);
	OutputFile.Write(UTF8Buffer, strlen(UTF8Buffer));

	OutputFile.Close();

	savepath = DATA_FOLDER;
	savepath += "CEDT\\Eval.txt";
	DeleteFile(NLPOP::string2CString(savepath));
}

void CAntical::ACE_Evaluation_Toolkit_Call(const char* toolkitfolder)
{
	/*ostringstream ostream;
	ostream.str("");
	ostream << toolkitfolder << "ace08-eval-v17.pl";

	char *embedding[] = {"", "F:\\YPench\\ConsensusGraph\\Data\\CEDT\\ace08-eval-v17.pl", "-R input.xml -T output.xml"};//perl脚本和参数，第一个为空，没有空格，//第二个为脚本文件，第三个为脚本的输入参数
	static PerlInterpreter *my_perl;//定义一个perl解释器
	my_perl = perl_alloc();//为解释器分配资源
	perl_construct(my_perl);//创建一个解释器
	perl_parse(my_perl, NULL, 3, embedding, NULL);//perl解析
	perl_run(my_perl);//运行
	perl_destruct(my_perl);//析构解释器
	perl_free(my_perl);//释放解释器*/

	/*wchar_t awToolkitFolder[256];
	wchar_t awOutputFile[256];
	wchar_t awCmdLine[256];
	ostringstream ostream;
	string sOutput(toolkitfolder);
	sOutput += "Evaluation.txt";

	SCONVERT::AnsiToUnicode(toolkitfolder, awToolkitFolder);
	SCONVERT::AnsiToUnicode(sOutput.c_str(), awOutputFile);

	SECURITY_ATTRIBUTES sa={sizeof ( sa ),NULL,TRUE};  
    SECURITY_ATTRIBUTES *psa=NULL; 
    DWORD dwShareMode=FILE_SHARE_READ|FILE_SHARE_WRITE;  
    OSVERSIONINFO osVersion={0};  
    osVersion.dwOSVersionInfoSize =sizeof ( osVersion );  
    if(GetVersionEx(&osVersion)){  
        if(osVersion.dwPlatformId ==VER_PLATFORM_WIN32_NT)  {  
            psa=&sa;  
            dwShareMode|=FILE_SHARE_DELETE;  
        }  
    }

	DeleteFile(NLPOP::string2CString(sOutput));
	

	HANDLE hConsoleRedirect = CreateFile(  
                                awOutputFile,  
                                GENERIC_WRITE,  
                                dwShareMode,  
                                psa,  
                                OPEN_ALWAYS,  
                                FILE_ATTRIBUTE_NORMAL,  
                                NULL );  
    ASSERT(hConsoleRedirect!=INVALID_HANDLE_VALUE ); 

    STARTUPINFO si={sizeof(si)};  
    si.dwFlags =STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;//使用标准柄和显示窗口  
    si.hStdOutput =hConsoleRedirect;//将文件作为标准输出句柄  
    si.wShowWindow =SW_SHOW;  
    PROCESS_INFORMATION pi={0};
	memset(&pi, 0, sizeof(pi));
	
	ostream.str("");
	ostream << toolkitfolder << "ace08-eval-v17.pl" << " " << "-R input.xml -T output.xml";
	SCONVERT::AnsiToUnicode(ostream.str().c_str(), awCmdLine);

	TCHAR cmdline[1024];
	_tcscpy_s(cmdline, 1024, awCmdLine);

	if(CreateProcess(NULL, cmdline, NULL,NULL,TRUE,NULL,NULL, awToolkitFolder,&si,&pi))  { 
        WaitForSingleObject(pi.hProcess, INFINITE);  
        CloseHandle(pi.hProcess);  
        CloseHandle(pi.hThread);
    }  
    CloseHandle (hConsoleRedirect);*/
}


void CAntical::ACE_Named_Entity_Recognition_Output_Result(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v)
{
	map<size_t, size_t> SENID_offset_m;
	size_t OffsetCnt = 0;
	map<size_t, vector<ACE_entity_mention>*> EMVector_m;
	for(vector<NE_Surround*>::iterator ne_vite = testing_Surround_v.begin(); ne_vite != testing_Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		OffsetCnt += NLPOP::Get_Chinese_Sentence_Length_Counter((*ne_vite)->insur.c_str());
		SENID_offset_m.insert(make_pair(SENID, OffsetCnt));
		EMVector_m.insert(make_pair(SENID, &(*ne_vite)->entity_mention_v));
	}
	size_t EntityID;
	//ostringstream ostream;
	//string ASCstr;
	//wstring ASCwstr;
	if(!CEDT_Extend_Done_Flag || !CEDT_Head_Done_Flag){
		//AppCall::Secretary_Message_Box("Missing data, CAntical::ACE_Named_Entity_Recognition_Output_Result() can not be done.", MB_OK);
		return;
	}
//---------------------------------------------------------------
	string savepath = DATA_FOLDER;
	savepath += "CEDT\\";
	savepath += "input.xml";

	ofstream out;
	out.open(savepath.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	EntityID = 0;
	//ostream.str("");
	out << "<?xml version=\"1.0\"?>" << endl;
	out << "<!DOCTYPE source_file SYSTEM \"apf.v5.1.1.dtd\">" << endl;
	out << "<source_file URI=\"000000.sgm\" SOURCE=\"000000\" TYPE=\"text\" AUTHOR=\"LDC\" ENCODING=\"UTF-8\">" << endl;
	out << "<document DOCID=\"000000\">" << endl;
	
	//ASCstr = ostream.str();
	//ASCwstr = SCONVERT::StringToWString(ASCstr);
	//out << SCONVERT::ToUtf8(ASCwstr);

	for(map<size_t, vector<ACE_entity_mention>*>::iterator mvite = EMVector_m.begin(); mvite != EMVector_m.end(); mvite++){
		size_t offset = 0;
		if(SENID_offset_m.find(mvite->first) == SENID_offset_m.end()){
			AppCall::Secretary_Message_Box("Unmatched data in CAntical::ACE_Named_Entity_Recognition_Output_Result()...", MB_OK);
		}
		else{
			offset = SENID_offset_m[mvite->first];
		}
		for(size_t i = 0; i < mvite->second->size(); i++){
			ACE_entity_mention& mentionref = (*mvite->second)[i];

			ACE_extent& ExtentRef = mentionref.extent;
			ACE_entity_head& HeadRef = mentionref.head;

			//if(strcmp(mentionref.Entity_TYPE.c_str(), "WEA")){
			//	continue;
			//}
			//ostream.str("");

			out << "<entity ID=\"" << "E" << EntityID << "\" TYPE=\"" << mentionref.Entity_TYPE << "\" SUBTYPE=\"" << mentionref.Entity_SUBSTYPE << "\" CLASS=\"" << mentionref.Entity_CLASS << "\">" << endl;
			out << "  <entity_mention ID=\"" << "E" << EntityID << "-1" << "\" TYPE=\"" << mentionref.TYPE << "\">" << endl;
			out << "    <extent>" << endl;
			out << "      <charseq START=\"" << ExtentRef.START+offset << "\" END=\"" << ExtentRef.END+offset << "\">" << ExtentRef.charseq << "</charseq>" << endl;
			out << "    </extent>" << endl;
			out << "    <head>" << endl;
			out << "      <charseq START=\"" << HeadRef.START+offset << "\" END=\"" << HeadRef.END+offset << "\">" << HeadRef.charseq << "</charseq>" << endl;
			out << "    </head>" << endl;
			out << "  </entity_mention>" << endl;
			out << "</entity>" << endl;

			//ASCstr = ostream.str();
			//ASCwstr = SCONVERT::StringToWString(ASCstr);
			//out << SCONVERT::ToUtf8(ASCwstr);

			EntityID++;
		}
	}
	//out.str("");
	out << "</document>" << endl;
	out << "</source_file>" << endl;

	//ASCstr = ostream.str();
	//ASCwstr = SCONVERT::StringToWString(ASCstr);
	//out << SCONVERT::ToUtf8(ASCwstr);

	out.close();

	map<string, string> defaultSUBTYPE_m;
	defaultSUBTYPE_m.insert(make_pair("FAC", "Airport"));
	defaultSUBTYPE_m.insert(make_pair("GPE", "Population-Center"));
	defaultSUBTYPE_m.insert(make_pair("LOC", "Region-International"));
	defaultSUBTYPE_m.insert(make_pair("ORG", "Government"));
	defaultSUBTYPE_m.insert(make_pair("PER", "Group"));
	defaultSUBTYPE_m.insert(make_pair("VEH", "Air"));
	defaultSUBTYPE_m.insert(make_pair("WEA", "Projectile"));
	map<string, set<string>> TYPE2SUBTYPE_ms;


	//map<size_t, vector<ACE_entity_mention>*> EMVector_m;
	map<size_t, map<size_t, map<size_t, ACE_entity_mention*>>> HeadRange_mm;
	map<size_t, map<size_t, map<size_t, ACE_entity_mention*>>> ExtendRange_mm;
	for(map<size_t, vector<ACE_entity_mention>*>::iterator mvite = EMVector_m.begin(); mvite != EMVector_m.end(); mvite++){
		if(HeadRange_mm.find(mvite->first) == HeadRange_mm.end()){
			HeadRange_mm[mvite->first];
			ExtendRange_mm[mvite->first];
		}
		map<size_t, map<size_t, ACE_entity_mention*>>& locHeadRange_m = HeadRange_mm[mvite->first];
		map<size_t, map<size_t, ACE_entity_mention*>>& locExtendRange_m = ExtendRange_mm[mvite->first];
		for(vector<ACE_entity_mention>::iterator vite = mvite->second->begin(); vite != mvite->second->end(); vite++){
			ACE_entity_head& headref = (*vite).head;
			if(locHeadRange_m.find(headref.START) == locHeadRange_m.end()){
				locHeadRange_m[headref.START];
			}
			if(locHeadRange_m[headref.START].find(headref.END) == locHeadRange_m[headref.START].end()){
				locHeadRange_m[headref.START].insert(make_pair(headref.END, &*vite));
			}
			ACE_extent& extentref = (*vite).extent;
			if(locExtendRange_m.find(extentref.START) == locExtendRange_m.end()){
				locExtendRange_m[extentref.START];
			}
			if(locExtendRange_m[extentref.START].find(extentref.END) == locExtendRange_m[extentref.START].end()){
				locExtendRange_m[extentref.START].insert(make_pair(extentref.END, &*vite));
			}
			if(TYPE2SUBTYPE_ms.find(vite->Entity_TYPE) == TYPE2SUBTYPE_ms.end()){
				TYPE2SUBTYPE_ms[vite->Entity_TYPE];
			}
			if(TYPE2SUBTYPE_ms[vite->Entity_TYPE].find(vite->Entity_SUBSTYPE) == TYPE2SUBTYPE_ms[vite->Entity_TYPE].end()){
				TYPE2SUBTYPE_ms[vite->Entity_TYPE].insert(vite->Entity_SUBSTYPE);
			}
		}
	}

//---------------------------------------------------------------
	savepath = DATA_FOLDER;
	savepath += "CEDT\\";
	savepath += "output.xml";
	out.open(savepath.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	EntityID = 0;
	//ostream.str("");
	out << "<?xml version=\"1.0\"?>" << endl;
	out << "<!DOCTYPE source_file SYSTEM \"apf.v5.1.1.dtd\">" << endl;
	out << "<source_file URI=\"000000.sgm\" SOURCE=\"000000\" TYPE=\"text\" AUTHOR=\"LDC\" ENCODING=\"UTF-8\">" << endl;
	out << "<document DOCID=\"000000\">" << endl;

	//ASCstr = ostream.str();
	//ASCwstr = SCONVERT::StringToWString(ASCstr);
	//out << SCONVERT::ToUtf8(ASCwstr);

	string Entity_SUBTYPE;
	string Enitiy_CLASS;
	string Mention_TYPE;
	//bool Match_Mention_Flag;
	for(size_t i = 0; i < pmDismCase_v.size(); i++){
		DismCase& locDism = *pmDismCase_v[i];
		CanditCase& locHead= *locDism.pHead;
		CanditCase& locExtend = *locDism.pExtend;

		size_t offset = 0;
		if(SENID_offset_m.find(locDism.SENID) == SENID_offset_m.end()){
			AppCall::Secretary_Message_Box("Unmatched data in CAntical::ACE_Named_Entity_Recognition_Output_Result()...", MB_OK);
		}
		else{
			offset = SENID_offset_m[locDism.SENID];
		}

		//ostream.str("");
		//if(strcmp(locDism.predit_TYPE.c_str(), "WEA")){
		//	continue;
		//}//<< "(" << locDism.org_TYPE << ")"

		Entity_SUBTYPE = defaultSUBTYPE_m[locDism.predit_TYPE];
		Enitiy_CLASS = "SPC";
		Mention_TYPE = "NOM";
		/*Match_Mention_Flag = false;
		if(HeadRange_mm.find(locDism.SENID) != HeadRange_mm.end()){
			map<size_t, map<size_t, ACE_entity_mention*>>& locHeadRange_m = HeadRange_mm[locDism.SENID];
			for(map<size_t, map<size_t, ACE_entity_mention*>>::iterator mmite = locHeadRange_m.begin(); mmite != locHeadRange_m.end(); mmite++){
				for(map<size_t, ACE_entity_mention*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
					if(((locHead.START >= (int)mmite->first) && (locHead.START <= (int)mite->first)) || (((int)mmite->first >= locHead.START) && ((int)mmite->first <= locHead.END))){
						Match_Mention_Flag = true;
						Entity_SUBTYPE = mite->second->Entity_SUBSTYPE;
						Enitiy_CLASS = mite->second->Entity_CLASS;
						Mention_TYPE = mite->second->TYPE;
						break;
					}
				}
				if(Match_Mention_Flag){
					break;
				}
			}
		}
		if(!Match_Mention_Flag && (ExtendRange_mm.find(locDism.SENID) != ExtendRange_mm.end())){
			map<size_t, map<size_t, ACE_entity_mention*>>& locExtendRange_m = ExtendRange_mm[locDism.SENID];
			for(map<size_t, map<size_t, ACE_entity_mention*>>::iterator mmite = locExtendRange_m.begin(); mmite != locExtendRange_m.end(); mmite++){
				for(map<size_t, ACE_entity_mention*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
					if(((locExtend.START >= (int)mmite->first) && (locExtend.START <= (int)mite->first)) || (((int)mmite->first >= locExtend.START) && ((int)mmite->first <= locExtend.END))){
						Match_Mention_Flag = true;
						Entity_SUBTYPE = mite->second->Entity_SUBSTYPE;
						Enitiy_CLASS = mite->second->Entity_CLASS;
						Mention_TYPE = mite->second->TYPE;
						break;
					}
				}
				if(Match_Mention_Flag){
					break;
				}
			}
		}
		if(TYPE2SUBTYPE_ms.find(locDism.predit_TYPE) == TYPE2SUBTYPE_ms.end()){
			AppCall::Secretary_Message_Box("Error: in CAntical::ACE_Named_Entity_Recognition_Output_Result()", MB_OK);
		}
		if(TYPE2SUBTYPE_ms[locDism.predit_TYPE].find(Entity_SUBTYPE) == TYPE2SUBTYPE_ms[locDism.predit_TYPE].end()){
			Entity_SUBTYPE = defaultSUBTYPE_m[locDism.predit_TYPE];
		}*/
			 
		out << "<entity ID=\"" << "E" << EntityID << "\" TYPE=\"" << locDism.predit_TYPE << "\" SUBTYPE=\"" << Entity_SUBTYPE << "\" CLASS=\"" << Enitiy_CLASS << "\">" << endl;
		out << "  <entity_mention ID=\"" << "E" << EntityID << "-1" << "\" TYPE=\"" << Mention_TYPE << "\">" << endl;
		out << "    <extent>" << endl;
		out << "      <charseq START=\"" << locExtend.START+offset << "\" END=\"" << locExtend.END+offset << "\">" << locExtend.mention << "</charseq>" << endl;
		out << "    </extent>" << endl;
		out << "    <head>" << endl;
		out << "      <charseq START=\"" << locHead.START+offset << "\" END=\"" << locHead.END+offset << "\">" << locHead.mention << "</charseq>" << endl;
		out << "    </head>" << endl;
		out << "  </entity_mention>" << endl;
		out << "</entity>" << endl;

		//ASCstr = ostream.str();
		//ASCwstr = SCONVERT::StringToWString(ASCstr);
		//out << SCONVERT::ToUtf8(ASCwstr);

		EntityID++;
	}
	out << "</document>" << endl;
	out << "</source_file>" << endl;

	//ASCstr = ostream.str();
	//ASCwstr = SCONVERT::StringToWString(ASCstr);
	//out << SCONVERT::ToUtf8(ASCwstr);
	out.close();

}
void CAntical::Candidates_Performance_Analysis_Single_Extend_or_Head_Mention(vector<NE_Surround*>& testing_Surround_v, bool CEDT_Head_Flag, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m)
{
	map<string, size_t> Positive_Mentions;
	NECOM::Positvie_Mention_Collection(testing_Surround_v, Positive_Mentions, CEDT_Detection_Flag);

	map<string, pair<size_t, size_t>> Pricision_map;
	map<string, pair<size_t, size_t>> Recall_map;

	vector<CanditCase*>& Candits_v = CEDT_Head_Flag?pCCEDT->m_TestingCEDTInfo.CanditHead_v:pCCEDT->m_TestingCEDTInfo.CanditExtend_v;

	for(size_t i = 0; i < Candits_v.size(); i++){
		CanditCase& locCandit = *Candits_v[i];
		if(Recall_map.find(locCandit.org_TYPE) == Recall_map.end()){
			Recall_map.insert(make_pair(locCandit.org_TYPE, make_pair(0, 0)));
			Pricision_map.insert(make_pair(locCandit.org_TYPE, make_pair(0, 0)));
		}
		if(Recall_map.find(locCandit.predit_TYPE) == Recall_map.end()){
			Recall_map.insert(make_pair(locCandit.predit_TYPE, make_pair(0, 0)));
			Pricision_map.insert(make_pair(locCandit.predit_TYPE, make_pair(0, 0)));
		}
		//------------------------------------------------------------
		if(locCandit.predit_TYPE == locCandit.org_TYPE){
			Recall_map[locCandit.org_TYPE].first++;
			Pricision_map[locCandit.org_TYPE].first++;
		}
		Pricision_map[locCandit.predit_TYPE].second++;
	}
	for(map<string, size_t>::iterator mite = Positive_Mentions.begin(); mite != Positive_Mentions.end(); mite++){
		if(Recall_map.find(mite->first) == Recall_map.end()){
			Recall_map.insert(make_pair(mite->first, make_pair(0, 0)));
			Pricision_map.insert(make_pair(mite->first, make_pair(0, 0)));
		}
		Recall_map[mite->first].second = mite->second;
	}

	MAXEN::Collect_nGross_Performances(Pricision_map, Recall_map, P_nCrossRtn_m, R_nCrossRtn_m);
}
void CAntical::Analysizing_Performance_Evaluation_on_Extend_or_Head_Only_Recognition(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m)
{
	map<string, size_t> Positive_Mentions;
	map<size_t, map<size_t, map<size_t, ACE_extent*>>*> HeadExtend_mm;

	NECOM::Positvie_Mention_Collection(testing_Surround_v, Positive_Mentions, CEDT_Detection_Flag);
	NECOM::Building_Head_to_Extend_Lookup_Table_map(testing_Surround_v, HeadExtend_mm);

	map<string, pair<size_t, size_t>> Pricision_map;
	map<string, pair<size_t, size_t>> Recall_map;

	for(size_t i = 0; i < pmDismCase_v.size(); i++){
		DismCase& locDism = *pmDismCase_v[i];
		VERIFY(locDism.Head_Held_Flag || locDism.Extend_Held_Flag);
		locDism.Matched_Extend_Flag = true;
		locDism.Dism_Flag = false;
		if(locDism.Head_Held_Flag){
			if(locDism.pHead->Cand_Flag){
				//locDism.org_TYPE = CEDT_Detection_Flag?POSITIVE:locDism.pHead->pNE_mention->Entity_TYPE;
				locDism.Dism_Flag = true;
			}
		}
		else if(locDism.Extend_Held_Flag){
			if(locDism.pExtend->Cand_Flag){
				//locDism.org_TYPE = CEDT_Detection_Flag?POSITIVE:locDism.pExtend->pNE_mention->Entity_TYPE;
				locDism.Dism_Flag = true;
			}
		}
		if((locDism.predit_TYPE.length() == 0) || (locDism.org_TYPE.length() == 0)){
			AppCall::Secretary_Message_Box("Data Error about predit_TYPE or org_TYPE in CAntical::Analysizing_Performance_Evaluation_on_Extend_and_Head_Matching()", MB_OK);
			continue;
		}
		if(Pricision_map.find(locDism.predit_TYPE) == Pricision_map.end()){
			Pricision_map.insert(make_pair(locDism.predit_TYPE, make_pair(0,0)));
			Recall_map.insert(make_pair(locDism.predit_TYPE, make_pair(0,0)));
		}
		if(Pricision_map.find(locDism.org_TYPE) == Pricision_map.end()){
			Pricision_map.insert(make_pair(locDism.org_TYPE, make_pair(0,0)));
			Recall_map.insert(make_pair(locDism.org_TYPE, make_pair(0,0)));
		}
		Pricision_map[locDism.predit_TYPE].second++;
		Recall_map[locDism.org_TYPE].second++;
		if(!strcmp(locDism.predit_TYPE.c_str(), locDism.org_TYPE.c_str()) && locDism.Matched_Extend_Flag){
			Pricision_map[locDism.predit_TYPE].first++;
			Recall_map[locDism.org_TYPE].first++;
		}
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n\nMatched Head or Extend Only Performance...\n");

	for(map<string, size_t>::iterator mite = Positive_Mentions.begin(); mite != Positive_Mentions.end(); mite++){
		Recall_map[mite->first].second = mite->second;
	}
	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, AppCall::Subsection_Responce_Message_Memo("Revised Performance").c_str());

	MAXEN::Collect_nGross_Performances(Pricision_map, Recall_map, P_nCrossRtn_m, R_nCrossRtn_m);

	for(map<size_t, map<size_t, map<size_t, ACE_extent*>>*>::iterator mmite = HeadExtend_mm.begin(); mmite != HeadExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	HeadExtend_mm.clear();

}
void CAntical::Analysizing_Performance_Evaluation_on_Mention(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m)
{
	if(!CEDT_Extend_Done_Flag && !CEDT_Head_Done_Flag){
		AppCall::Secretary_Message_Box("Emtpy data in: CDiscrimer::Analysizing_Performance_Evaluation_on_Extend_and_Head_Matching()", MB_OK);
	}
	else if(!CEDT_Extend_Done_Flag || !CEDT_Head_Done_Flag){
		//AppCall::Secretary_Message_Box("Take CDiscrimer::Analysizing_Performance_Evaluation_on_Extend_or_Head_Only_Recognition() instead...", MB_OK);
		Analysizing_Performance_Evaluation_on_Extend_or_Head_Only_Recognition(testing_Surround_v, pmDismCase_v, P_nCrossRtn_m, R_nCrossRtn_m);
	}
	else if(CEDT_Extend_Done_Flag && CEDT_Head_Done_Flag){
		Analysizing_Performance_Evaluation_on_Extend_and_Head_Matching(testing_Surround_v, pmDismCase_v, P_nCrossRtn_m, R_nCrossRtn_m);
	}
}

void CAntical::Analysizing_Performance_Evaluation_on_Extend_and_Head_Matching(vector<NE_Surround*>& testing_Surround_v, vector<DismCase*>& pmDismCase_v, Maxen_Rtn_map& P_nCrossRtn_m, Maxen_Rtn_map& R_nCrossRtn_m)
{
	map<string, size_t> Positive_Mentions;
	map<size_t, map<size_t, map<size_t, ACE_extent*>>*> HeadExtend_mm;

	NECOM::Positvie_Mention_Collection(testing_Surround_v, Positive_Mentions, CEDT_Detection_Flag);
	NECOM::Building_Head_to_Extend_Lookup_Table_map(testing_Surround_v, HeadExtend_mm);

	map<string, pair<size_t, size_t>> Pricision_map;
	map<string, pair<size_t, size_t>> Recall_map;

	
	for(size_t i = 0; i < pmDismCase_v.size(); i++){
		DismCase& locDism = *pmDismCase_v[i];
		VERIFY(locDism.Head_Held_Flag);
		VERIFY(locDism.Extend_Held_Flag);

		locDism.Dism_Flag = false;
		locDism.Matched_Extend_Flag = false;
		//locDism.org_TYPE = NEGETIVE;
		if(HeadExtend_mm.find(locDism.SENID) != HeadExtend_mm.end()){
			CanditCase& locHeadCandit = *locDism.pHead;
			CanditCase& locExtendCandit = *locDism.pExtend;
			if(locHeadCandit.SENID == locExtendCandit.SENID){// && (locHeadCandit.Cand_Flag)){
				map<size_t, map<size_t, ACE_extent*>>& locHeadExtend_m = *HeadExtend_mm[locDism.SENID];
				int START = locHeadCandit.START;
				int END = locHeadCandit.END;
				if(locHeadExtend_m.find(START) !=  locHeadExtend_m.end()){
					if(locHeadExtend_m[START].find(END) != locHeadExtend_m[START].end()){
						if((locHeadExtend_m[START][END]->START == locExtendCandit.START) && (locHeadExtend_m[START][END]->END == locExtendCandit.END)){
							locDism.Dism_Flag = true;
							locDism.Matched_Extend_Flag = true;
							locDism.org_TYPE = locHeadCandit.pNE_mention->Entity_TYPE;
							locDism.pNE_mention = locHeadCandit.pNE_mention;
						}
					}
				}
			}
		}
		if(Pricision_map.find(locDism.predit_TYPE) == Pricision_map.end()){
			Pricision_map.insert(make_pair(locDism.predit_TYPE, make_pair(0,0)));
			Recall_map.insert(make_pair(locDism.predit_TYPE, make_pair(0,0)));
		}
		if(Pricision_map.find(locDism.org_TYPE) == Pricision_map.end()){
			Pricision_map.insert(make_pair(locDism.org_TYPE, make_pair(0,0)));
			Recall_map.insert(make_pair(locDism.org_TYPE, make_pair(0,0)));
		}
		Pricision_map[locDism.predit_TYPE].second++;
		Recall_map[locDism.org_TYPE].second++;
		if(!strcmp(locDism.predit_TYPE.c_str(), locDism.org_TYPE.c_str()) && locDism.Matched_Extend_Flag){
			Pricision_map[locDism.predit_TYPE].first++;
			Recall_map[locDism.org_TYPE].first++;
		}
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "\n\nSelected Head or Extend Performance...\n");

	for(map<string, size_t>::iterator mite = Positive_Mentions.begin(); mite != Positive_Mentions.end(); mite++){
		Recall_map[mite->first].second = mite->second;
	}
	MAXEN::Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, AppCall::Subsection_Responce_Message_Memo("Revised Performance").c_str());

	MAXEN::Collect_nGross_Performances(Pricision_map, Recall_map, P_nCrossRtn_m, R_nCrossRtn_m);

	for(map<size_t, map<size_t, map<size_t, ACE_extent*>>*>::iterator mmite = HeadExtend_mm.begin(); mmite != HeadExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	HeadExtend_mm.clear();
}
//=================auxiliary function
void CAntical::Detect_Boundary_Result_Analyser()
{
/*	if(Testing_Cases.START_BoundCase_v.empty() || START_TestRet_v.empty() || END_TestRet_v.empty()){
		AppCall::Secretary_Message_Box("Data set is empty in CDetBound::Detect_Boundary_Result_Analyser()...", MB_OK);
		return;
	}
	vector<BountRet> BoundaryRet_v;
	map<size_t,map<size_t,vector<BoundCase>>> START_map;
	map<size_t,map<size_t,vector<BoundCase>>> END_map;

	for(vector<BoundCase>::iterator svite = Testing_Cases.START_BoundCase_v.begin(); svite != Testing_Cases.START_BoundCase_v.end(); svite++){
		if(svite->Bound_Falg){
			if(START_map.find(svite->SENID) == START_map.end()){
				START_map[svite->SENID];
			}
			if(START_map[svite->SENID].find(svite->position) == START_map[svite->SENID].end()){
				START_map[svite->SENID][svite->position];
			}
			START_map[svite->SENID][svite->position].push_back(*svite);
		}
	}
	for(vector<BoundCase>::iterator svite = Testing_Cases.END_BoundCase_v.begin(); svite != Testing_Cases.END_BoundCase_v.end(); svite++){
		if(svite->Bound_Falg){
			if(END_map.find(svite->SENID) == END_map.end()){
				END_map[svite->SENID];
			}
			if(END_map[svite->SENID].find(svite->position) == END_map[svite->SENID].end()){
				END_map[svite->SENID][svite->position];
			}
			END_map[svite->SENID][svite->position].push_back(*svite);
		}
	}
	
	bool Enitity_Hit_Flag;
	for(vector<NE_Surround>::iterator nevite = Testing_Cases.Surround_v.begin(); nevite != Testing_Cases.Surround_v.end(); nevite++){
		size_t SENID = (*nevite)->SENID;
		for(vector<ACE_entity_mention>::iterator mvite = nevite->entity_mention_v.begin(); mvite != nevite->entity_mention_v.end(); mvite++){

			BountRet loc_BountRet;
			loc_BountRet.insur = nevite->insur;
			loc_BountRet.mention = mvite->extent.charseq;

			ace_op::Delet_0AH_and_20H_in_string(loc_BountRet.insur);
			ace_op::Delet_0AH_and_20H_in_string(loc_BountRet.mention);
			Enitity_Hit_Flag = false;
			for(vector<BoundCase>::iterator svite = START_map[SENID][mvite->extent.START].begin(); svite != START_map[SENID][mvite->extent.START].end(); svite++){
				if((mvite->extent.START == svite->coordinatepair.first) && (mvite->extent.END == svite->coordinatepair.second)){
					loc_BountRet.START_Flag = START_TestRet_v[svite->ID];
					for(vector<BoundCase>::iterator evite = END_map[SENID][mvite->extent.END].begin(); evite != END_map[SENID][mvite->extent.END].end(); evite++){
						if((mvite->extent.START == evite->coordinatepair.first) && (mvite->extent.END == evite->coordinatepair.second)){
							Enitity_Hit_Flag = true;
							loc_BountRet.END_Flag = END_TestRet_v[evite->ID];
						}
					}
				}
			}
			if(!Enitity_Hit_Flag){
				AppCall::Secretary_Message_Box("Enitiy Hit missing...", MB_OK);
				return;
			}
			BoundaryRet_v.push_back(loc_BountRet);
		}
	}
	size_t Right_START = 0;
	size_t Right_END = 0;
	size_t Right_Both = 0;
	size_t Right_One = 0;
	size_t Wrong_Both = 0;

	ostringstream ostream;
	for(vector<BountRet>::iterator vite = BoundaryRet_v.begin(); vite != BoundaryRet_v.end(); vite++){
		if(vite->START_Flag){
			Right_START++;
		}
		if(vite->END_Flag){
			Right_END++;
		}
		if(vite->START_Flag && vite->END_Flag){
			Right_Both++;
		}
		if(vite->START_Flag || vite->END_Flag){
			Right_One++;
		}
		else{
			Wrong_Both++;
		}
	}
	ostream << "------------------------------------------------------" << endl;
	ostream << "size of test entity: " << BoundaryRet_v.size() << endl;
	ostream << "Right START:" <<	1.0*Right_START/BoundaryRet_v.size() <<	"\t\t" << Right_START << "/" << BoundaryRet_v.size() << endl;
	ostream << "Right END:" <<		1.0*Right_END/BoundaryRet_v.size() <<	"\t\t" << Right_END << "/" << BoundaryRet_v.size()<< endl;
	ostream << "Right of Both:" << 1.0*Right_Both/BoundaryRet_v.size() <<	"\t\t" << Right_Both << "/" << BoundaryRet_v.size() << endl;
	ostream << "Right of one:" <<	1.0*Right_One/BoundaryRet_v.size() <<	"\t\t" << Right_One << "/" << BoundaryRet_v.size() << endl;
	ostream << "Wrong of Both:" << 1.0*Wrong_Both/BoundaryRet_v.size() <<	"\t\t" << Wrong_Both << "/" << BoundaryRet_v.size() << endl;

	AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());

	NECOM::Out_BountRet(CEDT_BOUNDARY_RETURN, BoundaryRet_v);
	
*/
}
void CAntical::Named_Entity_Result_Analyser(vector<DismCase>& pmCanditCase_v, vector<bool>& pmNE_TestRet_v)
{
/*	if(pmCanditCase_v.size() != pmNE_TestRet_v.size()){
		AppCall::Secretary_Message_Box("Error: in CDetBound::Named_Entity_Result_Analyser()...", MB_OK);
	}

	string savefilename = DATA_FOLDER;
	savefilename += "CEDT\\NE_rtn.txt";
	ofstream out(savefilename.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	vector<bool>::iterator retite = pmNE_TestRet_v.begin();
	string rtn_flag;
	for(vector<DismCase>::iterator vite = pmCanditCase_v.begin(); vite != pmCanditCase_v.end(); vite++, retite++){
		rtn_flag = *retite?"true":"false";
		out << ace_op::Delet_0AH_and_20H_in_string(vite->sentence.c_str()) << '\t' 
			<< ace_op::Delet_0AH_and_20H_in_string(vite->mention_str.c_str()) << '\t' 
			<< vite->NE_mention.extent.START << '\t' << vite->NE_mention.Entity_TYPE << '\t' << rtn_flag << endl;
	}
	out.close();
	*/
}

void CAntical::Candit_Performance_Evaluation_Output_Result(vector<CanditCase*>& pmCandit_v)
{

	set<string> Number_Set;
	string LexiconPath = DATA_FOLDER;
	LexiconPath += "Number_Set.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(PATH_WORDDB, MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, Number_Set);

	map<string, string> Error_map;
	map<string, string> Right_map;

	string TestResult = DATA_FOLDER;
	TestResult += "CEDT\\Test_Result.txt";
	ofstream Test_out(TestResult.c_str());
	if(Test_out.bad())
		return;
	Test_out.clear();
	Test_out.seekp(0, ios::beg);

	for(size_t i = 0; i < pmCandit_v.size(); i++){
		CanditCase& loc_Candit = *pmCandit_v[i];

		ostringstream ostrsteam;
		if(!strcmp(loc_Candit.org_TYPE.c_str(), loc_Candit.predit_TYPE.c_str())){
			for(int i = 28 - loc_Candit.mention.length(); i > 0; i--){
				ostrsteam<< " ";
			}
			if(loc_Candit.Cand_Flag){
				ostrsteam << loc_Candit.pNE_mention->Entity_TYPE;
			}
			else{
				ostrsteam << NEGETIVE;
			}
			ostrsteam << "\t\t";
			ace_op::Delet_0AH_and_20H_in_string(loc_Candit.prix);
			ace_op::Delet_0AH_and_20H_in_string(loc_Candit.mention);
			ace_op::Delet_0AH_and_20H_in_string(loc_Candit.prox);

			ostrsteam << loc_Candit.prix << '[' << loc_Candit.mention << ']' << loc_Candit.prox;
			Right_map.insert(make_pair(loc_Candit.mention, ostrsteam.str()));
		}
		else{
			for(int i = 28 - loc_Candit.mention.length(); i > 0; i--){
				ostrsteam << " ";
			}
			if(loc_Candit.Cand_Flag){
				ostrsteam << loc_Candit.pNE_mention->Entity_TYPE;
			}
			else{
				ostrsteam << NEGETIVE;
			}
			ostrsteam << "\t\t";
			ace_op::Delet_0AH_and_20H_in_string(loc_Candit.prix);
			ace_op::Delet_0AH_and_20H_in_string(loc_Candit.mention);
			ace_op::Delet_0AH_and_20H_in_string(loc_Candit.prox);

			ostrsteam << loc_Candit.prix << '[' << loc_Candit.mention << ']' << loc_Candit.prox;
			Error_map.insert(make_pair(loc_Candit.mention, ostrsteam.str()));
		}
		if(strcmp(loc_Candit.predit_TYPE.c_str(), NEGETIVE)){
			string locstr = loc_Candit.prix + loc_Candit.mention;
			Test_out << "[" << NLPOP::Get_Chinese_Sentence_Length_Counter(loc_Candit.prix.c_str()) << ", "; 
			Test_out << NLPOP::Get_Chinese_Sentence_Length_Counter(locstr.c_str()) << "]" << '\t';
			Test_out << loc_Candit.predit_TYPE << '\t' << loc_Candit.mention << endl;
			Test_out << loc_Candit.prix << '[' << loc_Candit.mention << ']' << loc_Candit.prox << endl;
			Test_out << endl;
		}
	}
	TestResult = DATA_FOLDER;
	TestResult += "CEDT\\Test_Error_Result.txt";
	ofstream out(TestResult.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(map<string, string>::iterator mite = Error_map.begin(); mite != Error_map.end(); mite++){
		out << mite->first << " " << mite->second << endl;
	}
	out.close();

	TestResult = DATA_FOLDER;
	TestResult += "CEDT\\Test_Right_Result.txt";
	out.open(TestResult.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(map<string, string>::iterator mite = Right_map.begin(); mite != Right_map.end(); mite++){
		out << mite->first << " " << mite->second << endl;
	}
	out.close();
}