// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// layout.cpp : implementation file

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/layout.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/getobjpr.h"
#include "FalconView/include/fvwutil.h"

// this project's headers
#include "FalconView/PrintToolOverlay/PrntPage.h"

#define PIXELS_FROM_EDGE   30

CPageLayoutDlg::CPageLayoutDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CPageLayoutDlg::IDD, pParent)
{
   // {{AFX_DATA_INIT(CPageLayoutDlg)
   m_value_num_rows = 0;
   m_value_num_columns = 0;
   m_value_horz_spacing = 0.0;
   m_value_vert_spacing = 0.0;
   // }}AFX_DATA_INIT

   m_vpage_layout_is_initialized = FALSE;
}

void CPageLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   // {{AFX_DATA_MAP(CPageLayoutDlg)
   DDX_Control(pDX, IDC_BITMAP, m_strip_page_bitmap);
   DDX_Control(pDX, IDC_VERT_SPACING, m_vert_spacing);
   DDX_Control(pDX, IDC_SPIN_VERT_SPACING, m_spin_vert_spacing);
   DDX_Control(pDX, IDC_SPIN_ROWS, m_spin_rows);
   DDX_Control(pDX, IDC_SPIN_HORZ_SPACING, m_spin_horz_spacing);
   DDX_Control(pDX, IDC_SPIN_COLUMNS, m_spin_columns);
   DDX_Control(pDX, IDC_NUM_ROWS, m_num_rows);
   DDX_Control(pDX, IDC_NUM_COLUMNS, m_num_columns);
   DDX_Control(pDX, IDC_HORZ_SPACING, m_horz_spacing);
   DDX_Text(pDX, IDC_NUM_ROWS, m_value_num_rows);
   DDV_MinMaxInt(pDX, m_value_num_rows, 1, 10000);
   DDX_Text(pDX, IDC_NUM_COLUMNS, m_value_num_columns);
   DDV_MinMaxInt(pDX, m_value_num_columns, 1, 10000);
   DDX_Text(pDX, IDC_HORZ_SPACING, m_value_horz_spacing);
   DDV_MinMaxDouble(pDX, m_value_horz_spacing, 0., 1.);
   DDX_Text(pDX, IDC_VERT_SPACING, m_value_vert_spacing);
   DDV_MinMaxDouble(pDX, m_value_vert_spacing, 0., 1.);
   // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageLayoutDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   // {{AFX_MSG_MAP(CPageLayoutDlg)
   ON_WM_PAINT()
   ON_EN_KILLFOCUS(IDC_NUM_COLUMNS, OnChangeNumColumns)
   ON_EN_KILLFOCUS(IDC_NUM_ROWS, OnChangeNumRows)
   ON_EN_KILLFOCUS(IDC_HORZ_SPACING, OnChangeHorzSpacing)
   ON_EN_KILLFOCUS(IDC_VERT_SPACING, OnChangeVertSpacing)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROWS, OnDeltaposSpinRows)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COLUMNS, OnDeltaposSpinColumns)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HORZ_SPACING, OnDeltaposSpinHorzSpacing)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VERT_SPACING, OnDeltaposSpinVertSpacing)
   ON_BN_CLICKED(IDC_BUTTON2, OnPrintSetup)
   ON_BN_CLICKED(IDC_PORTRAIT, OnChangeOrientation)
   ON_BN_CLICKED(IDC_LANDSCAPE, OnChangeOrientation)
   ON_BN_CLICKED(IDC_DEFAULT, OnChangeOrientation)
   ON_BN_CLICKED(IDC_LAYOUT_HELP, OnHelp)
   // }}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPageLayoutDlg::OnInitDialog()
{
   // make sure the virtual page layout object has been initialized first
   if (!m_vpage_layout_is_initialized)
   {
      TRACE("Initialize the virtual page layout object using "
         "set_virtual_page_layout() before calling CPageLayoutDlg::DoModal.\n");
      ASSERT(0);
   }

   m_value_num_rows = m_vpage_layout.get_num_rows();
   m_value_num_columns = m_vpage_layout.get_num_columns();

   m_value_horz_spacing = m_vpage_layout.get_horz_spacing();
   m_value_vert_spacing = m_vpage_layout.get_vert_spacing();

   CDialog::OnInitDialog();

   // attach the spin controls to the edit windows
   m_spin_rows.SetBuddy(&m_num_rows);
   m_spin_columns.SetBuddy(&m_num_columns);
   m_spin_horz_spacing.SetBuddy(&m_horz_spacing);
   m_spin_vert_spacing.SetBuddy(&m_vert_spacing);

   // set the spin control ranges
   m_spin_rows.SetRange(1, 20);
   m_spin_rows.SetPos(m_vpage_layout.get_num_rows());
   m_spin_columns.SetRange(1, 20);
   m_spin_columns.SetPos(m_vpage_layout.get_num_columns());

   // store the bitmap resource names
   m_bitmap[0] = CreateMappedBitmap(AfxGetInstanceHandle(),
      IDB_PRNT_PORTRAIT, 0, NULL, 0);
   m_bitmap[1] = CreateMappedBitmap(AfxGetInstanceHandle(),
      IDB_PRNT_LANDSCAPE, 0, NULL, 0);

   // set the appropriate bitmap for the strip chart page orientation controls
   {
      // get the current orientation from the page layout object
      page_orientation_t orientation = m_vpage_layout.get_orientation();

      // portrait / landscape bitmap
      if (m_vpage_layout.get_page_width() < m_vpage_layout.get_page_height())
      {
         m_scale_x = SQUASH;
         m_scale_y = 1.0;
         m_strip_page_bitmap.SetBitmap(m_bitmap[0]);
      }
      else
      {
         m_scale_x = 1.0;
         m_scale_y = SQUASH;
         m_strip_page_bitmap.SetBitmap(m_bitmap[1]);
      }

      // strip chart page orientation
      (reinterpret_cast<CButton *>(GetDlgItem(IDC_PORTRAIT))->SetCheck(
         orientation == PAGE_ORIENTATION_PORTRAIT));
      (reinterpret_cast<CButton *>(GetDlgItem(IDC_LANDSCAPE))->SetCheck(
         orientation == PAGE_ORIENTATION_LANDSCAPE));
      (reinterpret_cast<CButton *>(GetDlgItem(IDC_DEFAULT))->SetCheck(
         orientation == PAGE_ORIENTATION_DEFAULT));
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageLayoutDlg::OnHelp()
{
   // Translate the message into the AFX standard help command;
   // this is equivalent to hitting F1 with this dialog box in focus
   GetParent()->SendMessage(WM_COMMAND, ID_HELP, 0);
}

void CPageLayoutDlg::OnPaint()
{
   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;

   CDialog::OnPaint();
   draw_preview(hwnd);
}

// draw the preview of the page layout
void CPageLayoutDlg::draw_preview(HWND hwnd)
{
   CRect client_rect;
   CWnd *wnd = CWnd::FromHandle(hwnd);
   CDC memory_dc;
   CBitmap bmp;

   CDC *pDC = wnd->GetDC();
   if (pDC)
   {
      wnd->GetClientRect(&client_rect);

      // create an equivalent dc in memory
      memory_dc.CreateCompatibleDC(pDC);
      bmp.CreateCompatibleBitmap(pDC,
         client_rect.Width(), client_rect.Height());
      memory_dc.SelectObject(&bmp);

      // draw the page
      draw_page(&memory_dc, client_rect);

      // draw the dashed virtual page dividers
      draw_dividers(&memory_dc, client_rect);

      // draw the text blocks denoting the page width and height
      draw_text(&memory_dc, client_rect);

      // draw arrows on each virtual page denoting which way is up
      draw_arrows(&memory_dc, client_rect);

      // copy contents of memory dc into the preview window's dc
      pDC->BitBlt(0, 0, client_rect.Width(),
         client_rect.Height(), &memory_dc, client_rect.TopLeft().x,
         client_rect.TopLeft().y, SRCCOPY);

      wnd->ReleaseDC(pDC);
   }
   else
      ERR_report("GetDC() failed.");
}

void CPageLayoutDlg::draw_page(CDC *pDC, CRect client_rect)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   CBrush dark_gray_brush(RGB(128, 128, 128));
   CBrush bright_white_brush(RGB(255, 255, 255));
   CBrush black_brush(RGB(0, 0, 0));

   CRect rect, rect2;

   // fill background
   pDC->FillRect(&client_rect, &dark_gray_brush);

   int pageview_width = (client_rect.BottomRight().x - PIXELS_FROM_EDGE)
      - (client_rect.TopLeft().x + PIXELS_FROM_EDGE);
   int pageview_height = (client_rect.BottomRight().y - PIXELS_FROM_EDGE)
      - (client_rect.TopLeft().y + PIXELS_FROM_EDGE);

   rect.TopLeft() = client_rect.TopLeft() +
      CPoint(PIXELS_FROM_EDGE, PIXELS_FROM_EDGE);
   rect.BottomRight() = CPoint(client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width), client_rect.TopLeft().y +
      PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height));
   pDC->FillRect(rect, &black_brush);
   rect2 = rect;

   rect.TopLeft() += CPoint(6, 6);
   rect.BottomRight() += CPoint(6, 6);
   pDC->FillRect(rect, &black_brush);

   rect2.TopLeft() += CPoint(1, 1);
   rect2.BottomRight() -= CPoint(1, 1);
   pDC->FillRect(rect2, &bright_white_brush);
}


