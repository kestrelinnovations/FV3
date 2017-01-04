// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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



// FontDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontDlg.h"
#include "ovlelem.h"    // OvlFont

/////////////////////////////////////////////////////////////////////////////
// CFontDlg - wraps the FVFontDialog COM object

CFontDlg::CFontDlg(int limit_types) :
   m_callback(NULL),
   m_pCallback(NULL)
{
	initialize(limit_types);
}
   
CFontDlg::CFontDlg(OvlFont font, int limit_types)
{
	initialize(limit_types);
   set_font(font);
}

void CFontDlg::initialize(int limit_types)
{
   m_lEnableRgb = 0;
	m_event_sink.set_parent(this);
	m_font_dialog.CreateInstance(__uuidof(FVFontDialog));

	if (limit_types)
		m_font_dialog->LimitBgTypes();
	
	IConnectionPoint *connection_point;
	IConnectionPointContainerPtr con_pt_container = m_font_dialog;
	con_pt_container->FindConnectionPoint(__uuidof(_IFVFontDialogEvents) ,&connection_point);
	
	LPDISPATCH pEventSink = m_event_sink.GetIDispatch(FALSE);
	connection_point->Advise(pEventSink, &m_cookie);
	
   connection_point->Release();
}

CFontDlg::~CFontDlg() 
{
	IConnectionPoint *connection_point;
	IConnectionPointContainerPtr con_pt_container = m_font_dialog;
	con_pt_container->FindConnectionPoint(__uuidof(_IFVFontDialogEvents) ,&connection_point);

	connection_point->Unadvise(m_cookie);
	
	connection_point->Release();
	m_font_dialog.Release();
}

void CFontDlg::EnableRgb(long lEnableRgb)
{
   m_font_dialog->EnableRgb(lEnableRgb);
   m_lEnableRgb = lEnableRgb;
}

// set the foreground color
void CFontDlg::SetColor(int color)
{
	m_font_dialog->SetColor(color);
}

// set the fontname
void CFontDlg::SetFont(char *fontname)
{
	m_font_dialog->SetFont(_bstr_t(fontname));
}

// set the pointsize
void CFontDlg::SetPointSize(int point_size)
{
	m_font_dialog->SetPointSize(point_size);
}

// set the attributes (bold, italic, underline, and strikeout
void CFontDlg::SetAttributes(int attribs)
{
   m_font_dialog->SetAttributes(attribs);
}
void CFontDlg::SetBackground(int background)
{
   m_font_dialog->SetBackground(background);
}

void CFontDlg::SetBackgroundColor(int color)
{
   m_font_dialog->SetBackgroundColor(color);
}

int CFontDlg::GetColor(void)
{
	int color;
	m_font_dialog->GetColor(&color);
   return color;
}

CString CFontDlg::GetFont(void)
{
   _bstr_t bstrFont;
	m_font_dialog->GetFont(bstrFont.GetAddress());
   return CString((char *)bstrFont);
}

int CFontDlg::GetPointSize(void)
{
   int point_size;
	m_font_dialog->GetPointSize(&point_size);
	return point_size;
}
int CFontDlg::GetAttributes(void)
{
   int attr;
	m_font_dialog->GetAttributes(&attr);
	return attr;
}
int CFontDlg::GetBackground(void)
{
	int bg;
	m_font_dialog->GetBackground(&bg);
   return bg;
}

int CFontDlg::GetBackgroundColor(void)
{
   int bg_color;
	m_font_dialog->GetBackgroundColor(&bg_color);
   return bg_color;
}

// set_font takes an OvlFont and initializes the dialog box accordingly
void CFontDlg::set_font(OvlFont font)
{
   CString fontname;
   int size, attributes;
   int fg_color, bg_color, type;
   
   font.get_font(fontname, size, attributes);

   SetFont(fontname.GetBuffer(fontname.GetLength()));
   SetPointSize(size);
   SetAttributes(attributes);

   if (font.color_type_is_RGB())
   {
      int type;
      COLORREF fg_color, bg_color;
      font.get_foreground_RGB(fg_color);
      font.get_background_RGB(type, bg_color);
      SetColor((int)fg_color);
      SetBackground(type);
      SetBackgroundColor((int)bg_color);
      EnableRgb(TRUE);
   }
   else
   {
      font.get_foreground(fg_color);
      font.get_background(type, bg_color);

      SetColor(fg_color);
      SetBackground(type);
      SetBackgroundColor(bg_color);
   }
}

boolean_t CFontDlg::is_modified(void) 
{ 
	return m_font_dialog->IsModified();
}

void CFontDlg::set_modified(boolean_t modified) 
{ 
	m_font_dialog->SetModified(modified);
}

int CFontDlg::DoModal()
{
	return m_font_dialog->DoModal();
}

// returns an OvlFont that contains the data currently in the dialog box
OvlFont CFontDlg::GetOvlFont()
{
   int redraw = TRUE;      // required in the OvlFont constructor
   OvlFont font(redraw);
   font = *this;
   return font;
}

void CFontDlg::apply_font()
{
	// static function based callback
   if (m_callback != NULL)
      m_callback(this);

   // interface based callback
   if (m_pCallback != NULL)
      m_pCallback->OnApplyFont(this);
}

/////////////////////////////////////////////////////////////////////////////
// CFontDialogEventSink

IMPLEMENT_DYNCREATE(CFontDialogEventSink, CCmdTarget)

CFontDialogEventSink::CFontDialogEventSink()
{
	EnableAutomation();
}

void CFontDialogEventSink::set_parent(CFontDlg *p)
{
   m_parent = p;
}

BEGIN_MESSAGE_MAP(CFontDialogEventSink, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFontDialogEventSink, CCmdTarget)
	DISP_FUNCTION(CFontDialogEventSink, "OnApply", OnApply,VT_EMPTY,VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CFontDialogEventSink, CCmdTarget)
	INTERFACE_PART(CFontDialogEventSink, __uuidof(_IFVFontDialogEvents), Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontDialogEventSink message handlers

// This event gets fired when the user click Apply or OK in the font dialog
void CFontDialogEventSink::OnApply()
{
	m_parent->apply_font();
}
