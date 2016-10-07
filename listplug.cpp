////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This file is subject to the terms and conditions defined in file           //
// 'http://dreamcyclestudios.com/LICENSE_GPL3.txt', which is part of this     //
//  source code package.                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Some of this code is taken from the original highlight plugin
// The sources to this plugin are available at:
// http://www.theess.com/highlight
// https://web.archive.org/web/20100120012511/http://www.theess.com/highlight
// Copyright (C) 2004 Jens Theeß <jens@theess.com>
// also uses portions of Highlight library by Andre Simon <andre.simon1@gmx.de>
// http://www.andre-simon.de/doku/highlight/en/highlight.php


#ifdef  _WIN32

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <richedit.h>
#include <commdlg.h>


HINSTANCE hinst;
HMODULE FLibHandle=0;

#endif

////////////////////////////////////////////////////////////////////////////////
//                                PLATFORM ANY                                //
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <QTWidgets/QTextEdit>

#include "listplug.h"
#include "yaml-cpp/yaml.h"
#include "hc_wlx.h"
#include "themereader.h"
#include <sstream>



DWORD BGCOLOR;

#define GETCONF(x,y,z) ((config[#x]) ?  config[#x].as<y>() : z)


// Unused in this plugin, may be used to save data
char inifilename[MAX_PATH]="highlightcmd.ini";

char* strlcpy(char* p,const char*p2,int maxlen)
{
    if ((int)strlen(p2)>=maxlen)
    {
        strncpy(p,p2,maxlen);
        p[maxlen]=0;
    }
    else
        strcpy(p,p2);
    return p;
}

string toUpper(const string& input)
{
  locale loc ("C");
  if (input.length() > 0)
  {
    std::vector<char> buffer(input.begin(), input.end());
    std::use_facet<std::ctype<char> >(loc).toupper(&buffer[0], &buffer[0] + buffer.size());
    return std::string(&buffer[0], &buffer[0] + buffer.size());
  }
  else
  {
    return std::string();
  }
}

