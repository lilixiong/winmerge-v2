/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  FileActionScript.cpp
 *
 * @brief Implementation of FileActionScript and related classes
 */

#include "stdafx.h"
#include "FileActionScript.h"
#include <vector>
#include "UnicodeString.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "MainFrm.h"
#include "ShellFileOperations.h"
#include "paths.h"
#include "SourceControl.h"

using std::vector;

/**
 * @brief Standard constructor.
 */
FileActionScript::FileActionScript()
: m_bUseRecycleBin(TRUE)
, m_bHasCopyOperations(FALSE)
, m_bHasMoveOperations(FALSE)
, m_bHasRenameOperations(FALSE)
, m_bHasDelOperations(FALSE)
, m_hParentWindow(NULL)
, m_pCopyOperations(new ShellFileOperations())
, m_pMoveOperations(new ShellFileOperations())
, m_pRenameOperations(new ShellFileOperations())
, m_pDelOperations(new ShellFileOperations())
{
}

/**
 * @brief Standard destructor.
 */
FileActionScript::~FileActionScript()
{
}

/**
 * @brief Remove last action item from the list.
 * @return Item removed from the list.
 */
FileActionItem FileActionScript::RemoveTailActionItem()
{
	FileActionItem item = m_actions.back();
	m_actions.pop_back();
	return item;
}

/**
 * @brief Set parent window used for showing MessageBoxes.
 * @param [in] hWnd Handle to parent window.
 */
void FileActionScript::SetParentWindow(HWND hWnd)
{
	m_hParentWindow = hWnd;
}

/**
 * @brief Does user want to move deleted files to Recycle Bin?
 * @param [in] bUseRecycleBin If TRUE deleted files are moved to Recycle Bin.
 */
void FileActionScript::UseRecycleBin(BOOL bUseRecycleBin)
{
	m_bUseRecycleBin = bUseRecycleBin;
}

/**
 * @brief Return amount of actions (copy, move, etc) in script.
 * @return Amount of actions.
 */
size_t FileActionScript::GetActionItemCount() const
{
	return m_actions.size();
}

/**
 * @brief Checkout file from VSS before synching (copying) it.
 * @param [in] path Full path to a file.
 * @param [in,out] bApplyToAll Apply user selection to all (selected)files?
 * @return One of CreateScriptReturn values.
 */
int FileActionScript::VCSCheckOut(const String &path, BOOL &bApplyToAll)
{
	String strErr;
	int retVal = SCRIPT_SUCCESS;

	if (GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM) == SourceControl::VCS_NONE)
		return retVal;

	// TODO: First param is not used!
	int nRetVal = theApp.SyncFileToVCS(path.c_str(), bApplyToAll, strErr);
	if (nRetVal == -1)
	{
		retVal = SCRIPT_FAIL; // So we exit without file operations done
		AfxMessageBox(strErr.c_str(), MB_OK | MB_ICONERROR);
	}
	else if (nRetVal == IDCANCEL)
	{
		retVal = SCRIPT_USERCANCEL; // User canceled, so we don't continue
	}
	else if (nRetVal == IDNO)
	{
		retVal = SCRIPT_USERSKIP;  // User wants to skip this item
	}

	return retVal;
}

/**
 * @brief Create ShellFileOperations operation lists from our scripts.
 *
 * We use ShellFileOperations internally to do actual file operations.
 * ShellFileOperations can do only one type of operation (copy, move, delete)
 * with one instance at a time, so we use own instance for every
 * type of action.
 * @return One of CreateScriptReturn values.
 */