// draw the dashed virtual page dividers
void CPageLayoutDlg::draw_dividers(CDC *pDC, CRect client_rect)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   UtilDraw util(pDC);
   CRect rect;
   int row, column;
   double x1, y1, x2, y2;

   // get the width and height of the page
   rect.TopLeft() = client_rect.TopLeft() + CPoint(PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE  + 1);
   rect.BottomRight() = client_rect.BottomRight() -
      CPoint(PIXELS_FROM_EDGE + 1, PIXELS_FROM_EDGE + 1);
   int w = rect.Width();
   int h = rect.Height();

   // get the offset to the page
   int offset_x = rect.TopLeft().x;
   int offset_y = rect.TopLeft().y;

   // get the width and height of the virtual page in inches
   double vpage_width = m_vpage_layout.get_virtual_page_width();
   double vpage_height = m_vpage_layout.get_virtual_page_height();

   // set the pen to black dashed line of width 2
   util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_DASH2, 2);

   // loop through each virtual page and draw its dividers
   for (column = 0; column < m_vpage_layout.get_num_columns(); column++)
      for (row = 0; row < m_vpage_layout.get_num_rows(); row++)
      {
         // get the bounds, in inches, of the virtual page at the
         // given row and column
         m_vpage_layout.get_map_bounds(row, column,
            &x1, &y1, &x2, &y2);

         // convert virtual page's bounding rect coordinates to match
         // the given CRect's
         x1 = (x1 * w * m_scale_x) / m_vpage_layout.get_page_width();
         x2 = (x2 * w * m_scale_x) / m_vpage_layout.get_page_width();
         y1 = (y1 * h * m_scale_y) / m_vpage_layout.get_page_height();
         y2 = (y2 * h * m_scale_y) / m_vpage_layout.get_page_height();

         // draw the dashed lined virtual page
         int a1 = futil->round(x1) + offset_x;
         int b1 = futil->round(y1) + offset_y;
         int a2 = futil->round(x2) + offset_x;
         int b2 = futil->round(y2) + offset_y;

         util.draw_line(a1, b1, a2, b1);
         util.draw_line(a2, b1, a2, b2);
         util.draw_line(a2, b2, a1, b2);
         util.draw_line(a1, b2, a1, b1);
      }
}

