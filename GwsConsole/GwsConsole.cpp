
// GwsConsole.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "GwsConsole.h"
#include "MainFrm.h"

#include "GwsConsoleDoc.h"
#include "GwsConsoleView.h"
#include "GWSPropertyGridCtrl.h"
#include "..\Libs\SocketApi\CommonFacility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGwsConsoleApp

BEGIN_MESSAGE_MAP(CGwsConsoleApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CGwsConsoleApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CGwsConsoleApp construction

CGwsConsoleApp::CGwsConsoleApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("GwsConsole.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_udp_ready		= false;
	m_remote_port	= 0;
	m_local_port	= 0;
	m_UDP			= NULL;
}

CGwsConsoleApp::~CGwsConsoleApp()
{
	if (m_UDP) delete m_UDP;
}
// The one and only CGwsConsoleApp object

CGwsConsoleApp theApp;


// CGwsConsoleApp initialization

BOOL CGwsConsoleApp::InitInstance()
{
	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGwsConsoleDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGwsConsoleView));
	if (!pDocTemplate)
		return FALSE;
	pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CGwsConsoleApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CGwsConsoleApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CGwsConsoleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CGwsConsoleApp customization load/save methods

void CGwsConsoleApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CGwsConsoleApp::LoadCustomState()
{
	CMainFrame*	pMainFrame = (CMainFrame*) AfxGetMainWnd();
	this->m_props.m_oColor		= this->GetProfileInt(SECTION_APPERANCE,KEYWORD_BACK_COLOR,5);
	this->m_props.m_oFont		= this->GetProfileString(SECTION_APPERANCE,KEYWORD_TEXT_FONT);
	this->m_props.m_oTargetIP	= this->GetProfileString(SECTION_APPERANCE,KEYWORD_TARGET_IP);
	this->m_props.m_szCmd_help	= this->GetProfileString(SECTION_COMMAND,KEYWORD_CMD_HELP);

	this->m_props.m_scripts[INDEX_UBOOT].m_szScript = SCRIPT_UBOOT;
	this->m_props.m_scripts[INDEX_UBOOT].m_AddCommand(KEYWORD_UBOOT_SETIP	, ACTION_SET_SVR_IP);
	this->m_props.m_scripts[INDEX_UBOOT].m_AddCommand(KEYWORD_UBOOT_TFTP	, ACTION_TFTP	, PROPERTY_EXPECT_TFTP);
	this->m_props.m_scripts[INDEX_UBOOT].m_AddCommand(KEYWORD_UBOOT_ERASE	, ACTION_ERASE	, PROPERTY_EXPECT_ERASED);
	this->m_props.m_scripts[INDEX_UBOOT].m_AddCommand(KEYWORD_UBOOT_WRITE	, ACTION_COPY	, PROPERTY_EXPECT_DONE);

	this->m_props.m_scripts[INDEX_KERNEL].m_szScript = SCRIPT_KERNEL;
	this->m_props.m_scripts[INDEX_KERNEL].m_AddCommand(KEYWORD_KERNEL_SETIP	, ACTION_SET_SVR_IP);
	this->m_props.m_scripts[INDEX_KERNEL].m_AddCommand(KEYWORD_KERNEL_TFTP	, ACTION_TFTP	, PROPERTY_EXPECT_TFTP);
	this->m_props.m_scripts[INDEX_KERNEL].m_AddCommand(KEYWORD_KERNEL_ERASE	, ACTION_ERASE	, PROPERTY_EXPECT_ERASED);
	this->m_props.m_scripts[INDEX_KERNEL].m_AddCommand(KEYWORD_KERNEL_WRITE	, ACTION_COPY	, PROPERTY_EXPECT_DONE);

	this->m_props.m_scripts[INDEX_FS].m_szScript = SCRIPT_FS;
	this->m_props.m_scripts[INDEX_FS].m_AddCommand(KEYWORD_FS_TFTP			, ACTION_TFTP	, PROPERTY_EXPECT_TFTP);
	this->m_props.m_scripts[INDEX_FS].m_AddCommand(KEYWORD_FS_ERASE			, ACTION_ERASE	, PROPERTY_EXPECT_ERASED);
	this->m_props.m_scripts[INDEX_FS].m_AddCommand(KEYWORD_FS_WRITE			, ACTION_COPY	, PROPERTY_EXPECT_DONE);

	this->m_props.m_scripts[INDEX_CALDATA].m_szScript = SCRIPT_CALDATA;
	this->m_props.m_scripts[INDEX_CALDATA].m_AddCommand(KEYWORD_ART_TFTP	, ACTION_TFTP	, PROPERTY_EXPECT_TFTP);
	this->m_props.m_scripts[INDEX_CALDATA].m_AddCommand(KEYWORD_ART_ERASE	, ACTION_ERASE	, PROPERTY_EXPECT_ERASED);
	this->m_props.m_scripts[INDEX_CALDATA].m_AddCommand(KEYWORD_ART_WRITE	, ACTION_COPY	, PROPERTY_EXPECT_DONE);

	this->m_props.m_scripts[INDEX_COMMON].m_szScript = SCRIPT_COMMON;
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Print Help Info")	, ACTION_HELP);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Start Web server")	, ACTION_HTTPD);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Ping Host IP")		, ACTION_PING);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Set Env Varialbe")	, ACTION_SET_ENV);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Show Env Varialbe")	, ACTION_SHOW_ENV);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Save Env Varialbe")	, ACTION_SAVE_ENV);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Show flash/memory")	, ACTION_SHOW_FLASH);
	this->m_props.m_scripts[INDEX_COMMON].m_AddCommand(_T("Reboot system")		, ACTION_RESET);
	
	for (int i = 0; i < MAX_SCRIPTS; i ++ ) {
		CGwsScript&	script = this->m_props.m_scripts[i];
		if (!script.m_szScript.IsEmpty()) {
			CObList&	list = script.m_oCommands;
			for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
				CGwsCommand* cmd = (CGwsCommand*) list.GetAt(pos);
				if (cmd && !cmd->m_szIndex.IsEmpty()) {
					cmd->m_szValue = this->GetProfileString(script.m_szScript, cmd->m_szIndex, cmd->m_szValue);
				}
			}
		}
	}

	for (int i = 0; i < MAX_USER_SCRIPTS; i ++) {
		CGwsScript&	script = this->m_props.m_sc_user[i];
		const int nCMD_PER_GROUP = MAX_USER_COMMANDS/MAX_USER_SCRIPTS;
		script.m_szScript.Format(_T("%s%d"), SCRIPT_USER, i + 1);
		for (int j = 0; j < nCMD_PER_GROUP; j ++) {
			int n = i * nCMD_PER_GROUP + j + 1;
			CString	szCommand;
			szCommand.Format(_T("command_%d"), n);
			CGwsCommand* cmd = script.m_AddCommand(szCommand, _T(""), PROPERTY_EXPECT_COMMON);
			cmd->m_szValue = this->GetProfileString(script.m_szScript, cmd->m_szIndex, cmd->m_szValue);
		}
	}
	if (pMainFrame)
		pMainFrame->m_wndProperties.InitPropList();
}