void createConfig( const string &s )
{
	// create default config
	YAML::Node config;
	config["theme"] = "solarized-dark.theme";
	config["pagesize"] = "letter";
	config["rtfcharstyles"]=false;
	config["wraplines"]=true;
	config["includestyle"]=false;
	config["rtfpagecolor"]=true;
	config["printlinenumbers"]=false;
	config["printzeros"]=false;
	config["fragmentcode"]=false;
	config["keepinjections"]=false;
	config["linenumberwidth"]=5;
	config["linewidth"]=80;
	config["encoding"] = "utf8";
	config["basefont"]="Courier New";
	config["basefontsize"]="11";
	config["disabletrailingnl"]=false;
	config["indentationscheme"]="allman";

	// save it
	std::ofstream fout(s.c_str());
	fout << config;
}
string processFile( const string &apppath ,const string &s)
{
	HighLight_WLX highlight_;
	DataDir dataDir;

	highlight_.dataDir.LSB_DATA_DIR = apppath;
	dataDir.initSearchDirectories ( apppath );
	if(!highlight_.loadFileTypeConfig ( apppath + "\\filetypes" ))
	{
		return "Could not load filetypes.conf";
	}

	// try to load config
	// create with defaults if it does not exist
	YAML::Node config;
	string config_file = string(apppath) + "\\highlightcommander.yaml";
	if (!Platform::fileExists(config_file))
	{
		createConfig(config_file);
	}

	try
	{
		config = YAML::LoadFile( config_file );
	}
	catch(const YAML::Exception& e)
	{
		stringstream ss;
		ss << "Could not load ["  << config_file << "] : " << e.what();
		return ss.str();
	}

	// generate file list to process
	// right now we only process one
    vector <string> inFileList;
	inFileList.push_back(s);

	// generate file list of plugins to apply
	vector <string> plugins;
	// set theme and get generator instance
	std::string theme =  GETCONF(theme,string,"matrix.theme");
	//std::string theme =  ((config["theme"]) ? config["theme"].as<std::string>() :"dusk.theme");

	std::string themePath=dataDir.getThemePath ( theme );
	// we hardcode RTF, but we could make this a web view and use HTML/CSS
	unique_ptr<highlight::CodeGenerator> generator ( highlight::CodeGenerator::getInstance ( highlight::OutputType::RTF ) );

	// Get the background color from the theme so we can set the background in the RTF control
	highlight::ThemeReader docStyle;
	bool loadOK = docStyle.load ( themePath, highlight::OutputType::RTF  );

	// set the color
	// set the color
	if(loadOK)
		BGCOLOR = RGB(docStyle.getBgColour().getRed(),
						docStyle.getBgColour().getGreen(),
						docStyle.getBgColour().getBlue());




	////////////////////////////////////////////////////////////////////////////////
	//                                   OPTIONS                                  //
	////////////////////////////////////////////////////////////////////////////////
	// set generator options
	// TODO:
	// these should probably be in the ini file
	// but for now we hardcode them for testing

    /** set RTF page size
     */
	// a3, a4, a5, b4, b5, b6, letter
	generator->setRTFPageSize ( GETCONF(pagesize,string,"letter") );

	/** set RTF output character styles flag
     */
	// include character stylesheets, bool
    generator->setRTFCharStyles ( GETCONF(rtfcharstyles,bool,false) );

	/** set RTF page color flag
     */
	// set page color, bool
    generator->setRTFPageColor ( GETCONF(rtfpagecolor,bool,true) );

	/** Flag if wrapped lines should receive unique line numbers as well */
	// wether or not wrapped lines should be numbered, bool
    generator->setNumberWrappedLines ( GETCONF(wraplines,bool,true) );

	// style file name css
    // generator->setStyleInputPath ( options.getStyleInFilename() );

	// output file name, no-op we are converting to string
    //generator->setStyleOutputPath ( options.getStyleOutFilename() );

	/** tell parser the include style definition in output
        \param flag true if style should be included
     */
	// include the style definition, bool
    generator->setIncludeStyle ( GETCONF(includestyle,bool,false) );

	/** output line numbers
       \param flag true if line numbers should be printed
       \param startCnt line number starting count
    */
	// print line numbers, bool / starting at number
    generator->setPrintLineNumbers ( GETCONF(printlinenumbers,bool,false) );

	/** output line numbers filled with zeroes
        \param  flag true if zeroes should be printed
    */
	// fill line numbers with zeros, bool
    generator->setPrintZeroes ( GETCONF(printzeros,bool,false) );

	/** omit document header and footer
       \param  flag true if output should be fragmented
    */
	// flag true if output should be fragmented
    generator->setFragmentCode ( GETCONF(fragmentcode,bool,false) );

	/** set keep injections flag
     * \param  flag true if plug-in injections should be outputted if header
     * and footer are omitted (fragmentCode is true)
     */
	// flag true if plug-in injections should be outputted
	// if header and footer are omitted (fragmentCode is true)
    generator->setKeepInjections ( GETCONF(keepinjections,bool,false) );


	/** define line number width
       \param  w width
    */
    generator->setLineNumberWidth ( GETCONF(linenumberwidth,int,7) );

	/** define the preformatting parameters. Preformatting takes place before
        the optional astyle reformatting and indenting is performed (defined by initIndentationScheme)
       \param lineWrappingStyle wrapping style (WRAP_DISABLED, WRAP_SIMPLE, WRAP_DEFAULT)
       \param lineLength max line length
       \param numberSpaces number of spaces which replace a tab
    */
	int linewidth = GETCONF(linewidth,int,70);
    generator->setPreformatting ( highlight::WRAP_DEFAULT,
                                  ( generator->getPrintLineNumbers() ) ?
                                  linewidth - generator->getLineNumberWidth() : linewidth,
                                  4 );


	/** Set encoding (output encoding must match input file)
      \param encodingName encoding name
    */
    generator->setEncoding ( GETCONF(encoding, string, "utf8") );

	/** use this font as base font
      * \param fontName the font name, e.g. "Courier New"
     */
    generator->setBaseFont ( GETCONF(basefont,string,"Courier New") ) ;
	/** use this size as base font size
      * \param fontSize the font size, e.g. "12"
     */
    generator->setBaseFontSize ( GETCONF(basefontsize,string,"11") ) ;

	/** Define the name of a nested langage which is located at the beginning of input.
        The opening embedded delimiter is missing, but the closing delimiter must exist.
    	\param langName name of nested language
    */
    //generator->setStartingNestedLang( options.getStartNestedLang());
	/** tell parser to omit trailing newline character
        \param flag true if no trailing newline should be printed
     */
    generator->disableTrailingNL( GETCONF(disabletrailingnl,bool,false) );
	/** \param path path of plugin input file
    */
	// datadir.getPluginPath
	// this is a user defined plugin
    //generator->setPluginReadFile(options.getPluginReadFilePath());

    //bool styleFileWanted = !options.fragmentOutput() || options.styleOutPathDefined();

	// get a list of stock lua plugins
	const  vector <string> pluginFileList=highlight_.collectPluginPaths( plugins );
	// iterate over plugin list
    for (unsigned int i=0; i<pluginFileList.size(); i++)
	{
		// initialize the stock plugin
        if ( !generator->initPluginScript(pluginFileList[i]) )
		{
			// fail send back error text
			stringstream ss;
            ss << "highlight: "
                 << generator->getPluginScriptError()
                 << " in "
                 << pluginFileList[i]
                 <<"\n";
            return ss.str();
        }
    }

    if ( !generator->initTheme ( themePath ) )
	{
		// fail send back error text
		stringstream ss;
        ss << "highlight: "
             << generator->getThemeInitError()
             << "\n";
        return ss.str();
    }

	/** initialize source code indentation and reformatting scheme;
        needs to be called before using a generate* method
        \param indentScheme Name of indentation scheme
        \return true if successfull
		 * [allman, banner, gnu,
                                  horstmann, java, kr, linux, otbs,
                                  stroustrup, whitesmith]
     */
	bool formattingEnabled = generator->initIndentationScheme ( GETCONF(indentationscheme,string,"allman") );

    if ( !formattingEnabled )
	{
		// fail send back error text
		stringstream ss;
        ss << "highlight: Undefined indentation scheme "
             << "allman"
             << ".\n";
        return ss.str();
    }

	//guess lang
	string suffix = highlight_.guessFileType ( highlight_.getFileSuffix ( inFileList[0] ), inFileList[0] );
	string langDefPath=dataDir.getLangPath ( suffix+".lang" );


	highlight::LoadResult loadRes= generator-> loadLanguage( langDefPath );

	std::stringstream ss;

	if ( loadRes==highlight::LOAD_FAILED_REGEX )
	{

		ss << "highlight: Regex error ( "
			<< generator->getSyntaxRegexError()
			<< " ) in "<<suffix<<".lang\n";
		return ss.str();
	}
	else if ( loadRes==highlight::LOAD_FAILED_LUA )
	{
		ss << "highlight: Lua error ( "
			<< generator->getSyntaxLuaError()
			<< " ) in "<<suffix<<".lang\n";
		return ss.str();
	}
	else if ( loadRes==highlight::LOAD_FAILED )
	{
		ss << "highlight: Unknown source file extension \""
			<< suffix
			<< " [" << langDefPath << "]";

		return ss.str();
	}

	return generator->generateStringFromFile(s);
}