int FileActionScript::CreateOperationsScripts()
{
	UINT operation = 0;
	FILEOP_FLAGS operFlags = 0;
	BOOL bApplyToAll = FALSE;
	BOOL bContinue = TRUE;

	// Copy operations first
	operation = FO_COPY;
	operFlags |= FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOCONFIRMATION;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	vector<FileActionItem>::const_iterator iter = m_actions.begin();
	while (iter != m_actions.end() && bContinue == TRUE)
	{
		BOOL bSkip = FALSE;
		if ((*iter).atype == FileAction::ACT_COPY && !(*iter).dirflag)
		{
			// Handle VCS checkout
			// Before we can write over destination file, we must unlock
			// (checkout) it. This also notifies VCS system that the file
			// has been modified.
			if (GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM) != SourceControl::VCS_NONE)
			{
				int retVal = VCSCheckOut((*iter).dest, bApplyToAll);
				if (retVal == SCRIPT_USERCANCEL)
					bContinue = FALSE;
				else if (retVal == SCRIPT_USERSKIP)
					bSkip = TRUE;
				else if (retVal == SCRIPT_FAIL)
					bContinue = FALSE;
			}

			if (bContinue)
			{
				if (!theApp.CreateBackup(TRUE, (*iter).dest.c_str()))
				{
					String strErr = _("Error backing up file");
					AfxMessageBox(strErr.c_str(), MB_OK | MB_ICONERROR);
					bContinue = FALSE;
				}
			}
		}

		if ((*iter).atype == FileAction::ACT_COPY &&
			bSkip == FALSE && bContinue == TRUE)
		{
			m_pCopyOperations->AddSourceAndDestination((*iter).src, (*iter).dest);
			m_bHasCopyOperations = TRUE;
		}
		++iter;
	}
	if (bContinue == FALSE)
	{
		m_bHasCopyOperations = FALSE;
		m_pCopyOperations->Reset();
		return SCRIPT_USERCANCEL;
	}
	
	if (m_bHasCopyOperations)
		m_pCopyOperations->SetOperation(operation, operFlags, m_hParentWindow);

	// Move operations next
	operation = FO_MOVE;
	operFlags = FOF_MULTIDESTFILES;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		if ((*iter).atype == FileAction::ACT_MOVE)
		{
			m_pMoveOperations->AddSourceAndDestination((*iter).src, (*iter).dest);
			m_bHasMoveOperations = TRUE;
		}
		++iter;
	}
	if (m_bHasMoveOperations)
		m_pMoveOperations->SetOperation(operation, operFlags,  m_hParentWindow);

	// Rename operations nextbbbb
	operation = FO_RENAME;
	operFlags = FOF_MULTIDESTFILES;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		if ((*iter).atype == FileAction::ACT_RENAME)
		{
			m_pRenameOperations->AddSourceAndDestination((*iter).src, (*iter).dest);
			m_bHasRenameOperations = TRUE;
		}
		++iter;
	}
	if (m_bHasRenameOperations)
		m_pRenameOperations->SetOperation(operation, operFlags, m_hParentWindow);

	// Delete operations last
	operation = FO_DELETE;
	operFlags = 0;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		if ((*iter).atype == FileAction::ACT_DEL)
		{
			m_pDelOperations->AddSource((*iter).src);
			if (!(*iter).dest.empty())
				m_pDelOperations->AddSource((*iter).dest);
			m_bHasDelOperations = TRUE;
		}
		++iter;
	}
	if (m_bHasDelOperations)
		m_pDelOperations->SetOperation(operation, operFlags, m_hParentWindow);
	return SCRIPT_SUCCESS;
}

/**
 * @brief Run one operation set.
 * @param [in] oplist List of operations to run.
 * @param [out] userCancelled Did user cancel the operation?
 * @return true if the operation succeeded and finished.
 */
bool FileActionScript::RunOp(ShellFileOperations *oplist, bool & userCancelled)
{
	bool fileOpSucceed = false;
	__try
	{
		fileOpSucceed = oplist->Run();
		userCancelled = oplist->IsCanceled();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		fileOpSucceed = false;
	}
	return fileOpSucceed;
}

/**
 * @brief Execute fileoperations.
 * @return TRUE if all actions were done successfully, FALSE otherwise.
 */
BOOL FileActionScript::Run()
{
	// Now process files/directories that got added to list
	bool bFileOpSucceed = true;
	bool bUserCancelled = false;
	BOOL bRetVal = TRUE;

	CreateOperationsScripts();

	if (m_bHasCopyOperations)
	{
		vector<FileActionItem>::const_iterator iter = m_actions.begin();
		while (iter != m_actions.end())
		{
			if ((*iter).dirflag)
				paths_CreateIfNeeded((*iter).dest);
			++iter;
		}
		bFileOpSucceed = RunOp(m_pCopyOperations.get(), bUserCancelled);
	}

	if (m_bHasMoveOperations)
	{
		if (bFileOpSucceed && !bUserCancelled)
		{
			bFileOpSucceed = RunOp(m_pMoveOperations.get(), bUserCancelled);
		}
		else
			bRetVal = FALSE;
	}

	if (m_bHasRenameOperations)
	{
		if (bFileOpSucceed && !bUserCancelled)
		{
			bFileOpSucceed = RunOp(m_pRenameOperations.get(), bUserCancelled);
		}
		else
			bRetVal = FALSE;
	}

	if (m_bHasDelOperations)
	{
		if (bFileOpSucceed && !bUserCancelled)
		{
			bFileOpSucceed = RunOp(m_pDelOperations.get(), bUserCancelled);
		}
		else
			bRetVal = FALSE;
	}

	if (!bFileOpSucceed || bUserCancelled)
		bRetVal = FALSE;

	return bRetVal;
}
