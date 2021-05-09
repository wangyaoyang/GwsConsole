
// GwsConsole.h : main header file for the GwsConsole application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "..\Libs\SocketApi\SOCKET.H"


#define	SECTION_APPERANCE	_T("Appearance")
#define	KEYWORD_BACK_COLOR	_T("Background Color")
#define	KEYWORD_TEXT_FONT	_T("Text Font")
#define	KEYWORD_TARGET_IP	_T("Device IP Address")

#define	SECTION_COMMAND		_T("Command")
#define	KEYWORD_CMD_HELP	_T("CMD help")

#define	ACTION_SET_SVR_IP		_T("set serverip")
#define	ACTION_TFTP				_T("tftp")
#define	ACTION_ERASE			_T("erase")
#define	ACTION_COPY				_T("cp.b")
#define	ACTION_HELP				_T("help")
#define	ACTION_HTTPD			_T("httpd")
#define	ACTION_PING				_T("ping")
#define	ACTION_SET_ENV			_T("setenv")
#define	ACTION_SHOW_ENV			_T("printenv")
#define	ACTION_SAVE_ENV			_T("saveenv")
#define	ACTION_SHOW_FLASH		_T("md.b")
#define	ACTION_RESET			_T("reset")

#define	INDEX_UBOOT				0
#define	SCRIPT_UBOOT			_T("upgrade uboot")
#define	KEYWORD_UBOOT_SETIP		_T("set server ip")
#define	KEYWORD_UBOOT_TFTP		_T("uboot download")
#define	KEYWORD_UBOOT_ERASE		_T("uboot erase")
#define	KEYWORD_UBOOT_WRITE		_T("uboot write")

#define	INDEX_KERNEL			1
#define	SCRIPT_KERNEL			_T("upgrade kernel")
#define	KEYWORD_KERNEL_SETIP	KEYWORD_UBOOT_SETIP
#define	KEYWORD_KERNEL_TFTP		_T("kernel download")
#define	KEYWORD_KERNEL_ERASE	_T("kernel erase")
#define	KEYWORD_KERNEL_WRITE	_T("kernel write")

#define	INDEX_FS				2
#define	SCRIPT_FS				_T("upgrade fs")
#define	KEYWORD_FS_TFTP			_T("fs download")
#define	KEYWORD_FS_ERASE		_T("fs erase")
#define	KEYWORD_FS_WRITE		_T("fs write")

#define	INDEX_CALDATA			3
#define	SCRIPT_CALDATA			_T("upgrade caldata")
#define	KEYWORD_ART_TFTP		_T("caldata download")
#define	KEYWORD_ART_ERASE		_T("caldata erase")
#define	KEYWORD_ART_WRITE		_T("caldata write")

#define	INDEX_COMMON			4
#define	SCRIPT_COMMON			_T("Utilities")

#define	MAX_SCRIPTS				5

#define	SCRIPT_USER				_T("User Group")

#define	MAX_USER_SCRIPTS	10
#define	MAX_USER_COMMANDS	40

// CGwsConsoleApp:
// See GwsConsole.cpp for the implementation of this class
//
class CGwsCommand : public CObject {
public:
	CString			m_szAction;
	CString			m_szIndex;
	CString			m_szValue;
	CString			m_szExpect;
};

class CGwsScript {
public:
	CString		m_szScript;
//	CGwsCommand	m_oCommands[MAX_COMMANDS];
	CObList		m_oCommands;
public:
	CGwsScript(void) {}
	~CGwsScript(void) {
		while (!this->m_oCommands.IsEmpty())
			delete ((CGwsCommand*) this->m_oCommands.RemoveHead());
	}
	CGwsCommand* m_AddCommand(CString szIndex, CString szAction, CString szExpect = _T("")) {
		CGwsCommand*	command = new CGwsCommand();
		command->m_szIndex = szIndex;
		command->m_szAction = szAction;
		command->m_szExpect = szExpect;
		this->m_oCommands.AddTail(command);
		return command;
	}
};