////////////////////////////////////////////////////////////////////////////////
//                               PLATFORM LINUX                               //
////////////////////////////////////////////////////////////////////////////////
/// add linux

#ifdef __linux__ 

#endif

////////////////////////////////////////////////////////////////////////////////
//                              PLATFORM WINDOWS                              //
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	UNREFERENCED_PARAMETER(lpReserved);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hinst=(HINSTANCE)hModule;
        break;
    case DLL_PROCESS_DETACH:
        if (FLibHandle)
            FreeLibrary(FLibHandle);
        FLibHandle=NULL;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

int lastloadtime=0;   // Workaround to RichEdit bug

int __stdcall ListNotificationReceived(HWND ListWin,int Message,WPARAM wParam,LPARAM lParam)
{
    switch (Message)
    {
    case WM_COMMAND:
        if (HIWORD(wParam)==EN_UPDATE && abs(GetCurrentTime()-lastloadtime)>1000)
        {
            int firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
            int linecount=SendMessage(ListWin,EM_GETLINECOUNT,0,0);
            if (linecount>0)
            {
                int percent=MulDiv(firstvisible,100,linecount);
                PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(percent,itm_percent),(LPARAM)ListWin);
            }
            return 0;
        }
        break;
    case WM_NOTIFY:
        break;
    case WM_MEASUREITEM:
        break;
    case WM_DRAWITEM:
        break;
    }
    return 0;
}
HWND __stdcall ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags)
{
    HWND hwnd = NULL;
    RECT r;
    const char* text = NULL;

    // Get the directory of the dll
	char path[MAX_PATH];
	GetModuleFileName(hinst, path, sizeof(path));
	*strrchr(path, '\\') = '\0';

    // Highlight file.
    string result = processFile(string(path) +"\\",FileToLoad);

    // Has the file been parsed?
    if (result == "")
    {
      /*cerr.rdbuf(buf);
      text = strdup(debug.str().c_str());*/
      return NULL;
    }
    else
      text = result.c_str();

	//text = "This is a test";
    if (!FLibHandle)
    {
        int OldError = SetErrorMode(SEM_NOOPENFILEERRORBOX);
        FLibHandle = LoadLibrary("Riched20.dll");
        if (!FLibHandle)
            FLibHandle = LoadLibrary("RICHED32.DLL");
        if (!FLibHandle)
            FLibHandle = NULL;
        SetErrorMode(OldError);
    }

    lastloadtime=GetCurrentTime();


#if(0)
	QTextEdit *txt = new QTextEdit();
	txt->setText("Hello, world!");
	txt->append("Appending some text…");

	txt->show();
#endif
    GetClientRect(ParentWin,&r);
    // Create window invisbile, only show when data fully loaded!
    hwnd=CreateWindow("RichEdit20A","",WS_CHILD | ES_MULTILINE | ES_READONLY |
                      WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL,
                      r.left,r.top,r.right-r.left,
                      r.bottom-r.top,ParentWin,NULL,hinst,NULL);

    if (!hwnd)
    {
        hwnd=CreateWindow("RichEdit","",WS_CHILD | ES_MULTILINE | ES_READONLY |
                          WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL,
                          r.left,r.top,r.right-r.left,
                          r.bottom-r.top,ParentWin,NULL,hinst,NULL);
    }

    if (hwnd)
    {
        SendMessage(hwnd, EM_SETMARGINS, EC_LEFTMARGIN, 8);

        //ENM_SCROLL doesn't work for thumb movements!
        SendMessage(hwnd, EM_SETEVENTMASK, 0, ENM_UPDATE);

        PostMessage(ParentWin,WM_COMMAND,MAKELONG(lcp_ansi,itm_fontstyle),(LPARAM)hwnd);

        // Amount of text the control can contain.
        SendMessage(hwnd, EM_EXLIMITTEXT, 0, strlen(text));

        // Hide the caret.
        SetFocus(ParentWin);

        // Word-wrapping
        if ((ShowFlags & lcp_wraptext) == 0)
        {
          // no wrapping
          SendMessage(hwnd, EM_SETTARGETDEVICE, 0, 1);
        }
        else
        {
          // enable wrapping
          SendMessage(hwnd, EM_SETTARGETDEVICE, 0, 0);
        }

		SendMessage(hwnd,EM_SETBKGNDCOLOR,0,BGCOLOR);

        // Display file in lister window.
        if (text != NULL)
        {
          SetWindowText(hwnd,text);
          PostMessage(ParentWin,WM_COMMAND,MAKELONG(0,itm_percent),(LPARAM)hwnd);
        }
    }

    lastloadtime=GetCurrentTime();

    if (hwnd)
        ShowWindow(hwnd,SW_SHOW);

    return hwnd;
}

