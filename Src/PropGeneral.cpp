/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PropGeneral.h
 *
 * @brief Implementation file for PropGeneral propertyheet
 *
 */

#include "stdafx.h"
#include "PropGeneral.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "LanguageSelect.h"
#include "DDXHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** 
 * @brief Constructor initialising members.
 */
PropGeneral::PropGeneral(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropGeneral::IDD)
, m_bScroll(false)
, m_bSingleInstance(false)
, m_bVerifyPaths(false)
, m_bCloseWindowWithEsc(TRUE)
, m_bAskMultiWindowClose(false)
, m_bMultipleFileCmp(false)
, m_bMultipleDirCmp(false)
, m_nAutoCompleteSource(0)
, m_bPreserveFiletime(false)
, m_bShowSelectFolderOnStartup(false)
, m_bCloseWithOK(true)
{
}

PropGeneral::~PropGeneral()
{
}

BOOL PropGeneral::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	CComboBox *pWnd = (CComboBox*)GetDlgItem(IDC_AUTO_COMPLETE_SOURCE);
	ASSERT(NULL != pWnd);

	pWnd->AddString(_("Disabled").c_str());
	pWnd->AddString(_("From file system").c_str());
	pWnd->AddString(_("From MRU list").c_str());

	pWnd->SetCurSel(m_nAutoCompleteSource);

	m_ctlLangList.SetDroppedWidth(600);

	for (auto&& i : theApp.m_pLangDlg->GetAvailableLanguages())
	{
		m_ctlLangList.AddString(i.second.c_str());
		m_ctlLangList.SetItemData(m_ctlLangList.GetCount() - 1, i.first);
		if (i.first == theApp.m_pLangDlg->GetLangId())
			m_ctlLangList.SetCurSel(m_ctlLangList.GetCount() - 1);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void PropGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropGeneral)
	DDX_Check(pDX, IDC_SCROLL_CHECK, m_bScroll);
	DDX_Check(pDX, IDC_SINGLE_INSTANCE, m_bSingleInstance);
	DDX_Check(pDX, IDC_VERIFY_OPEN_PATHS, m_bVerifyPaths);
	DDX_Check(pDX, IDC_ESC_CLOSES_WINDOW, m_bCloseWindowWithEsc);
	DDX_Check(pDX, IDC_ASK_MULTIWINDOW_CLOSE, m_bAskMultiWindowClose);
	DDX_Check(pDX, IDC_MULTIDOC_FILECMP, m_bMultipleFileCmp);
	DDX_Check(pDX, IDC_MULTIDOC_DIRCMP, m_bMultipleDirCmp);
	DDX_CBIndex(pDX, IDC_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource);
	DDX_Check(pDX, IDC_PRESERVE_FILETIME, m_bPreserveFiletime);
	DDX_Check(pDX, IDC_STARTUP_FOLDER_SELECT, m_bShowSelectFolderOnStartup);
	DDX_Check(pDX, IDC_CLOSE_WITH_OK, m_bCloseWithOK);
	DDX_Control(pDX, IDC_LANGUAGE_LIST, m_ctlLangList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(PropGeneral)
	ON_BN_CLICKED(IDC_RESET_ALL_MESSAGE_BOXES, OnResetAllMessageBoxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropGeneral::ReadOptions()
{
	m_bScroll = GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST);
	m_bSingleInstance = GetOptionsMgr()->GetBool(OPT_SINGLE_INSTANCE);
	m_bVerifyPaths = GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS);
	m_bCloseWindowWithEsc = GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_ESC);
	m_bAskMultiWindowClose = GetOptionsMgr()->GetBool(OPT_ASK_MULTIWINDOW_CLOSE);
	m_bMultipleFileCmp = GetOptionsMgr()->GetBool(OPT_MULTIDOC_MERGEDOCS);
	m_bMultipleDirCmp = GetOptionsMgr()->GetBool(OPT_MULTIDOC_DIRDOCS);
	m_nAutoCompleteSource = GetOptionsMgr()->GetInt(OPT_AUTO_COMPLETE_SOURCE);
	m_bPreserveFiletime = GetOptionsMgr()->GetBool(OPT_PRESERVE_FILETIMES);
	m_bShowSelectFolderOnStartup = GetOptionsMgr()->GetBool(OPT_SHOW_SELECT_FILES_AT_STARTUP);
	m_bCloseWithOK = GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_OK);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropGeneral::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_SCROLL_TO_FIRST, m_bScroll);
	GetOptionsMgr()->SaveOption(OPT_SINGLE_INSTANCE, m_bSingleInstance);
	GetOptionsMgr()->SaveOption(OPT_VERIFY_OPEN_PATHS, m_bVerifyPaths);
	GetOptionsMgr()->SaveOption(OPT_CLOSE_WITH_ESC, m_bCloseWindowWithEsc);
	GetOptionsMgr()->SaveOption(OPT_ASK_MULTIWINDOW_CLOSE, m_bAskMultiWindowClose);
	GetOptionsMgr()->SaveOption(OPT_MULTIDOC_MERGEDOCS, m_bMultipleFileCmp);
	GetOptionsMgr()->SaveOption(OPT_MULTIDOC_DIRDOCS, m_bMultipleDirCmp);
	GetOptionsMgr()->SaveOption(OPT_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource);
	GetOptionsMgr()->SaveOption(OPT_PRESERVE_FILETIMES, m_bPreserveFiletime);
	GetOptionsMgr()->SaveOption(OPT_SHOW_SELECT_FILES_AT_STARTUP, m_bShowSelectFolderOnStartup);
	GetOptionsMgr()->SaveOption(OPT_CLOSE_WITH_OK, m_bCloseWithOK);
	int index = m_ctlLangList.GetCurSel();
	if (index >= 0)
	{
		WORD lang = (WORD)m_ctlLangList.GetItemData(index);
		GetOptionsMgr()->SaveOption(OPT_SELECTED_LANGUAGE, (int)lang);
	}
}

/** 
 * @brief Called when user wants to see all messageboxes again.
 */
void PropGeneral::OnResetAllMessageBoxes()
{
	CMessageBoxDialog::ResetMessageBoxes();
	LangMessageBox(IDS_MESSAGE_BOX_ARE_RESET, MB_ICONINFORMATION);
}