// draw the text blocks denoting the page width and height
void CPageLayoutDlg::draw_text(CDC *pDC, CRect client_rect)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CRect rect;
   UtilDraw util(pDC);
   const int BUF_LEN = 25;
   char buf[BUF_LEN];
   POINT cpt[4];
   CString h, w;
   float decimal_value;
   int number_of_decimal_places = 2;

   int pageview_width = (client_rect.BottomRight().x - PIXELS_FROM_EDGE)
      - (client_rect.TopLeft().x + PIXELS_FROM_EDGE);
   int pageview_height = (client_rect.BottomRight().y - PIXELS_FROM_EDGE)
      - (client_rect.TopLeft().y + PIXELS_FROM_EDGE);

   util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 2);

   // page width/height text and arrows

   util.draw_line(PIXELS_FROM_EDGE + 1, 5, PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE - 5);

   util.draw_line(client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 1,
      5, client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 1,
      PIXELS_FROM_EDGE - 5);

   util.draw_line(PIXELS_FROM_EDGE + 1, PIXELS_FROM_EDGE / 2,
      client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 1,
      PIXELS_FROM_EDGE / 2);



   util.draw_line(PIXELS_FROM_EDGE + 1, PIXELS_FROM_EDGE / 2,
      PIXELS_FROM_EDGE + 7, PIXELS_FROM_EDGE / 2 - 5);

   util.draw_line(PIXELS_FROM_EDGE + 1, PIXELS_FROM_EDGE / 2,
      PIXELS_FROM_EDGE + 7, PIXELS_FROM_EDGE / 2 + 5);

   util.draw_line(client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 1,
      PIXELS_FROM_EDGE / 2,
      client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 7,
      PIXELS_FROM_EDGE / 2 + 5);

   util.draw_line(client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 1,
      PIXELS_FROM_EDGE / 2,
      client_rect.TopLeft().x + PIXELS_FROM_EDGE +
      futil->round(m_scale_x * pageview_width) - 7,
      PIXELS_FROM_EDGE / 2 - 5);

      CString page_w, page_h;
      double p_height = m_vpage_layout.get_page_height();
      double p_width = m_vpage_layout.get_page_width();

   // convert the width without the fractional part to a CString
   TRACE(_T("This code requires testing due to security changes (Line %d File "
      "%s).  Remove this message after this line has been executed."),
      __LINE__, __FILE__);
   _itoa_s(static_cast<int>(p_width), buf, BUF_LEN, 10);
   w = buf;

   // calculate the decimal value
   decimal_value = static_cast<float>((p_width - static_cast<int>(p_width)) *
      pow(10.0, number_of_decimal_places));

   // concat the decimal value to the string
   w += ".";
   _itoa_s(futil->round(decimal_value), buf, BUF_LEN, 10);
   w += buf;
   w += "\"";

      futil->draw_text(
         pDC,  // pointer to dc
         w,  // text to draw
         (client_rect.TopLeft().x +
            futil->round(m_scale_x * pageview_width) - 2) / 2 +
            PIXELS_FROM_EDGE,  // screen x, y position
         5,
         UTIL_ANCHOR_UPPER_CENTER,  // reference position of text
         "Arial",  // font name
         15,  // font size in points
         0,  // font attributes (bold, italic, etc)
         UTIL_BG_NONE,  // background type
         UTIL_COLOR_BLACK,  // code for text color
         UTIL_COLOR_DARK_GRAY,  // code for background color
         0,  // angle of text
         cpt  // 4 point array defining the text polygon corners
);
      CBrush dark_gray_brush(RGB(128, 128, 128));
      CRect i_rect(cpt[0], cpt[2]);
      pDC->FillRect(&i_rect, &dark_gray_brush);
      futil->draw_text(
         pDC,  // pointer to dc
         w,  // text to draw
         (client_rect.TopLeft().x +
            futil->round(m_scale_x * pageview_width) - 2) / 2 +
            PIXELS_FROM_EDGE,  // screen x, y position
         5,
         UTIL_ANCHOR_UPPER_CENTER,  // reference position of text
         "Arial",  // font name
         15,  // font size in points
         0,  // font attributes (bold, italic, etc)
         UTIL_BG_NONE,  // background type
         UTIL_COLOR_BLACK,  // code for text color
         UTIL_COLOR_DARK_GRAY,  // code for background color
         0,  // angle of text
         cpt  // 4 point array defining the text polygon corners
);


   // page height text and arrows
   util.draw_line(5, PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE-5, PIXELS_FROM_EDGE + 1);
   util.draw_line(5, client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 1,
      PIXELS_FROM_EDGE - 5, client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 1);

   util.draw_line(PIXELS_FROM_EDGE / 2, PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE / 2, client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 1);

   util.draw_line(PIXELS_FROM_EDGE / 2, PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE / 2 - 5, PIXELS_FROM_EDGE + 7);
   util.draw_line(PIXELS_FROM_EDGE / 2, PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE / 2 + 5, PIXELS_FROM_EDGE + 7);

   util.draw_line(PIXELS_FROM_EDGE / 2,
      client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 1,
      PIXELS_FROM_EDGE / 2 - 5,
      client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 7);
   util.draw_line(PIXELS_FROM_EDGE / 2,
      client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 1,
      PIXELS_FROM_EDGE / 2 + 5,
      client_rect.TopLeft().y + PIXELS_FROM_EDGE +
      futil->round(m_scale_y * pageview_height) - 7);

   // convert the height without the fractional part to a CString
   _itoa_s(static_cast<int>(p_height), buf, BUF_LEN, 10);
   h = buf;

   // calculate the decimal value
   decimal_value = static_cast<float>((p_height- static_cast<int>(p_height)) *
      pow(10.0, number_of_decimal_places));

   // concat the decimal value to the string
   h += ".";
   _itoa_s(futil->round(decimal_value), buf, BUF_LEN, 10);
   h += buf;
   h += "\"";

    futil->draw_text(
         pDC,  // pointer to dc
         h,  // text to draw
         5,  // screen x, y position
         (client_rect.TopLeft().y +
         futil->round(m_scale_y * pageview_height) - 2) / 2 + PIXELS_FROM_EDGE,
         UTIL_ANCHOR_UPPER_CENTER,  // reference position of text
         "Arial",  // font name
         14,  // font size in points
         0,  // font attributes (bold, italic, etc)
         UTIL_BG_NONE,  // background type
         UTIL_COLOR_BLACK,  // code for text color
         UTIL_COLOR_DARK_GRAY,  // code for background color
         -90,  // angle of text
         cpt  // 4 point array defining the text polygon corners
);
      CRect i_rect2(cpt[0], cpt[2]);
      pDC->FillRect(&i_rect2, &dark_gray_brush);
      futil->draw_text(
         pDC,  // pointer to dc
         h,  // text to draw
         5,  // screen x, y position
         (client_rect.TopLeft().y +
         futil->round(m_scale_y * pageview_height) - 2) / 2 + PIXELS_FROM_EDGE,
         UTIL_ANCHOR_UPPER_CENTER,  // reference position of text
         "Arial",  // font name
         14,  // font size in points
         0,  // font attributes (bold, italic, etc)
         UTIL_BG_NONE,  // background type
         UTIL_COLOR_BLACK,  // code for text color
         UTIL_COLOR_DARK_GRAY,  // code for background color
         -90,  // angle of text
         cpt  // 4 point array defining the text polygon corners
);

   // text for virtual page width/height
   CString s("Virtual page size: ");
   double height = m_vpage_layout.get_virtual_page_height();
   double width = m_vpage_layout.get_virtual_page_width();

   // convert the width without the fractional part to a CString
   _itoa_s(static_cast<int>(width), buf, BUF_LEN, 10);
   w = buf;

   // calculate the decimal value
   decimal_value = static_cast<float>((width - static_cast<int>(width)) *
      pow(10.0, number_of_decimal_places));

   // concat the decimal value to the string
   w += ".";
   _itoa_s(futil->round(decimal_value), buf, BUF_LEN, 10);
   w += buf;

   // convert the height without the fractional part to a CString
   _itoa_s(static_cast<int>(height), buf, BUF_LEN, 10);
   h = buf;

   // calculate the decimal value
   decimal_value = static_cast<float>((height- static_cast<int>(height)) *
      pow(10.0, number_of_decimal_places));

   // concat the decimal value to the string
   h += ".";
   _itoa_s(futil->round(decimal_value), buf, 10);
   h += buf;

   s += w + "\" x " + h + "\"";

   futil->draw_text(
               pDC,  // pointer to dc
               s,  // text to draw
               client_rect.BottomRight().x / 2,  // screen x
               client_rect.BottomRight().y - 5,  // screen y
               UTIL_ANCHOR_LOWER_CENTER,  // reference position of text
               "Arial",  // font name
               15,  // font size in points
               0,  // font attributes (bold, italic, etc)
               UTIL_BG_NONE,  // background type
               UTIL_COLOR_BLACK,  // code for text color
               UTIL_COLOR_DARK_GRAY,  // code for background color
               0,  // angle of text
               cpt  // 4 point array defining the text polygon corners
);
}

