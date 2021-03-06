/** 
 * @file  PropShell.h
 *
 * @brief Implementation file for Shell Options dialog.
 *
 */

#include "stdafx.h"
#include "PropShell.h"
#include "Merge.h"
#include "RegKey.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "DDXHelper.h"
#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Flags for enabling and mode of extension
#define CONTEXT_F_ENABLED 0x01
#define CONTEXT_F_ADVANCED 0x02
#define CONTEXT_F_SUBFOLDERS 0x04

// registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");


PropShell::PropShell(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropShell::IDD)
, m_bContextAdded(false)
, m_bContextAdvanced(false)
, m_bContextSubfolders(false)
{
}

BOOL PropShell::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	// Update shell extension checkboxes
	GetContextRegValues();
	AdvancedContextMenuCheck();
	SubfolderOptionCheck();
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void PropShell::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropShell)
	DDX_Check(pDX, IDC_EXPLORER_CONTEXT, m_bContextAdded);
	DDX_Check(pDX, IDC_EXPLORER_ADVANCED, m_bContextAdvanced);
	DDX_Check(pDX, IDC_EXPLORER_SUBFOLDERS, m_bContextSubfolders);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropShell, CPropertyPage)
	//{{AFX_MSG_MAP(PropShell)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropShell::ReadOptions()
{
	GetContextRegValues();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropShell::WriteOptions()
{
	SaveMergePath(); // saves context menu settings as well
}

/// Get registry values for ShellExtension
void PropShell::GetContextRegValues()
{
	CRegKeyEx reg;
	LONG retVal = 0;
	retVal = reg.Open(HKEY_CURRENT_USER, RegDir);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = string_format(_T("Failed to open registry key HKCU/%s:\n\t%d : %s"),
			RegDir, retVal, GetSysError(retVal).c_str());
		LogErrorString(msg);
		return;
	}

	// Read bitmask for shell extension settings
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);

	if (dwContextEnabled & CONTEXT_F_ENABLED)
		m_bContextAdded = true;

	if (dwContextEnabled & CONTEXT_F_ADVANCED)
		m_bContextAdvanced = true;

	if (dwContextEnabled & CONTEXT_F_SUBFOLDERS)
		m_bContextSubfolders = true;
}

/// Set registry values for ShellExtension
void PropShell::OnAddToExplorer()
{
	AdvancedContextMenuCheck();
	SubfolderOptionCheck();
}

/// Saves given path to registry for ShellExtension, and Context Menu settings
void PropShell::SaveMergePath()
{
	TCHAR temp[MAX_PATH] = {0};
	LONG retVal = 0;
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);

	CRegKeyEx reg;
	retVal = reg.Open(HKEY_CURRENT_USER, RegDir);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = string_format(_T("Failed to open registry key HKCU/%s:\n\t%d : %s"),
			RegDir, retVal, GetSysError(retVal).c_str());
		LogErrorString(msg);
		return;
	}

	// Save path to WinMerge(U).exe
	retVal = reg.WriteString(f_RegValuePath, temp);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = string_format(_T("Failed to set registry value %s:\n\t%d : %s"),
			f_RegValuePath, retVal, GetSysError(retVal).c_str());
		LogErrorString(msg);
	}

	// Determine bitmask for shell extension
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	if (m_bContextAdded)
		dwContextEnabled |= CONTEXT_F_ENABLED;
	else
		dwContextEnabled &= ~CONTEXT_F_ENABLED;

	if (m_bContextAdvanced)
		dwContextEnabled |= CONTEXT_F_ADVANCED;
	else
		dwContextEnabled &= ~CONTEXT_F_ADVANCED;

	if (m_bContextSubfolders)
		dwContextEnabled |= CONTEXT_F_SUBFOLDERS;
	else
		dwContextEnabled &= ~CONTEXT_F_SUBFOLDERS;

	retVal = reg.WriteDword(f_RegValueEnabled, dwContextEnabled);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = string_format(_T("Failed to set registry value %s to %d:\n\t%d : %s"),
			f_RegValueEnabled, dwContextEnabled, retVal, GetSysError(retVal).c_str());
		LogErrorString(msg);
	}
}

/// Enable/Disable "Advanced menu" checkbox.
void PropShell::AdvancedContextMenuCheck()
{
	if (IsDlgButtonChecked(IDC_EXPLORER_CONTEXT))
		GetDlgItem(IDC_EXPLORER_ADVANCED)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_EXPLORER_ADVANCED)->EnableWindow(FALSE);
		CheckDlgButton(IDC_EXPLORER_ADVANCED, FALSE);
		m_bContextAdvanced = false;
	}
}

/// Enable/Disable "Include subfolders by default" checkbox.
void PropShell::SubfolderOptionCheck()
{
	if (IsDlgButtonChecked(IDC_EXPLORER_CONTEXT))
		GetDlgItem(IDC_EXPLORER_SUBFOLDERS)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_EXPLORER_SUBFOLDERS)->EnableWindow(FALSE);
		CheckDlgButton(IDC_EXPLORER_SUBFOLDERS, FALSE);
		m_bContextSubfolders = false;
	}
}