class CMFCPropertyGridProperty;
template <class T>
class CVParameter {
private:
	CString		m_szFormat;
	CString		m_szValue;
	T			m_oValue;
public:
	CMFCPropertyGridProperty*	m_pProp;
public:
	CVParameter(void) {
		const std::type_info&	ti = typeid(this->m_oValue);
		const char* sType = ti.name();

		if (strstr(sType, "String"))					{ this->m_szFormat = _T("%s");
		} else if (strstr(sType, "unsigned char"))		{ this->m_szFormat = _T("%hhu");
		} else if (strstr(sType, "unsigned short"))		{ this->m_szFormat = _T("%hu");
		} else if (strstr(sType, "unsigned int"))		{ this->m_szFormat = _T("%u");
		} else if (strstr(sType, "unsigned long"))		{ this->m_szFormat = _T("%ul");
		} else if (strstr(sType, "unsigned __int64"))	{ this->m_szFormat = _T("%l64u");
		} else if (strstr(sType, "__int64"))			{ this->m_szFormat = _T("%l64d");		
		} else if (strstr(sType, "bool"))				{ this->m_szFormat = _T("%u");
		} else if (strstr(sType, "char"))				{ this->m_szFormat = _T("%c");
		} else if (strstr(sType, "wchar_t"))			{ this->m_szFormat = _T("%lc");
		} else if (strstr(sType, "short"))				{ this->m_szFormat = _T("%hd");
		} else if (strstr(sType, "int"))				{ this->m_szFormat = _T("%d");
		} else if (strstr(sType, "long"))				{ this->m_szFormat = _T("%l");
		} else if (strstr(sType, "float"))				{ this->m_szFormat = _T("%f");
		} else if (strstr(sType, "double"))				{ this->m_szFormat = _T("%lf");
		}
		this->m_pProp = NULL;
	}
	CVParameter(T value) { *this = value; }
	CVParameter* operator = (T value) {
		this->m_oValue = value;
		return this;
	}
	virtual void operator << (CString szValue)  {
		if (!this->m_szFormat.IsEmpty()) {
			if (this->m_szFormat == _T("%s")) {
				CString& sz = *(CString*) &this->m_oValue;
				sz = szValue;
			} else uniscanf(szValue, this->m_szFormat, &this->m_oValue);
		}
	}
	virtual void m_UpdateProp(void) {
		if (this->m_pProp) {
			this->m_pProp->SetValue((_variant_t) this->m_oValue);
		}
	}
	virtual CString	Str(void) {
		CString	szValue;
		if (this->m_szFormat.IsEmpty() == false) {
			if (this->m_szFormat == _T("%s"))
				return *(CString*) &this->m_oValue;
			else szValue.Format(this->m_szFormat, this->m_oValue);
		}
		return szValue;
	}
	void operator = (CVParameter& src) {
		const std::type_info&	ti = typeid(this->m_oValue);
		const std::type_info&	ts = typeid(src.m_oValue);

		if (ti.hash_code() == ts.hash_code()) {
			this->m_oValue = src.m_oValue;
			this->m_szFormat = src.m_szFormat;
		}
	}
	void operator += (T value) {
		this->m_oValue += value;
		this->m_szValue.Format(this->m_szFormat, this->m_oValue);
	}
	bool operator == (CVParameter& value) {
		const std::type_info&	ti = typeid(this->m_oValue);
		const std::type_info&	ts = typeid(value.m_oValue);

		if (ti.hash_code() != ts.hash_code()) return false;
		if (this->m_oValue != value.m_oValue) return false;
		return true;
	}
	bool operator != (CVParameter& value) {
		return !(*this == value);
	}
	bool operator == (T value) { return !!(this->m_oValue == value); }
	bool operator != (T value) { return !(this->m_oValue == value); }
	T&	V(void) { return m_oValue; }
};


class CGwsProperties {
public:
	CVParameter<COLORREF>	m_oColor;
	CVParameter<CString>	m_oFont;
	CVParameter<CString>	m_oTargetIP;
	CVParameter<CString>	m_szCmd_help;
	CGwsScript	m_scripts[MAX_SCRIPTS];
	CGwsScript	m_sc_user[MAX_USER_SCRIPTS];
public:
	CGwsProperties() {
		m_oColor = RGB(0,0,0);
	}
};

class CGwsConsoleApp : public CWinAppEx
{
public:
	CGwsConsoleApp();
	~CGwsConsoleApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:
	bool				m_udp_ready;
	char				m_udp_buff[MAX_BUFF_SIZE];
	CString				m_szRemoteIP;
	WORD				m_remote_port;
	WORD				m_local_port;
	CSocketDatagram*	m_UDP;
public:
	CGwsProperties		m_props;
	CStringList			m_szScripts;
public:
	bool m_InitNetwork(WORD local_port,WORD remote_port);
	bool m_IsNetworkReady(void);
	void m_CloseNetwork(void);
	CString	m_UdpRecv(void);
	int m_UdpSend(CString szText);
	bool m_RunScript(CString szScript,CString szSpecifiedEntry);
};

extern CGwsConsoleApp theApp;