// draw arrows on each virtual page denoting which way is up
void CPageLayoutDlg::draw_arrows(CDC *pDC, CRect client_rect)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   UtilDraw util(pDC);
   int row, column;
   double v_height = m_vpage_layout.get_virtual_page_height();
   double v_width = m_vpage_layout.get_virtual_page_width();
   double x1, y1, x2, y2;
   CPoint arrow[7];
   CRect rect;

   // get the width and height of the page
   rect.TopLeft() = client_rect.TopLeft() + CPoint(PIXELS_FROM_EDGE + 1,
      PIXELS_FROM_EDGE  + 1);
   rect.BottomRight() = client_rect.BottomRight() -
      CPoint(PIXELS_FROM_EDGE + 1, PIXELS_FROM_EDGE + 1);
   int w = rect.Width();
   int h = rect.Height();

   // get the offset to the page
   int offset_x = rect.TopLeft().x;
   int offset_y = rect.TopLeft().y;

   util.set_pen(UTIL_COLOR_DARK_GRAY, UTIL_LINE_SOLID, 2);

    // loop through each virtual page and draw its arrow
   for (column = 0; column < m_vpage_layout.get_num_columns(); column++)
      for (row = 0; row < m_vpage_layout.get_num_rows(); row++)
      {
         // get the bounds, in inches, of the virtual page at the
         // given row and column
         m_vpage_layout.get_map_bounds(row, column,
            &x1, &y1, &x2, &y2);

         // convert virtual page's bounding rect coordinates to match
         // the given CRect's
         x1 = (x1 * w * m_scale_x) / m_vpage_layout.get_page_width();
         x2 = (x2 * w * m_scale_x) / m_vpage_layout.get_page_width();
         y1 = (y1 * h * m_scale_y) / m_vpage_layout.get_page_height();
         y2 = (y2 * h * m_scale_y) / m_vpage_layout.get_page_height();

         int a1 = futil->round(x1) + offset_x;
         int b1 = futil->round(y1) + offset_y;
         int a2 = futil->round(x2) + offset_x;
         int b2 = futil->round(y2) + offset_y;

         // arrow parameters
         int h1, h2, arrow_width, base;

         // always draw the arrow pointing up on the page
         base = static_cast<int>((1/13.0)* static_cast<double>(a2-a1));
         h1 = static_cast<int>((1/2.0) * static_cast<double>(b2-b1));
         h2 = static_cast<int>((1/6.0) * static_cast<double>(b2-b1));
         arrow_width = static_cast<int>((1/6.0)* static_cast<double>(a2-a1));

         arrow[0].x = a1 + ((a2 - a1) - base) / 2;
         arrow[0].y = b1 + ((b2-b1)-(h1+h2))/2 + h2 + h1;
         arrow[1].x = arrow[0].x + base;
         arrow[1].y = arrow[0].y;
         arrow[2].x = arrow[1].x;
         arrow[2].y = b1 + ((b2-b1)-(h1+h2))/2 + h2;
         arrow[3].x = (arrow[0].x + arrow[1].x)/2 + arrow_width;
         arrow[3].y = arrow[2].y;
         arrow[4].x = (arrow[0].x + arrow[1].x) / 2;
         arrow[4].y = b1 + ((b2-b1)-(h1+h2))/2;
         arrow[5].x = (arrow[0].x + arrow[1].x) / 2 - arrow_width;
         arrow[5].y = arrow[2].y;
         arrow[6].x = arrow[0].x;
         arrow[6].y = arrow[2].y;

         util.draw_polygon(TRUE, arrow, 7);
      }
}