int __stdcall ListSendCommand(HWND ListWin,int Command,int Parameter)
{
  switch (Command)
  {
  case lc_copy:
    SendMessage(ListWin,WM_COPY,0,0);
    return LISTPLUGIN_OK;

  case lc_newparams:
    if ((Parameter & lcp_wraptext) == 0)
    {
      // no wrapping
      SendMessage(ListWin, EM_SETTARGETDEVICE, 0, 1);
    }
    else if ((Parameter & lcp_wraptext) != 0)
    {
      // enable wrapping
      SendMessage(ListWin, EM_SETTARGETDEVICE, 0, 0);
    }
    return LISTPLUGIN_OK;

  case lc_selectall:
    SendMessage(ListWin,EM_SETSEL,0,-1);
    return LISTPLUGIN_OK;

  case lc_setpercent:
    int firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
    int linecount=SendMessage(ListWin,EM_GETLINECOUNT,0,0);
    if (linecount>0)
    {
      int pos=MulDiv(Parameter,linecount,100);
      SendMessage(ListWin,EM_LINESCROLL,0,pos-firstvisible);
      firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
      // Place caret on first visible line!
      int firstchar=SendMessage(ListWin,EM_LINEINDEX,firstvisible,0);
      SendMessage(ListWin,EM_SETSEL,firstchar,firstchar);
      pos=MulDiv(firstvisible,100,linecount);
      // Update percentage display
      PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(pos,itm_percent),(LPARAM)ListWin);
      return LISTPLUGIN_OK;
    }
    break;
  }
  return LISTPLUGIN_ERROR;
}

