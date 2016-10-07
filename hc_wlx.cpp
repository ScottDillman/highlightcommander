////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This file is subject to the terms and conditions defined in file           //
// 'http://dreamcyclestudios.com/LICENSE.txt', which is part of this source   //
// code package.                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Some of this code is taken from the original highlight plugin
// The sources to this plugin are available at:
// http://www.theess.com/highlight
// https://web.archive.org/web/20100120012511/http://www.theess.com/highlight
// Copyright (C) 2004 Jens Theeß <jens@theess.com>
// also uses portions of Highlight library by Andre Simon <andre.simon1@gmx.de>
// http://www.andre-simon.de/doku/highlight/en/highlight.php


////////////////////////////////////////////////////////////////////////////////
//                                  INCLUDES                                  //
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <algorithm>
#include <Diluculum/LuaState.hpp>



#include "hc_wlx.h"
#include "datadir.h"
#include "syntaxreader.h"

// wrap at line
#define MAX_LINE__WIDTH       80

using namespace std;

// get file extension
string HighLight_WLX::getFileSuffix(const string& fileName)
{
    size_t ptPos=fileName.rfind(".");
    size_t psPos = fileName.rfind ( Platform::pathSeparator );
    if (ptPos == string::npos) {
        return  (psPos==string::npos) ? fileName : fileName.substr(psPos+1, fileName.length());
    }
    return (psPos!=string::npos && psPos>ptPos) ? "" : fileName.substr(ptPos+1, fileName.length());
}


// given a file type get config
bool HighLight_WLX::loadFileTypeConfig ( const string& confName )
{
   // if ( !extMap || !shebangMap ) return false;
    string confPath=dataDir.getFiletypesConfPath(confName);
    try {
        Diluculum::LuaState ls;
        Diluculum::LuaValueList ret= ls.doFile (confPath);

        int idx=1;
        string langName;
        Diluculum::LuaValue mapEntry;
        while ((mapEntry = ls["FileMapping"][idx].value()) !=Diluculum::Nil) {
            langName = mapEntry["Lang"].asString();
            if (mapEntry["Extensions"] !=Diluculum::Nil) {
                int extIdx=1;
                while (mapEntry["Extensions"][extIdx] !=Diluculum::Nil) {
                    extensions.insert ( make_pair ( mapEntry["Extensions"][extIdx].asString(),  langName ) );
                    extIdx++;
                }
            } else if (mapEntry["Shebang"] !=Diluculum::Nil) {
                scriptShebangs.insert ( make_pair ( mapEntry["Shebang"].asString(),  langName ) );
            }
            idx++;
        }

    } catch (Diluculum::LuaError err) {
        cerr <<err.what()<<"\n";
        return false;
    }
    return true;
}

int HighLight_WLX::getNumDigits ( int i )
{
    int res=0;
    while ( i ) {
        i/=10;
        ++res;
    }
    return res;
}

using namespace boost::xpressive;

string HighLight_WLX::analyzeFile ( const string& file )
{
    string firstLine;
    if ( !file.empty() ) {
        ifstream inFile ( file.c_str() );
        getline ( inFile, firstLine );
    } else {
        //  This copies all the data to a new buffer, uses the data to get the
        //  first line, then makes cin use the new buffer that underlies the
        //  stringstream instance
        cin_bufcopy << cin.rdbuf();
        getline ( cin_bufcopy, firstLine );
        cin_bufcopy.seekg ( 0, ios::beg );
        cin.rdbuf ( cin_bufcopy.rdbuf() );
    }
	boost::xpressive::sregex rex;
    boost::xpressive::smatch what;
	StringMap::iterator it;
	
	sregex_compiler compiler;
	
	for ( it=scriptShebangs.begin(); it!=scriptShebangs.end(); it++ ) 
	{
        rex = boost::xpressive::sregex::compile( it->first );
		if ( boost::xpressive::regex_search( firstLine, what, rex )  ) return it->second;
    }
	
	#if(0)
	// TODO: LINKER ERROR
	// multiple definition of `boost::xpressive::cpp_regex_traits<char>::char_class(unsigned long long)::s_char_class_map'
	StringMap::iterator it;
    boost::xpressive::sregex rex2;
    boost::xpressive::smatch what;
    for ( it=scriptShebangs.begin(); it!=scriptShebangs.end(); it++ ) 
	{
        rex2 = boost::xpressive::sregex::compile( it->first );
        if ( boost::xpressive::regex_search( firstLine, what, rex2 )  ) return it->second;
    }
	
	#endif
	
    return "";
}

string HighLight_WLX::guessFileType ( const string& suffix, const string &inputFile, bool useUserSuffix )
{
    string lcSuffix = StringTools::change_case(suffix);
    if (extensions.count(lcSuffix)) {
        return extensions[lcSuffix];
    }

    if (!useUserSuffix) {
        string shebang =  analyzeFile(inputFile);
        if (!shebang.empty()) return shebang;
    }
    return lcSuffix;
}

vector <string> HighLight_WLX::collectPluginPaths(const vector<string>& plugins)
{
    vector<string> absolutePaths;
    for (unsigned int i=0; i<plugins.size(); i++) {
        if (Platform::fileExists(plugins[i])) {
            absolutePaths.push_back(plugins[i]);
        } else {
            absolutePaths.push_back(dataDir.getPluginPath(plugins[i]+".lua"));
        }
    }
    return absolutePaths;
}