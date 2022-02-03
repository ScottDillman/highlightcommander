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

#ifndef HC_WLX
#define HC_WLX


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cassert>
#include <sstream>


// #include "cmdlineoptions.h"
#include "codegenerator.h"
//#include "help.h"
#include "datadir.h"
#include "version.h"
#include "platform_fs.h"

#define IO_ERROR_REPORT_LENGTH 5
#define SHEBANG_CNT 12

typedef map<string, string> StringMap;

class HighLight_WLX
{

public:

    HighLight_WLX() {};
    ~HighLight_WLX() {};

	DataDir dataDir;
  StringMap extensions;
  StringMap scriptShebangs;


	stringstream cin_bufcopy;

    /** \return file extension or the base filename if no extension exists
    */
    string getFileSuffix ( const string &fileName );

    /** \return file type deferred from extension or file shebang comment
    */
    string guessFileType ( const string &suffix, const string &inputFile, bool useUserSuffix=false );

    int getNumDigits ( int i );

    bool readInputFilePaths ( vector<string> &fileList, string wildcard,
                              bool recursiveSearch );

    string analyzeFile ( const string& file );
    bool loadFileTypeConfig ( const string& name );

    vector <string> collectPluginPaths(const vector<string>& plugins);

};

#endif
