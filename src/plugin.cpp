#ifdef  _WIN32

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <yaml-cpp/yaml.h>

#endif


#include <QFile>
#include <QFontDatabase>
#include <QCoreApplication>
#include <QMimeDatabase>
#include <QPlainTextEdit>
#include <QSettings>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>



#ifdef __linux__ 

#include "yaml-cpp/yaml.h"
#include <dlfcn.h>
#include <libintl.h>
#include <locale.h>
#define _(STRING) gettext(STRING)
#define GETTEXT_PACKAGE "plugins"

#endif

#include "listplug.h"
#include "hc_wlx.h"

bool darktheme = false;
QFont font;
QMimeDatabase db;

#define GETCONF(x, y, z) ((config[#x]) ? config[#x].as<y>() : z)

char *strlcpy(char *p, const char *p2, int maxlen)
{
	if ((int)strlen(p2) >= maxlen)
	{
		strncpy(p, p2, maxlen);
		p[maxlen] = 0;
	}
	else
		strcpy(p, p2);
	return p;
}

string toUpper(const string &input)
{
	locale loc("C");
	if (input.length() > 0)
	{
		std::vector<char> buffer(input.begin(), input.end());
		std::use_facet<std::ctype<char>>(loc).toupper(&buffer[0], &buffer[0] + buffer.size());
		return std::string(&buffer[0], &buffer[0] + buffer.size());
	}
	else
	{
		return std::string();
	}
}

void createConfig(const string &s)
{
	/// create default config
	YAML::Node config;
    config["theme"] = "bitwiseninja.theme";
	config["pagesize"] = "letter";
	config["rtfcharstyles"] = false;
	config["wraplines"] = true;
    config["includestyle"] = true;
	config["rtfpagecolor"] = true;
	config["printlinenumbers"] = false;
	config["printzeros"] = false;
	config["fragmentcode"] = false;
	config["keepinjections"] = false;
	config["linenumberwidth"] = 5;
    config["linewidth"] = 120;
	config["encoding"] = "utf8";
	config["basefont"] = "Courier New";
	config["basefontsize"] = "11";
	config["disabletrailingnl"] = false;
	config["indentationscheme"] = "allman";

	    // save it
	std::ofstream fout(s.c_str());
	fout << config;
}