void CPageLayoutDlg::OnChangeNumColumns()
{
   // TRUE means that we want to show the error message
   change_num_columns(TRUE);
}

void CPageLayoutDlg::change_num_columns(boolean_t show_error_message,
   boolean_t update_data /*=TRUE*/)
{
   if (update_data && UpdateData())
   {
      m_vpage_layout.set_num_columns(m_value_num_columns);

      // make sure the virtual page width is at a minimum 2" x 4"
      if (m_vpage_layout.get_virtual_page_width() < 2.0 ||
         m_vpage_layout.get_virtual_page_height() < 4.0)
      {
         while (m_value_num_columns > 1 &&
            (m_vpage_layout.get_virtual_page_width() < 2.0 ||
            m_vpage_layout.get_virtual_page_height() < 4.0))
         {
            m_value_num_columns--;
            m_vpage_layout.set_num_columns(m_value_num_columns);
         }

         UpdateData(FALSE);

         if (show_error_message)
            AfxMessageBox("The minimum virtual page size is 2\" x 4\".  Cannot "
            "add another column.");
      }

      HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
      draw_preview(hwnd);
   }
}

void CPageLayoutDlg::OnChangeNumRows()
{
   // TRUE means that we want to show the error message
   change_num_rows(TRUE);
}

void CPageLayoutDlg::change_num_rows(boolean_t show_error_message,
   boolean_t update_data)
{
   if (update_data && UpdateData())
   {
      m_vpage_layout.set_num_rows(m_value_num_rows);

      // make sure the virtual page width is at a minimum 2" x 4"
      if (m_vpage_layout.get_virtual_page_width() < 2.0 ||
         m_vpage_layout.get_virtual_page_height() < 4.0)
      {
         while (m_value_num_rows > 1 &&
            (m_vpage_layout.get_virtual_page_width() < 2.0 ||
            m_vpage_layout.get_virtual_page_height() < 4.0))
         {
            m_value_num_rows--;
            m_vpage_layout.set_num_rows(m_value_num_rows);
         }

         m_vpage_layout.set_num_rows(m_value_num_rows);

         UpdateData(FALSE);

         if (show_error_message)
            AfxMessageBox("The minimum virtual page size is 2\" x 4\".  Cannot "
            "add another row.");
      }

      HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
      draw_preview(hwnd);
   }
}