void CGwsConsoleApp::SaveCustomState()
{
	for (int i = 0; i < MAX_SCRIPTS; i ++ ) {
		CGwsScript&	script = this->m_props.m_scripts[i];
		if (!script.m_szScript.IsEmpty()) {
			CObList&	list = script.m_oCommands;
			for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
				CGwsCommand* cmd = (CGwsCommand*) list.GetAt(pos);
				if (cmd && !cmd->m_szIndex.IsEmpty()) {
					this->WriteProfileString(script.m_szScript, cmd->m_szIndex, cmd->m_szValue);
				}
			}
		}
	}
	for (int i = 0; i < MAX_USER_SCRIPTS; i ++ ) {
		CGwsScript&	script = this->m_props.m_sc_user[i];
		CObList&	list = script.m_oCommands;
		for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
			CGwsCommand* cmd = (CGwsCommand*) list.GetAt(pos);
			if (cmd && !cmd->m_szIndex.IsEmpty()) {
				this->WriteProfileString(script.m_szScript, cmd->m_szIndex, cmd->m_szValue);
			}
		}
	}
	this->WriteProfileInt(SECTION_APPERANCE,KEYWORD_BACK_COLOR	,this->m_props.m_oColor.V());
	this->WriteProfileString(SECTION_APPERANCE,KEYWORD_TEXT_FONT,this->m_props.m_oFont.V());
	this->WriteProfileString(SECTION_APPERANCE,KEYWORD_TARGET_IP,this->m_props.m_oTargetIP.V());
	this->WriteProfileString(SECTION_COMMAND,KEYWORD_CMD_HELP	,this->m_props.m_szCmd_help.V());
}

// CGwsConsoleApp message handlers