string processFile(const string &apppath, const string &s)
{
	HighLight_WLX highlight_;
	DataDir dataDir;

	highlight_.dataDir.LSB_DATA_DIR = apppath;
	dataDir.initSearchDirectories(apppath);

    string types_file = string(apppath) + "/filetypes";

    if (!highlight_.loadFileTypeConfig(types_file))
	{
        return "Could not find conf: " + types_file;
	}

    // try to load config
    // create with defaults if it does not exist
	YAML::Node config;
	string config_file = string(apppath) + "/highlightcommander.yaml";
	if (!Platform::fileExists(config_file))
	{
		createConfig(config_file);
	}

	try
	{
		config = YAML::LoadFile(config_file);
	}
	catch (const YAML::Exception &e)
	{
		stringstream ss;
		ss << "Could not load [" << config_file << "] : " << e.what();
		return ss.str();
	}

	// generate file list to process
	// right now we only process one
	vector<string> inFileList;
	inFileList.push_back(s);

	// generate file list of plugins to apply
	vector<string> plugins;
	// set theme and get generator instance
	std::string theme = GETCONF(theme, string, "matrix.theme");

	std::string themePath = dataDir.getThemePath(theme);
    	// we hardcode RTF, but we could make this a web view and use HTML/CSS
	unique_ptr<highlight::CodeGenerator> generator(highlight::CodeGenerator::getInstance(highlight::OutputType::HTML));

	    // Get the background color from the theme so we can set the background in the RTF control
	//highlight::ThemeReader docStyle;
	//bool loadOK = docStyle.load(themePath, highlight::OutputType::HTML);

// #ifdef _WIN32
// 	set the color
// 	set the color
// 	if (loadOK)
// 		BGCOLOR = RGB(docStyle.getBgColour().getRed(),
// 					  docStyle.getBgColour().getGreen(),
// 					  docStyle.getBgColour().getBlue());
// #endif

	////////////////////////////////////////////////////////////////////////////////
	//                                   OPTIONS                                  //
	////////////////////////////////////////////////////////////////////////////////
	// set generator options
	// TODO:
	// these should probably be in the ini file
	// but for now we hardcode them for testing


	/** Flag if wrapped lines should receive unique line numbers as well */
	// wether or not wrapped lines should be numbered, bool
	generator->setNumberWrappedLines(GETCONF(wraplines, bool, true));

	// include the style definition, bool
	generator->setIncludeStyle(GETCONF(includestyle, bool, false));

	/** output line numbers
       \param flag true if line numbers should be printed
       \param startCnt line number starting count
    */
	// print line numbers, bool / starting at number
	generator->setPrintLineNumbers(GETCONF(printlinenumbers, bool, false));

	/** output line numbers filled with zeroes
        \param  flag true if zeroes should be printed
    */
	// fill line numbers with zeros, bool
	generator->setPrintZeroes(GETCONF(printzeros, bool, false));

	/** omit document header and footer
       \param  flag true if output should be fragmented
    */
	// flag true if output should be fragmented
	generator->setFragmentCode(GETCONF(fragmentcode, bool, false));

	/** set keep injections flag
     * \param  flag true if plug-in injections should be outputted if header
     * and footer are omitted (fragmentCode is true)
     */
	// flag true if plug-in injections should be outputted
	// if header and footer are omitted (fragmentCode is true)
	generator->setKeepInjections(GETCONF(keepinjections, bool, false));

	/** define line number width
       \param  w width
    */
	generator->setLineNumberWidth(GETCONF(linenumberwidth, int, 7));

	/** define the preformatting parameters. Preformatting takes place before
        the optional astyle reformatting and indenting is performed (defined by initIndentationScheme)
       \param lineWrappingStyle wrapping style (WRAP_DISABLED, WRAP_SIMPLE, WRAP_DEFAULT)
       \param lineLength max line length
       \param numberSpaces number of spaces which replace a tab
    */
	int linewidth = GETCONF(linewidth, int, 70);
	generator->setPreformatting(highlight::WRAP_DEFAULT,
								(generator->getPrintLineNumbers()) ? linewidth - generator->getLineNumberWidth() : linewidth,
								4);

	/** Set encoding (output encoding must match input file)
      \param encodingName encoding name
    */
	generator->setEncoding(GETCONF(encoding, string, "utf8"));

	/** use this font as base font
      * \param fontName the font name, e.g. "Courier New"
     */
	generator->setBaseFont(GETCONF(basefont, string, "Courier New"));
	/** use this size as base font size
      * \param fontSize the font size, e.g. "12"
     */
	generator->setBaseFontSize(GETCONF(basefontsize, string, "9"));

	/** Define the name of a nested langage which is located at the beginning of input.
        The opening embedded delimiter is missing, but the closing delimiter must exist.
    	\param langName name of nested language
    */
	//generator->setStartingNestedLang( options.getStartNestedLang());
	/** tell parser to omit trailing newline character
        \param flag true if no trailing newline should be printed
     */
	generator->disableTrailingNL(GETCONF(disabletrailingnl, bool, false));
	/** \param path path of plugin input file
    */
	// datadir.getPluginPath
	// this is a user defined plugin
	//generator->setPluginReadFile(options.getPluginReadFilePath());

	//bool styleFileWanted = !options.fragmentOutput() || options.styleOutPathDefined();

	// get a list of stock lua plugins
	const vector<string> pluginFileList = highlight_.collectPluginPaths(plugins);
	// iterate over plugin list
	for (unsigned int i = 0; i < pluginFileList.size(); i++)
	{
		// initialize the stock plugin
		if (!generator->initPluginScript(pluginFileList[i]))
		{
			// fail send back error text
			stringstream ss;
			ss << "highlight: "
			   << generator->getPluginScriptError()
			   << " in "
			   << pluginFileList[i]
			   << "\n";
			return ss.str();
		}
	}

	if (!generator->initTheme(themePath))
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
	bool formattingEnabled = generator->initIndentationScheme(GETCONF(indentationscheme, string, "allman"));

	if (!formattingEnabled)
	{
        // fail send back error text
		stringstream ss;
		ss << "highlight: Undefined indentation scheme "
		   << "allman"
		   << ".\n";
		return ss.str();
	}

    	//guess lang
	string suffix = highlight_.guessFileType(highlight_.getFileSuffix(inFileList[0]), inFileList[0]);
	string langDefPath = dataDir.getLangPath(suffix + ".lang");

	highlight::LoadResult loadRes = generator->loadLanguage(langDefPath);

	std::stringstream ss;

	if (loadRes == highlight::LOAD_FAILED_REGEX)
	{

		ss << "highlight: Regex error ( "
		   << generator->getSyntaxRegexError()
		   << " ) in " << suffix << ".lang\n";
		return ss.str();
	}
	else if (loadRes == highlight::LOAD_FAILED_LUA)
	{
		ss << "highlight: Lua error ( "
		   << generator->getSyntaxLuaError()
		   << " ) in " << suffix << ".lang\n";
		return ss.str();
	}
	else if (loadRes == highlight::LOAD_FAILED)
	{
		ss << "highlight: Unknown source file extension \""
		   << suffix
		   << " [" << langDefPath << "]";

		return ss.str();
	}

	return generator->generateStringFromFile(s);
}