int __stdcall ListSearchText(HWND ListWin,char* SearchString,int SearchParameter)
{
    FINDTEXT find;
    int StartPos,Flags;

    if (SearchParameter & lcs_findfirst)
    {
        //Find first: Start at top visible line
        StartPos=SendMessage(ListWin,EM_LINEINDEX,SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0),0);
        SendMessage(ListWin,EM_SETSEL,StartPos,StartPos);
    }
    else
    {
        //Find next: Start at current selection+1
        SendMessage(ListWin,EM_GETSEL,(WPARAM)&StartPos,0);
        StartPos+=1;
    }

    find.chrg.cpMin=StartPos;
    find.chrg.cpMax=SendMessage(ListWin,WM_GETTEXTLENGTH,0,0);
    Flags=0;
    if (SearchParameter & lcs_wholewords)
        Flags |= FR_WHOLEWORD;
    if (SearchParameter & lcs_matchcase)
        Flags |= FR_MATCHCASE;
    if (!(SearchParameter & lcs_backwards))
        Flags |= FR_DOWN;
    find.lpstrText=SearchString;
    int index=SendMessage(ListWin, EM_FINDTEXT, Flags, (LPARAM)&find);

    if (index!=-1)
    {
        int indexend=index+strlen(SearchString);
        SendMessage(ListWin,EM_SETSEL,index,indexend);
        int line=SendMessage(ListWin,EM_LINEFROMCHAR,index,0)-3;
        if (line<0)
            line=0;
        line-=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
        SendMessage(ListWin,EM_LINESCROLL,0,line);
        return LISTPLUGIN_OK;
    }

    // Restart search at the beginning
    // SendMessage(ListWin,EM_SETSEL,0,0);

    MessageBox(ListWin, SearchString, "Not found:", MB_OK);

    return LISTPLUGIN_ERROR;
}

void __stdcall ListCloseWindow(HWND ListWin)
{
    DestroyWindow(ListWin);
    return;
}