void CPageLayoutDlg::OnChangeHorzSpacing()
{
   if (UpdateData())
      m_vpage_layout.set_horz_spacing(static_cast<float>(m_value_horz_spacing));

   // check to be sure the number of columns is still valid, don't
   // show an error message just adjust the number of columns if
   // necessary.  Also, there is no need to call UpdateData again (second param)
   change_num_columns(FALSE, FALSE);

   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
   draw_preview(hwnd);
}

void CPageLayoutDlg::OnChangeVertSpacing()
{
   if (UpdateData())
      m_vpage_layout.set_vert_spacing(static_cast<float>(m_value_vert_spacing));

   // check to be sure the number of rows is still valid, don't
   // show an error message just adjust the number of rows if
   // necessary.  Also, there is no need to call UpdateData again (second param)
   change_num_rows(FALSE, FALSE);

   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
   draw_preview(hwnd);
}

void CPageLayoutDlg::OnDeltaposSpinHorzSpacing(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);

   if (pNMUpDown->iDelta < 0 && m_value_horz_spacing <= 0.99)
      m_value_horz_spacing += 0.01;
   else if (pNMUpDown->iDelta > 0 && m_value_horz_spacing >= 0.01)
      m_value_horz_spacing -= 0.01;

   m_vpage_layout.set_horz_spacing(static_cast<float>(m_value_horz_spacing));
   UpdateData(FALSE);

   // check to be sure the number of columns is still valid, don't
   // show an error message just adjust the number of columns if
   // necessary
   change_num_columns(FALSE);


   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
   draw_preview(hwnd);

   *pResult = 0;
}

void CPageLayoutDlg::OnDeltaposSpinVertSpacing(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);

   if (pNMUpDown->iDelta < 0 && m_value_vert_spacing <= 0.99)
      m_value_vert_spacing += 0.01;
   else if (pNMUpDown->iDelta > 0 && m_value_vert_spacing >= 0.01)
      m_value_vert_spacing -= 0.01;

   m_vpage_layout.set_vert_spacing(static_cast<float>(m_value_vert_spacing));
   UpdateData(FALSE);

   // check to be sure the number of rows is still valid, don't
   // show an error message just adjust the number of rows if
   // necessary
   change_num_rows(FALSE);

   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
   draw_preview(hwnd);

   *pResult = 0;
}

void CPageLayoutDlg::OnDeltaposSpinRows(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);

   if (pNMUpDown->iDelta > 0)
      m_value_num_rows++;
   else if (pNMUpDown->iDelta < 0 && m_value_num_rows > 1)
      m_value_num_rows--;

   UpdateData(FALSE);

   change_num_rows(FALSE);
}

void CPageLayoutDlg::OnDeltaposSpinColumns(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);

   int num_columns = m_value_num_columns;

   if (pNMUpDown->iDelta > 0)
      num_columns++;
   else if (pNMUpDown->iDelta < 0 && m_value_num_columns > 1)
      num_columns--;
   else
      return;

   m_vpage_layout.set_num_columns(num_columns);

   // make sure the virtual page width is at a minimum 2" x 4"
   if (m_vpage_layout.get_virtual_page_width() < 2.0 ||
      m_vpage_layout.get_virtual_page_height() < 4.0)
   {
      m_vpage_layout.set_num_columns(--num_columns);
   }
   else
   {
      m_value_num_columns = num_columns;
      UpdateData(FALSE);

      HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
      draw_preview(hwnd);
   }
}

