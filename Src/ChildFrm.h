/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
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
 * @file  ChildFrm.h
 *
 * @brief interface of the CChildFrame class
 *
 */
#pragma once

#include "SplitterWndEx.h"
#include "MergeEditStatus.h"
#include "EditorFilepathBar.h"
#include "DiffViewBar.h"
#include "LocationBar.h"

class CMergeDoc;

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Operations
public:
	void UpdateResources();
	void CloseNow();
	IHeaderBar * GetHeaderInterface();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; };
	CMergeDoc * GetMergeDoc() { return m_pMergeDoc; }
	void SetLastCompareResult(int nResult);

	void UpdateAutoPaneResize();
	void UpdateSplitter();


// Attributes
protected:
	CSplitterWndEx m_wndSplitter;
	CEditorFilePathBar m_wndFilePathBar;
	CDiffViewBar m_wndDetailBar;
	CSplitterWndEx m_wndDetailSplitter;
	CStatusBar m_wndStatusBar;
	CLocationBar m_wndLocationBar;
	// Object that displays status line info for one side of a merge view
	class MergeStatus : public IMergeEditStatus
	{
	public:
		// ctr
		MergeStatus();
		// Implement MergeEditStatus
		void SetLineInfo(LPCTSTR szLine, int nColumn, int nColumns,
			int nChar, int nChars, LPCTSTR szEol, int nCodepage, bool bHasBom);
		void UpdateResources();
	protected:
		void Update();
	public:
		CChildFrame * m_pFrame;
		int m_base; /**< 0 for left, 1 for right */
	private:
		String m_sLine;
		int m_nColumn; /**< Current column, tab-expanded */
		int m_nColumns; /**< Amount of columns, tab-expanded */
		int m_nChar; /**< Current char */
		int m_nChars; /**< Amount of chars in line */
		int m_nCodepage;
		bool m_bHasBom;
		String m_sEol;
		String m_sEolDisplay;
		String m_sCodepageName;
	};
	friend class MergeStatus; // MergeStatus accesses status bar
	MergeStatus m_status[3];



// Overrides
public:
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	protected:
	//}}AFX_VIRTUAL

// Implementation
private:
	BOOL EnsureValidDockState(CDockState& state);
	void SavePosition();
	virtual ~CChildFrame();

// Generated message map functions
private:
	int m_nLastSplitPos;
	void UpdateHeaderSizes();
	BOOL m_bActivated;
	CMergeDoc * m_pMergeDoc;
	HICON m_hIdentical;
	HICON m_hDifferent;

	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnViewSplitVertically();
	afx_msg void OnUpdateViewSplitVertically(CCmdUI* pCmdUI);
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