int __stdcall ListPrint(HWND ListWin,char* FileToPrint,char* DefPrinter,int PrintFlags,RECT* Margins)
{
    PRINTDLG PrintDlgRec;
    memset(&PrintDlgRec,0,sizeof(PRINTDLG));
    PrintDlgRec.lStructSize=sizeof(PRINTDLG);

    PrintDlgRec.Flags= PD_ALLPAGES | PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
    PrintDlgRec.nFromPage   = 0xFFFF;
    PrintDlgRec.nToPage     = 0xFFFF;
    PrintDlgRec.nMinPage	= 1;
    PrintDlgRec.nMaxPage	= 0xFFFF;
    PrintDlgRec.nCopies		= 1;
    PrintDlgRec.hwndOwner	= ListWin;// MUST be Zero, otherwise crash!
    if (PrintDlg(&PrintDlgRec))
    {
        HDC hdc=PrintDlgRec.hDC;
        DOCINFO DocInfo;
        POINT offset,physsize,start,avail,printable;
        int LogX,LogY;
        RECT rcsaved;

        // Warning: PD_ALLPAGES is zero!
        BOOL PrintSel=(PrintDlgRec.Flags & PD_SELECTION);
        BOOL PrintPages=(PrintDlgRec.Flags & PD_PAGENUMS);
        int PageFrom=1;
        int PageTo=0x7FFF;
        if (PrintPages)
        {
            PageFrom=PrintDlgRec.nFromPage;
            PageTo=PrintDlgRec.nToPage;
            if (PageTo<=0)
                PageTo=0x7FFF;
        }


        memset(&DocInfo,0,sizeof(DOCINFO));
        DocInfo.cbSize=sizeof(DOCINFO);
        DocInfo.lpszDocName=FileToPrint;
        if (StartDoc(hdc,&DocInfo))
        {
            SetMapMode(hdc,MM_LOMETRIC);
            offset.x=GetDeviceCaps(hdc,PHYSICALOFFSETX);
            offset.y=GetDeviceCaps(hdc,PHYSICALOFFSETY);
            DPtoLP(hdc,&offset,1);
            physsize.x=GetDeviceCaps(hdc,PHYSICALWIDTH);
            physsize.y=GetDeviceCaps(hdc,PHYSICALHEIGHT);
            DPtoLP(hdc,&physsize,1);

            start.x=Margins->left-offset.x;
            start.y=-Margins->top-offset.y;
            if (start.x<0)
                start.x=0;
            if (start.y>0)
                start.y=0;
            avail.x=GetDeviceCaps(hdc,HORZRES);
            avail.y=GetDeviceCaps(hdc,VERTRES);
            DPtoLP(hdc,&avail,1);

            printable.x=min(physsize.x-(Margins->right+Margins->left),avail.x-start.x);
            printable.y=max(physsize.y+(Margins->top+Margins->bottom),avail.y-start.y);

            LogX=GetDeviceCaps(hdc, LOGPIXELSX);
            LogY=GetDeviceCaps(hdc, LOGPIXELSY);

            SendMessage(ListWin, EM_FORMATRANGE, 0, 0);

            FORMATRANGE Range;
            memset(&Range,0,sizeof(FORMATRANGE));
            Range.hdc=hdc;
            Range.hdcTarget=hdc;
            LPtoDP(hdc,&start,1);
            LPtoDP(hdc,&printable,1);
            Range.rc.left = start.x * 1440 / LogX;
            Range.rc.top = start.y * 1440 / LogY;
            Range.rc.right = (start.x+printable.x) * 1440 / LogX;
            Range.rc.bottom = (start.y+printable.y) * 1440 / LogY;
            SetMapMode(hdc,MM_TEXT);

            BOOL PrintAborted=false;
            Range.rcPage = Range.rc;
            rcsaved = Range.rc;
            int CurrentPage = 1;
            int LastChar = 0;
            int LastChar2= 0;
            int MaxLen = SendMessage(ListWin,WM_GETTEXTLENGTH,0,0);
            Range.chrg.cpMax = -1;
            if (PrintPages)
            {
                do
                {
                    Range.chrg.cpMin = LastChar;
                    if (CurrentPage<PageFrom)
                    {
                        LastChar = SendMessage(ListWin, EM_FORMATRANGE, 0, (LPARAM)&Range);
                    }
                    else
                    {
                        //waitform.ProgressLabel.Caption:=spage+inttostr(CurrentPage);
                        //application.processmessages;
                        LastChar = SendMessage(ListWin, EM_FORMATRANGE, 1, (LPARAM)&Range);
                    }
                    // Warning: At end of document, LastChar may be<MaxLen!!!
                    if (LastChar!=-1 && LastChar < MaxLen)
                    {
                        Range.rc=rcsaved;                // Check whether another page comes
                        Range.rcPage = Range.rc;
                        Range.chrg.cpMin = LastChar;
                        LastChar2= SendMessage(ListWin, EM_FORMATRANGE, 0, (LPARAM)&Range);
                        if (LastChar<LastChar2 && LastChar < MaxLen && LastChar != -1 &&
                                CurrentPage>=PageFrom && CurrentPage<PageTo)
                        {
                            EndPage(hdc);
                        }
                    }

                    CurrentPage++;
                    Range.rc=rcsaved;
                    Range.rcPage = Range.rc;
                }
                while (LastChar < MaxLen && LastChar != -1 && LastChar<LastChar2 &&
                        (PrintPages && CurrentPage<=PageTo) && !PrintAborted);
            }
            else
            {
                if (PrintSel)
                {
                    SendMessage(ListWin,EM_GETSEL,(WPARAM)&LastChar,(LPARAM)&MaxLen);
                    Range.chrg.cpMax = MaxLen;
                }
                do
                {
                    Range.chrg.cpMin = LastChar;
                    //waitform.ProgressLabel.Caption:=spage+inttostr(CurrentPage);
                    //waitform.ProgressLabel.update;
                    //application.processmessages;
                    LastChar = SendMessage(ListWin, EM_FORMATRANGE, 1, (LPARAM)&Range);

                    // Warning: At end of document, LastChar may be<MaxLen!!!
                    if (LastChar!=-1 && LastChar < MaxLen)
                    {
                        Range.rc=rcsaved;                // Check whether another page comes
                        Range.rcPage = Range.rc;
                        Range.chrg.cpMin = LastChar;
                        LastChar2= SendMessage(ListWin, EM_FORMATRANGE, 0, (LPARAM)&Range);
                        if (LastChar<LastChar2 && LastChar < MaxLen && LastChar != -1)
                        {
                            EndPage(hdc);
                        }
                    }
                    CurrentPage++;
                    Range.rc=rcsaved;
                    Range.rcPage = Range.rc;
                }
                while (LastChar<LastChar2 && LastChar < MaxLen && LastChar != -1 && !PrintAborted);
            }
            if (PrintAborted)
                AbortDoc(hdc);
            else
                EndDoc(hdc);
        } //StartDoc

        SendMessage(ListWin, EM_FORMATRANGE, 0, 0);
        DeleteDC(PrintDlgRec.hDC);
    }
    if (PrintDlgRec.hDevNames)
        GlobalFree(PrintDlgRec.hDevNames);
    return 0;
}