// launch the print setup dialog
void CPageLayoutDlg::OnPrintSetup()
{
   ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND,
      ID_FILE_PRINT_SETUP, 0);

   // set the appropriate bitmap for the strip chart page orientation controls
   {
      // get the current orientation from the page layout object
      page_orientation_t orientation = m_vpage_layout.get_orientation();

      // portrait / landscape bitmap
      if (m_vpage_layout.get_page_width() < m_vpage_layout.get_page_height())
      {
         m_scale_x = SQUASH;
         m_scale_y = 1.0;
         m_strip_page_bitmap.SetBitmap(m_bitmap[0]);
      }
      else
      {
         m_scale_x = 1.0;
         m_scale_y = SQUASH;
         m_strip_page_bitmap.SetBitmap(m_bitmap[1]);
      }

      // strip chart page orientation
      (reinterpret_cast<CButton *>(GetDlgItem(IDC_PORTRAIT))->SetCheck(
         orientation == PAGE_ORIENTATION_PORTRAIT));
      (reinterpret_cast<CButton *>(GetDlgItem(IDC_LANDSCAPE))->SetCheck(
         orientation == PAGE_ORIENTATION_LANDSCAPE));
      (reinterpret_cast<CButton *>(GetDlgItem(IDC_DEFAULT))->SetCheck(
         orientation == PAGE_ORIENTATION_DEFAULT));
   }

   // check to be sure the number of rows is still valid, don't
   // show an error message just adjust the number of rows if
   // necessary
   change_num_rows(FALSE);

   // check to be sure the number of columns is still valid, don't
   // show an error message just adjust the number of columns if
   // necessary
   change_num_columns(FALSE);

   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
   draw_preview(hwnd);
}

void CPageLayoutDlg::OnChangeOrientation(void)
{
   if ((reinterpret_cast<CButton *>(GetDlgItem(IDC_PORTRAIT))->GetCheck()))
      m_vpage_layout.set_orientation(PAGE_ORIENTATION_PORTRAIT);
   else if ((reinterpret_cast<CButton *>(
      GetDlgItem(IDC_LANDSCAPE))->GetCheck()))
      m_vpage_layout.set_orientation(PAGE_ORIENTATION_LANDSCAPE);
   else
      m_vpage_layout.set_orientation(PAGE_ORIENTATION_DEFAULT);

   if (m_vpage_layout.get_page_width() < m_vpage_layout.get_page_height())
   {
      m_scale_x = SQUASH;
      m_scale_y = 1.0;
      m_strip_page_bitmap.SetBitmap(m_bitmap[0]);
   }
   else
   {
      m_scale_x = 1.0;
      m_scale_y = SQUASH;
      m_strip_page_bitmap.SetBitmap(m_bitmap[1]);
   }

   // check to be sure the number of rows is still valid, don't
   // show an error message just adjust the number of rows if
   // necessary
   change_num_rows(FALSE);

   // check to be sure the number of columns is still valid, don't
   // show an error message just adjust the number of columns if
   // necessary
   change_num_columns(FALSE);

   HWND hwnd = GetDlgItem(IDC_PREVIEW)->m_hWnd;
   draw_preview(hwnd);
}

void CPageLayoutDlg::OnOK()
{
   CWnd *current_window = GetFocus();

   if (current_window->IsKindOf(RUNTIME_CLASS(CEdit)))
   {
      // if we are on an edit control, then fake a tab.  This
      // is so we can eat the enter key in the single edit controls
      // If we at the last edit control then jump to the OK button
      CWnd *next_window;
      if (current_window == GetDlgItem(IDC_VERT_SPACING))
         next_window = GetDlgItem(IDOK);
      else
         next_window = GetNextDlgTabItem(current_window);

      // make sure that the focus rectangle surrounds the next control.
      GotoDlgCtrl(GetDlgItem(next_window->GetDlgCtrlID()));

      return;
   }

   CDialog::OnOK();
}

CVirtualPageLayout::CVirtualPageLayout()
{
   m_num_rows = 1;
   m_num_columns = 1;
   m_horz_spacing = 0.0;
   m_vert_spacing = 0.0;
   m_orientation = PAGE_ORIENTATION_DEFAULT;
}

CVirtualPageLayout::CVirtualPageLayout(int rows, int columns,
                   float horz_spacing, float vert_spacing) :
m_num_rows(rows), m_num_columns(columns),
m_horz_spacing(horz_spacing), m_vert_spacing(vert_spacing)
{
}

// initialize number of rows, number of columns, horizontal spacing and
// vertical spacing from the registry located at the given registry key
// into this object
void CVirtualPageLayout::initialize_from_registry(CString key)
{
   float value;
   DWORD storage_size;
   BYTE *storage_loc = reinterpret_cast<BYTE *>(&value);

   m_num_rows = PRM_get_registry_int(key, "NumRows", 1);
   m_num_columns = PRM_get_registry_int(key, "NumColumns", 1);

   storage_size = sizeof(value);
   if (PRM_get_registry_binary(key, "VertSpacing", storage_loc, &storage_size)
      == SUCCESS)
      m_vert_spacing = value;
   else
      m_vert_spacing = 0.25;

   storage_size = sizeof(value);
   if (PRM_get_registry_binary(key, "HorzSpacing", storage_loc, &storage_size)
      == SUCCESS)
      m_horz_spacing = value;
   else
      m_horz_spacing = 0.25;

   m_orientation = (page_orientation_t)PRM_get_registry_int(key, "Orientation",
      PAGE_ORIENTATION_DEFAULT);
}

