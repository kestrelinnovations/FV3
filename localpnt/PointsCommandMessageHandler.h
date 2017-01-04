// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// PointsCommandMessageHandler.h
//

class PointsCommandMessageHandler : public CCmdTarget
{
protected:
   afx_msg void OnLocalPtAdd();
   afx_msg void OnUpdateLocalPtAdd(CCmdUI* pCmdUI);
   afx_msg void OnLocalPtSelect();
   afx_msg void OnUpdateLocalPtSelect(CCmdUI* pCmdUI);
   afx_msg void OnLocalEditorTabularEditor();
   afx_msg void OnUpdateLocalEditorTabularEditor(CCmdUI* pCmdUI);
   afx_msg void OnLocalEditorDialogToggle();
   afx_msg void OnUpdateLocalEditorDialogToggle(CCmdUI* pCmdUI);
   afx_msg void OnLocalDragLock();
   afx_msg void OnUpdateLocalDragLock(CCmdUI* pCmdUI);
   afx_msg void OnDisplayLocalPointIconDialog();

   DECLARE_MESSAGE_MAP()
};