void __stdcall ListGetDetectString(char* DetectString,int maxlen)
{
  // OMG do we cover a hell of a lot of langs
  strlcpy(DetectString, "EXT='ADB'|EXT='ADS'|EXT='A'|EXT='GNAD'|EXT='ALG'|EXT='DAT'|EXT='RUN'|"
		"EXT='S4'|EXT='S4T'|EXT='S4H'|EXT='HND'|EXT='T4'|EXT='A51'|EXT='29K'|"
		"EXT='68S'|EXT='68X'|EXT='X86'|EXT='S'|EXT='ASPX'|EXT='ASHX'|EXT='ASCX'|"
		"EXT='DATS'|EXT='WAS'|EXT='WUD'|EXT='CMD'|EXT='C++'|EXT='CPP'|EXT='CXX'|"
		"EXT='CC'|EXT='H'|EXT='HH'|EXT='HXX'|EXT='HPP'|EXT='CU'|EXT='INP'|EXT='CFC'|"
		"EXT='CFM'|EXT='COB'|EXT='CBL'|EXT='PATCH'|EXT='DTSI'|EXT='E'|EXT='SE'|"
		"EXT='HRL'|EXT='ERL'|EXT='EX'|EXT='EXW'|EXT='WXU'|EXT='EW'|EXT='EU'|EXT='F'|"
		"EXT='FOR'|EXT='FTN'|EXT='F95'|EXT='F90'|EXT='CLASS'|EXT='HS'|EXT='DOXYFILE'|"
		"EXT='DESKTOP'|EXT='GROOVY'|EXT='GRV'|EXT='JL'|EXT='KT'|EXT='B'|EXT='CL'|"
		"EXT='CLISP'|EXT='EL'|EXT='LSP'|EXT='SBCL'|EXT='SCOM'|EXT='MAK'|EXT='MK'|"
		"EXT='MAKEFILE'|EXT='MIB'|EXT='SMI'|EXT='ML'|EXT='MLI'|EXT='MOD'|EXT='DEF'|"
		"EXT='M3'|EXT='I3'|EXT='OOC'|EXT='PHP3'|EXT='PHP4'|EXT='PHP5'|EXT='PHP6'|"
		"EXT='PMOD'|EXT='FF'|EXT='FP'|EXT='FPP'|EXT='RPP'|EXT='SF'|EXT='SP'|EXT='SPB'|"
		"EXT='SPP'|EXT='SPS'|EXT='WP'|EXT='WF'|EXT='WPP'|EXT='WPS'|EXT='WPB'|EXT='BDY'|"
		"EXT='SPE'|EXT='PL'|EXT='PERL'|EXT='CGI'|EXT='PM'|EXT='PLX'|EXT='PLEX'|EXT='P'|"
		"EXT='I'|EXT='W'|EXT='RB'|EXT='RUBY'|EXT='PP'|EXT='RJS'|EXT='GEMFILE'|"
		"EXT='RAKEFILE'|EXT='REX'|EXT='RX'|EXT='THE'|EXT='BASH'|EXT='ZSH'|EXT='EBUILD'|"
		"EXT='ECLASS'|EXT='ST'|EXT='GST'|EXT='SQ'|EXT='SP'|EXT='WISH'|EXT='ITCL'|"
		"EXT='STY'|EXT='CLS'|EXT='BAS'|EXT='BASIC'|EXT='BI'|EXT='VBS'|EXT='V'|"
		"EXT='HTM'|EXT='XHTML'|EXT='SGM'|EXT='SGML'|EXT='NRM'|EXT='ENT'|EXT='HDR'|"
		"EXT='HUB'|EXT='DTD'|EXT='GLADE'|EXT='WML'|EXT='VXML'|EXT='WML'|EXT='TLD'|"
		"EXT='CSPROJ'|EXT='XSL'|EXT='ECF'|EXT='JNLP'|EXT='XSD'|EXT='RESX'|EXT='FS'|"
		"EXT='FSX'|EXT='4GL'|EXT='BB'|EXT='ISS'|EXT='LS'|EXT='A4C'|EXT='AS'|EXT='EXP'|"
		"EXT='HX'|EXT='PYX'|EXT='ABP'|EXT='CS'|EXT='ILI'|EXT='LGT'|EXT='M'|EXT='NSI'|"
		"EXT='NSH'|EXT='Y'|EXT='NUT'|EXT='LBN'|EXT='MEL'|EXT='N'|EXT='SC'|EXT='NRX'|"
		"EXT='CB'|EXT='DOT'|EXT='SMA'|EXT='AU3'|EXT='CHL'|EXT='AHK'|EXT='FAME'|EXT='MO'|"
		"EXT='MPL'|EXT='J'|EXT='SNO'|EXT='ICN'|EXT='FLX'|EXT='LSL'|EXT='LY'|EXT='NAS'|"
		"EXT='ICL'|EXT='ASM'|EXT='BIB'|EXT='PY'|EXT='TEXT'|EXT='TTL'|EXT='NT'|EXT='BFR'|"
		"EXT='SCI'|EXT='SCE'|EXT='NBS'", maxlen);

}

void __stdcall ListSetDefaultParams(ListDefaultParamStruct* dps)
{

  // strlcpy(inifilename,dps->DefaultIniName,MAX_PATH-1);
}
#endif