// Store number of rows, number of columns, horizontal spacing and
// vertical spacing into the registry located at the given registry key
void CVirtualPageLayout::save_to_registry(CString key)
{
   float value;
   DWORD storage_size = sizeof(value);
   BYTE *storage_loc = reinterpret_cast<BYTE *>(&value);

   PRM_set_registry_int(key, "NumRows", m_num_rows);
   PRM_set_registry_int(key, "NumColumns", m_num_columns);

   value = m_vert_spacing;
   PRM_set_registry_binary(key, "VertSpacing", storage_loc, storage_size);

   value = m_horz_spacing;
   PRM_set_registry_binary(key, "HorzSpacing", storage_loc, storage_size);

   PRM_set_registry_int(key, "Orientation", m_orientation);
}

// returns the upper left and lower right values in inches where
// the map at the given row and column will be printed
void CVirtualPageLayout::get_map_bounds(int row, int column,
   double *upper_left_x, double *upper_left_y,
   double *lower_right_x, double *lower_right_y) const
{
   // calculate the virtual page width and height
   double virtual_page_width = get_virtual_page_width();
   double virtual_page_height = get_virtual_page_height();

   *upper_left_x = column * (virtual_page_width + m_horz_spacing);
   *upper_left_y = row * (virtual_page_height + m_vert_spacing);

   *lower_right_x = *upper_left_x + virtual_page_width;
   *lower_right_y = *upper_left_y + virtual_page_height;
}

double CVirtualPageLayout::get_page_width(void) const
{
   // make sure that the printer paper size is valid
   double page_width = CPrinterPage::GetPageWidthInInches();
   double page_height = CPrinterPage::GetPageHeightInInches();
   if (page_width <= 0.0 || page_height <= 0.0)
   {
      ERR_report("Invalid page dimensions.");
      return -1.0;
   }

   if (m_orientation == PAGE_ORIENTATION_PORTRAIT)
      page_width = __min(page_width, page_height);
   else if (m_orientation == PAGE_ORIENTATION_LANDSCAPE)
      page_width = __max(page_width, page_height);

   return page_width;
}

// Return the virtual page width.  Returns -1.0 for error.  Returns 0.0,
// if m_num_columns is too large for the current printer paper size.
double CVirtualPageLayout::get_virtual_page_width(void) const
{
   // make sure that number of columns is set to a valid value
   // this will also avoid a possible divide by zero below
   if (m_num_columns < 1)
   {
      ERR_report("m_num_columns < 1.");
      return -1.0;
   }

   double page_width = get_page_width();

   // in the event that the printer paper size gets too small
   // return a virtual paper size of zero
   double virtual_page_width = (page_width -
      (m_num_columns - 1)*m_horz_spacing) / m_num_columns;
   if (virtual_page_width < 0.0)
      return 0.0;

   return virtual_page_width;
}

double CVirtualPageLayout::get_page_height(void) const
{
   // make sure that the printer paper size is valid
   double page_width = CPrinterPage::GetPageWidthInInches();
   double page_height = CPrinterPage::GetPageHeightInInches();
   if (page_height <= 0.0 || page_width <= 0.0)
   {
      ERR_report("Invalid page height.");
      return -1.0;
   }

   // adjust the page height depending on the orientation of the
   // strip chart page
   if (m_orientation == PAGE_ORIENTATION_PORTRAIT)
      page_height = __max(page_width, page_height);
   else if (m_orientation == PAGE_ORIENTATION_LANDSCAPE)
      page_height= __min(page_width, page_height);

   return page_height;
}

// Return the virtual page height.  Returns -1.0 for error.  Returns 0.0,
// if m_num_rows is too large for the current printer paper size.
double CVirtualPageLayout::get_virtual_page_height(void) const
{
   // make sure that number of rows is set to a valid value
   // this will also avoid a possible divide by zero below
   if (m_num_rows < 1)
   {
      ERR_report("m_num_rows < 1.");
      return -1.0;
   }

   double page_height = get_page_height();

   double virtual_page_height = (page_height -
      (m_num_rows - 1)*m_vert_spacing) / m_num_rows;
   if (virtual_page_height < 0.0)
      return 0.0;

   return virtual_page_height;
}

// makes sure the virtual page width and height do not go below
// the set minimum.  If they do then adjust the number of rows
// and columns until every thing is a-OK
void CVirtualPageLayout::validate_num_rows_and_columns(void)
{
   while (m_num_rows > 1 && (get_virtual_page_width() < 2.0 ||
      get_virtual_page_height() < 4.0))

      m_num_rows--;


   while (m_num_columns > 1 && (get_virtual_page_width() < 2.0 ||
      get_virtual_page_height() < 4.0))

      m_num_columns--;
}
LRESULT CPageLayoutDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