bool CGwsConsoleApp::m_InitNetwork(WORD local_port,WORD remote_port) {
	this->m_CloseNetwork();
	this->m_remote_port = remote_port;
	this->m_UDP = new CSocketDatagram(m_udp_buff, 6666);
	if( local_port > 0 )
	{
		this->m_udp_ready = this->m_UDP->Bind ("224.0.0.1",local_port);
	}
	return this->m_udp_ready;
}

bool CGwsConsoleApp::m_IsNetworkReady(void) {
	return (this->m_udp_ready && this->m_UDP) ? true : false;
}

void CGwsConsoleApp::m_CloseNetwork(void) {
	this->m_udp_ready = false;
	this->m_szScripts.RemoveAll();
	if (this->m_UDP) {
		delete this->m_UDP;
		this->m_UDP = NULL;
	}
}

CString	CGwsConsoleApp::m_UdpRecv(void) {
	int		nCount, nRecv = 0;
	CString	szRecv;
	if (this->m_udp_ready && this->m_UDP) {
		char* buff = this->m_UDP->GetBuffer();
		for (nCount = 0; nCount < 5; nCount ++) {
			if ((nRecv = this->m_UDP->Recv(	this->m_szRemoteIP,
											this->m_remote_port)) > 0) {
				nCount = 0;
				buff[nRecv] = 0;
				szRecv += MultiByteStr_WideStr(buff);
				if (this->m_props.m_oTargetIP != this->m_szRemoteIP) {
					this->m_props.m_oTargetIP = this->m_szRemoteIP;
					this->m_props.m_oTargetIP.m_UpdateProp();
				}
			}
		}
	}
	return szRecv;
}

int CGwsConsoleApp::m_UdpSend(CString szText) {
	DWORD	dwIP = IPconvertA2N(this->m_props.m_oTargetIP.V());
	CString	szSend = szText + _T("\n");
	char	sSend[128] = "";
	int		nSend = WideStr_MultiByteStr(szSend,sSend,127);
	//if (!szText.IsEmpty())
	//	this->m_szLastCommand = szText;
	//szSend = this->m_szLastCommand + _T("\n");
	if (this->m_udp_ready && this->m_UDP && dwIP) {
		return this->m_UDP->Send(dwIP, this->m_remote_port, sSend, nSend);
	}
	return 0;
}


bool CGwsConsoleApp::m_RunScript(CString szScript,CString szSpecifiedEntry) {
	CGwsProperties&				props = this->m_props;

	if (!this->m_IsNetworkReady()) {
		this->m_szScripts.RemoveAll();
		return false;
	} if (this->m_szScripts.GetCount() > 0) {
		return false;	//Last script still not done
	}
	for (int i = 0; i < MAX_SCRIPTS; i ++) {
		CGwsScript&	script = props.m_scripts[i];
		if (script.m_szScript == szScript) {
			CObList&	list = script.m_oCommands;
			for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
				CGwsCommand* command = (CGwsCommand*) list.GetAt(pos);
				CString	szEntry = command->m_szIndex;
				if (szEntry.IsEmpty()) continue;	//if entry specified, run that entry only
				//otherwise, run all entries in this script
				if (szSpecifiedEntry.IsEmpty() || szSpecifiedEntry == szEntry) {
					CString	szCommand = command->m_szAction + _T(" ") + command->m_szValue;
					CString	szExpected = command->m_szExpect;
					this->m_szScripts.AddTail(szCommand);
					if (!szExpected.IsEmpty())
						this->m_szScripts.AddTail(_T("[") + szExpected + _T("]"));
				}
			}
			return true;
		}
	}
	for (int i = 0; i < MAX_USER_SCRIPTS; i ++) {
		CGwsScript&	script = props.m_sc_user[i];
		if (script.m_szScript == szScript) {
			CObList&	list = script.m_oCommands;
			for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
				CGwsCommand* command = (CGwsCommand*) list.GetAt(pos);
				CString	szEntry = command->m_szIndex;
				if (szEntry.IsEmpty()) continue;	//if entry specified, run that entry only
				//otherwise, run all entries in this script
				if (szSpecifiedEntry.IsEmpty() || szSpecifiedEntry == szEntry) {
					CString	szCommand = command->m_szAction + _T(" ") + command->m_szValue;
					CString	szExpected = command->m_szExpect;
					if (!szCommand.IsEmpty()) {
						this->m_szScripts.AddTail(szCommand);
						if (!szExpected.IsEmpty())
							this->m_szScripts.AddTail(_T("[") + szExpected + _T("]"));
					}
				}
			}
			return true;
		}
	}
	return false;
}
