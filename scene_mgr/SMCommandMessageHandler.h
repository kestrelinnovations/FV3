// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// SMCommandMessageHandler.h
//

#ifndef SMCOMMANDMESSAGEHANDLER_H
#define SMCOMMANDMESSAGEHANDLER_H


namespace scene_mgr
{

class SMCommandMessageHandler : public CCmdTarget
{
protected:
   afx_msg void OnSmToolSelcopy();
   afx_msg void OnSmToolSeldelete();
   afx_msg void OnUpdateSmToolSeldelete(CCmdUI* pCmdUI);
   afx_msg void OnUpdateSmToolSelcopy(CCmdUI* pCmdUI);
   afx_msg void OnSceneManager();
   afx_msg void OnSmToolCompatible();
   afx_msg void OnUpdateSmToolCompatible(CCmdUI* pCmdUI);

   DECLARE_MESSAGE_MAP()
};

};  // namespace scene_mgr

#endif