HANDLE DCPCALL ListLoad(HANDLE ParentWin, char* FileToLoad, int ShowFlags)
{
   
  QMimeType type = db.mimeTypeForFile(QString(FileToLoad));
	if (type.name() == "application/octet-stream")
		return nullptr;

    QTextEdit *view = new QTextEdit((QWidget*)ParentWin);
    view->setAcceptRichText(true);

    QString qs = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir config_dir = QDir::cleanPath(qs + QDir::separator() + "highlightcommander");
    std::string utf8_text = config_dir.path().toUtf8().constData();
	string result = processFile(utf8_text , FileToLoad);

	view->setHtml(QString::fromStdString(result));
	view->setReadOnly(true);

    view->show();

	return view;
}

int DCPCALL ListLoadNext(HWND ParentWin, HWND PluginWin, char* FileToLoad, int ShowFlags)
{
	QMimeType type = db.mimeTypeForFile(QString(FileToLoad));

	if (type.name() == "application/octet-stream")
		return LISTPLUGIN_ERROR;

	QTextEdit *view = (QTextEdit*)PluginWin;

	QFile file(FileToLoad);

	if (!file.open(QFile::ReadOnly | QFile::Text))
		return LISTPLUGIN_ERROR;

    file.close();
    view->setAcceptRichText(true);

    QString qs = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir config_dir = QDir::cleanPath(qs + QDir::separator() + "highlightcommander");
    std::string utf8_text = config_dir.path().toUtf8().constData();
    string result = processFile(utf8_text , FileToLoad);

	view->setHtml(QString::fromStdString(result));
	view->setReadOnly(true);

    view->show();
    
	return LISTPLUGIN_OK;
}

void DCPCALL ListCloseWindow(HANDLE ListWin)
{
	QTextEdit *view = (QTextEdit*)ListWin;
	delete view;
}

int DCPCALL ListSearchText(HWND ListWin, char* SearchString, int SearchParameter)
{
	QTextDocument::FindFlags sflags;

	if (SearchParameter & lcs_matchcase)
		sflags |= QTextDocument::FindCaseSensitively;

	if (SearchParameter & lcs_backwards)
		sflags |= QTextDocument::FindBackward;

	QTextEdit *view = (QTextEdit*)ListWin;

	if (SearchParameter & lcs_findfirst)
	{
		QTextCursor cursor = view->textCursor();
		cursor.movePosition(SearchParameter & lcs_backwards ? QTextCursor::End : QTextCursor::Start);
		view->setTextCursor(cursor);
	}

// 	if (view->find(SearchString, sflags))
// 		return LISTPLUGIN_OK;
// 	else
// 		QMessageBox::information(view, "", QString::asprintf(_("\"%s\" not found!"), SearchString));

	return LISTPLUGIN_ERROR;
}

int DCPCALL ListSendCommand(HWND ListWin, int Command, int Parameter)
{
	QTextEdit *view = (QTextEdit*)ListWin;

	switch (Command)
	{
	case lc_copy :
		view->copy();
		break;

	case lc_selectall :
		view->selectAll();
		break;

	case lc_newparams :
		if (Parameter & lcp_wraptext)
			view->setLineWrapMode(QTextEdit::WidgetWidth);
		else
			view->setLineWrapMode(QTextEdit::NoWrap);

		break;

	default :
		return LISTPLUGIN_ERROR;
	}

	return LISTPLUGIN_OK;
}

void DCPCALL ListSetDefaultParams(ListDefaultParamStruct* dps)
{
  
}
