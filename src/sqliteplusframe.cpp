/*---------------------------------------------------------------------------*/
/* Logiciel de gestion de fichier de bases de données SQLite                 */
/*---------------------------------------------------------------------------*/
/* Projet  : wxSQLitePlus                              Version  : 0.3.0.0    */
/* Fichier : sqliteplusframe.cpp                                             */
/* Auteur  : Fred Cailleau-Lepetit                     Date     : 04/07/2007 */
/* email   : softinthebox@free.fr                      Révision : 16/05/2008 */
/*---------------------------------------------------------------------------*/
/* Copyright (C) Fred Cailleau-Lepetit 2007                                  */
/* Licence GNU General Public License  http://www.fsf.org/copyleft/gpl.html  */
/*---------------------------------------------------------------------------*/
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation (version 3);

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Ce programme est libre, vous pouvez le redistribuer et/ou le modifier
selon les termes de la Licence Publique Générale GNU publiée par la
Free Software Foundation (version 3).

Ce programme est distribué car potentiellement utile, mais
SANS AUCUNE GARANTIE, ni explicite ni implicite, y compris
les garanties de commercialisation ou d'adaptation dans un but
spécifique. Reportez-vous à la Licence Publique Générale GNU
pour plus de détails.
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "sqliteplusframe.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/aui/framemanager.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
/*---------------------------------------------------------------------------*/
#include "sqliteplusframe.h"
#include "wxsqliteplusapp.h"
#include "sqlbook.h"
#include "tablebook.h"
#include "viewbook.h"
#include "indexbook.h"
#include "triggerbook.h"
#include "dropobject.h"
#include "createindex.h"
#include "createtable.h"
#include "createview.h"
#include "createtrigger.h"
#include "dbbook.h"
#include "preferences.h"
#include "aboutdlg.h"
#include "addcolumn.h"
#include "attachdbdlg.h"
#include "detachdbdlg.h"
#include "encrypkeydlg.h"
#include "backrestdlg.h"
/*---------------------------------------------------------------------------*/
#define ID_TREE_ROOT          -1
#define ID_TREE_DATABASE       0
#define ID_TREE_TABLES         1
#define ID_TREE_VIEWS          2
#define ID_TREE_INDEXES        3
#define ID_TREE_TRIGGERS       4
#define ID_TREE_TABLE          5
#define ID_TREE_TABLE_TMP      6
#define ID_TREE_VIEW           7
#define ID_TREE_VIEW_TMP       8
#define ID_TREE_INDEX          9
#define ID_TREE_INDEX_TMP     10
#define ID_TREE_TRIGGER       11
#define ID_TREE_TRIGGER_TMP   12
#define ID_TREE_FIELD         13

#define ID_DBTREE       15001
#define ID_CENTERBOOK   15002
#define ID_STATUSBAR    15003
#define ID_TB_FILE      15004
#define ID_TB_EDIT      15005
#define ID_TBQUERY      15006
#define ID_TB_TRANSACT  15007
#define ID_SQL_PANEL    15008
/*---------------------------------------------------------------------------*/
wxString g_SQLiteObjTypeName[5] =
{
    _("table"),
    _("view"),
    _("index"),
    _("trigger"),
    ""
};
/*---------------------------------------------------------------------------*/
IMPLEMENT_DYNAMIC_CLASS(wxSQLitePlusFrame, wxFrame)

BEGIN_EVENT_TABLE(wxSQLitePlusFrame, wxFrame)
    EVT_CLOSE(wxSQLitePlusFrame::OnCloseWindow)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_DBTREE, wxSQLitePlusFrame::OnDbtreeItemRightClick)
    EVT_TREE_SEL_CHANGED(ID_DBTREE, wxSQLitePlusFrame::OnDbtreeItemSelChanged)
    EVT_TREE_ITEM_ACTIVATED(ID_DBTREE, wxSQLitePlusFrame::OnDbtreeItemActivated)

    EVT_MENU(wxID_OPEN, wxSQLitePlusFrame::OnOpenCreateClick)
    EVT_MENU(ID_MNU_ATTACH_DB, wxSQLitePlusFrame::OnAttachDbClick)
    EVT_UPDATE_UI(ID_MNU_ATTACH_DB, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_CLOSE_DB, wxSQLitePlusFrame::OnMnuCloseDbClick)
    EVT_UPDATE_UI(ID_MNU_CLOSE_DB, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_BACKUP, wxSQLitePlusFrame::OnMnuBackupDbClick)
    EVT_UPDATE_UI(ID_MNU_BACKUP, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_RESTORE, wxSQLitePlusFrame::OnMnuRestoreDbClick)
    EVT_UPDATE_UI(ID_MNU_RESTORE, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_DETACH_DB, wxSQLitePlusFrame::OnDetachDbClick)
    EVT_UPDATE_UI(ID_MNU_DETACH_DB, wxSQLitePlusFrame::OnDetachDbUpdate)
    EVT_MENU(ID_MNU_COMPACT, wxSQLitePlusFrame::OnEventClick)
    EVT_MENU(ID_MNU_REKEY, wxSQLitePlusFrame::OnReKeyDbClick)
    EVT_UPDATE_UI(ID_MNU_REKEY, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_UPDATE_UI(ID_MNU_COMPACT, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(wxID_REFRESH, wxSQLitePlusFrame::OnRefreshClick)
    EVT_MENU(ID_MNU_REFRESHALL, wxSQLitePlusFrame::OnRefreshAllClick)
    EVT_MENU(wxID_PREFERENCES, wxSQLitePlusFrame::OnPreferencesClick)
    EVT_MENU(wxID_EXIT, wxSQLitePlusFrame::OnExitClick)
    EVT_MENU(wxID_UNDO, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(wxID_UNDO, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(wxID_REDO, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(wxID_REDO, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(wxID_CUT, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(wxID_CUT, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(wxID_COPY, wxSQLitePlusFrame::OnCopyClick)
    EVT_UPDATE_UI(wxID_COPY, wxSQLitePlusFrame::OnCopyUpdate)
    EVT_MENU(wxID_PASTE, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(wxID_PASTE, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(wxID_CLEAR, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(wxID_CLEAR, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_CLEAR_ALL, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_CLEAR_ALL, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(wxID_SELECTALL, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(wxID_SELECTALL, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_ADD_TABLE, wxSQLitePlusFrame::OnMnuAddTableClick)
    EVT_UPDATE_UI(ID_MNU_ADD_TABLE, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_DEL_TABLE, wxSQLitePlusFrame::OnMnuDelTableClick)
    EVT_UPDATE_UI(ID_MNU_DEL_TABLE, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_ADD_VIEW, wxSQLitePlusFrame::OnMnuAddViewClick)
    EVT_UPDATE_UI(ID_MNU_ADD_VIEW, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_DEL_VIEW, wxSQLitePlusFrame::OnMnuDelViewClick)
    EVT_UPDATE_UI(ID_MNU_DEL_VIEW, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_ADD_INDEX, wxSQLitePlusFrame::OnMnuAddIndexClick)
    EVT_UPDATE_UI(ID_MNU_ADD_INDEX, wxSQLitePlusFrame::OnMnuAddIndexUpdate)
    EVT_MENU(ID_MNU_DEL_INDEX, wxSQLitePlusFrame::OnMnuDelIndexClick)
    EVT_UPDATE_UI(ID_MNU_DEL_INDEX, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_ADD_TRIGGER, wxSQLitePlusFrame::OnMnuAddTriggerClick)
    EVT_UPDATE_UI(ID_MNU_ADD_TRIGGER, wxSQLitePlusFrame::OnMnuAddTriggerUpdate)
    EVT_MENU(ID_MNU_DEL_TRIGGER, wxSQLitePlusFrame::OnMnuDelTriggerClick)
    EVT_UPDATE_UI(ID_MNU_DEL_TRIGGER, wxSQLitePlusFrame::OnMnuCloseDbUpdate)
    EVT_MENU(ID_MNU_ADD_COLUMNS, wxSQLitePlusFrame::OnMnuAddColumnsClick)
    EVT_UPDATE_UI(ID_MNU_ADD_COLUMNS, wxSQLitePlusFrame::OnMnuAddIndexUpdate)
    EVT_MENU(ID_MNU_EXECUTESQL, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_EXECUTESQL, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_EXECUTESCRIPT, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_EXECUTESCRIPT, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_EXPLAIN, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_EXPLAIN, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_DESCRIBE, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_DESCRIBE, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_BEGINTRANSACT, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_BEGINTRANSACT, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_ENDTRANSACT, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_ENDTRANSACT, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_COMMIT, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_COMMIT, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_ROLLBACK, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_ROLLBACK, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_AUTOTRANSACT, wxSQLitePlusFrame::OnMnuAutotransactClick)
    EVT_MENU(ID_MNU_SHOWTREE, wxSQLitePlusFrame::OnMnuShowTreeClick)
    EVT_UPDATE_UI(ID_MNU_SHOWTREE, wxSQLitePlusFrame::OnMnuShowTreeUpdate)
    EVT_MENU(ID_MNU_SHOWHISTORY, wxSQLitePlusFrame::OnEventClick)
    EVT_UPDATE_UI(ID_MNU_SHOWHISTORY, wxSQLitePlusFrame::OnEventUpdate)
    EVT_MENU(ID_MNU_TB_FILE, wxSQLitePlusFrame::OnMnuTbFileClick)
    EVT_UPDATE_UI(ID_MNU_TB_FILE, wxSQLitePlusFrame::OnMnuTbFileUpdate)
    EVT_MENU(ID_MNU_TB_EDIT, wxSQLitePlusFrame::OnMnuTbEditClick)
    EVT_UPDATE_UI(ID_MNU_TB_EDIT, wxSQLitePlusFrame::OnMnuTbEditUpdate)
    EVT_MENU(ID_MNU_TB_DATABASE, wxSQLitePlusFrame::OnMnuTbDatabaseClick)
    EVT_UPDATE_UI(ID_MNU_TB_DATABASE, wxSQLitePlusFrame::OnMnuTbDatabaseUpdate)
    EVT_MENU(ID_MNU_TB_QUERY, wxSQLitePlusFrame::OnMnuTbQueryClick)
    EVT_UPDATE_UI(ID_MNU_TB_QUERY, wxSQLitePlusFrame::OnMnuTbQueryUpdate)
    EVT_MENU(ID_MNU_TB_TRANSACT, wxSQLitePlusFrame::OnMnuTbTransactClick)
    EVT_UPDATE_UI(ID_MNU_TB_TRANSACT, wxSQLitePlusFrame::OnMnuTbTransactUpdate)
    EVT_MENU(wxID_HELP, wxSQLitePlusFrame::OnHelpClick)
    EVT_MENU(wxID_ABOUT, wxSQLitePlusFrame::OnAboutClick)
    EVT_MENU(ID_MNU_EXPLORE, wxSQLitePlusFrame::OnMnuExploreClick)
    EVT_UPDATE_UI(ID_MNU_EXPLORE, wxSQLitePlusFrame::OnMnuExploreUpdate)
    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_CENTERBOOK, wxSQLitePlusFrame::OnNotebookPageClose)
END_EVENT_TABLE()
/*---------------------------------------------------------------------------*/
wxSQLitePlusFrame::wxSQLitePlusFrame()
{
    Init();
}
/*---------------------------------------------------------------------------*/
wxSQLitePlusFrame::wxSQLitePlusFrame(wxWindow* parent, wxWindowID id,
                                     const wxString& caption, const wxPoint& pos,
                                     const wxSize& size, long style)
{
    Init();
    Create(parent, id, caption, pos, size, style);

    // open a in-memory database when start
    // http://www.sqlite.org/inmemorydb.html
    OpenDatabase(":memory:", "memory");
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::Create(wxWindow* parent, wxWindowID id,
                               const wxString& caption, const wxPoint& pos,
                               const wxSize& size, long style)
{
    wxFrame::Create(parent, id, caption, pos, size, style);

    CreateControls();
    SetIcon(wxGetApp().GetIcon(ID_ICO_LOGO32));
    InitTree();

    if (wxGetApp().GetSavePerspective())
    {
        wxString perspective;
        perspective = wxGetApp().GetParamStr(PARAM_FRAME_PERSPECTIVE);
        if (!perspective.IsEmpty())
            GetAuiManager().LoadPerspective(perspective);
    }

    int autotransact = wxGetApp().GetParamInt(PARAM_AUTOTRANSACT);
    m_MnuTransact->Check(ID_MNU_AUTOTRANSACT, (autotransact == 1));
    m_TbTransact->ToggleTool(ID_MNU_AUTOTRANSACT, (autotransact == 1));

    return true;
}
/*---------------------------------------------------------------------------*/
wxSQLitePlusFrame::~wxSQLitePlusFrame()
{
    GetAuiManager().UnInit();
    for(auto cipher: this->m_Cipher)
    {
        delete cipher;
    }
    this->m_Cipher.clear();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::Init()
{
    m_TreeCtrl = NULL;
    m_CenterNotebook = NULL;
    m_MnuFile = NULL;
    m_MnuEdit = NULL;
    m_MnuDatabase = NULL;
    m_MnuTransact = NULL;
    m_MnuWindow = NULL;
    m_MnuHelp = NULL;
    m_TbFile = NULL;
    m_TbEdit = NULL;
    m_TbTransact = NULL;
    m_TbDatabase = NULL;

    InitNodeParams();

    // Accelerators
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_DATABASE));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_TABLES));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_VIEWS));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_INDEXES));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_TRIGGERS2));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_TABLE));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_TABLE_TMP));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_VIEW));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_VIEW_TMP));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_INDEXE));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_INDEXE_TMP));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_TRIGGER2));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_TRIGGER2_TMP));
    m_images.push_back(wxGetApp().GetBmp(ID_BMP_FIELD));

    wxAcceleratorEntry entries[5];

    entries[0].Set(wxACCEL_CTRL, 'E', ID_MNU_EXECUTESQL);
    entries[1].Set(wxACCEL_CTRL, 'S', ID_MNU_EXECUTESCRIPT);
    entries[2].Set(wxACCEL_CTRL, 'C', wxID_COPY);
    entries[3].Set(wxACCEL_CTRL, WXK_INSERT, wxID_COPY);
    entries[4].Set(wxACCEL_CTRL, 'A', wxID_SELECTALL);

    wxAcceleratorTable accel(5, entries);
    SetAcceleratorTable(accel);

    wxSQLite3CipherAes128* aes128 = new wxSQLite3CipherAes128;
    this->m_Cipher.push_back(aes128);

    wxSQLite3CipherSQLCipher* sqlCipher1 = new wxSQLite3CipherSQLCipher();
    sqlCipher1->InitializeVersionDefault(1);
    this->m_Cipher.push_back(sqlCipher1);

    wxSQLite3CipherSQLCipher* sqlCipher2 = new wxSQLite3CipherSQLCipher();
    sqlCipher2->InitializeVersionDefault(2);
    sqlCipher2->SetFastKdfIter(4000);
    sqlCipher2->SetHmacSaltMask(0);
    this->m_Cipher.push_back(sqlCipher2);

    wxSQLite3CipherSQLCipher* sqlCipher2be = new wxSQLite3CipherSQLCipher();
    sqlCipher2be->InitializeVersionDefault(2);
    sqlCipher2be->SetHmacPgNo(2);
    this->m_Cipher.push_back(sqlCipher2be);

    wxSQLite3CipherSQLCipher* sqlCipher2le = new wxSQLite3CipherSQLCipher();
    sqlCipher2le->InitializeVersionDefault(2);
    this->m_Cipher.push_back(sqlCipher2le);

    wxSQLite3CipherSQLCipher* sqlCipher3 = new wxSQLite3CipherSQLCipher();
    sqlCipher3->InitializeVersionDefault(3);
    sqlCipher3->SetLegacy(true);
    this->m_Cipher.push_back(sqlCipher3);

    wxSQLite3CipherSQLCipher* sqlCipher4 = new wxSQLite3CipherSQLCipher();
    sqlCipher4->InitializeVersionDefault(4);
    sqlCipher4->SetLegacy(true);
    this->m_Cipher.push_back(sqlCipher4);

}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateControls()
{
    GetAuiManager().SetManagedWindow(this);

    CreateMenus();

    m_TreeCtrl = new wxTreeCtrl(this, ID_DBTREE, wxDefaultPosition,
                                wxDefaultSize,
                                wxTR_HAS_BUTTONS |wxTR_LINES_AT_ROOT|
                                wxTR_HIDE_ROOT|wxTR_SINGLE|wxSUNKEN_BORDER);

    GetAuiManager().AddPane(m_TreeCtrl, wxAuiPaneInfo().Name(("TreePane"))
                            .Caption(("Database Structure"))
                            .BestSize(wxSize(250, -1))
                            .TopDockable(false)
                            .BottomDockable(false)
                            .DestroyOnClose(false)
                            .Resizable(true)
                            .FloatingSize(wxSize(250, 300)));

    m_CenterNotebook = new wxAuiNotebook(this, ID_CENTERBOOK, wxDefaultPosition,
                                         wxDefaultSize,
                                         wxAUI_NB_TAB_SPLIT|wxAUI_NB_TAB_MOVE|
                                         wxAUI_NB_TAB_EXTERNAL_MOVE|
                                         wxAUI_NB_CLOSE_ON_ACTIVE_TAB);

    GetAuiManager().AddPane(m_CenterNotebook, wxAuiPaneInfo().Name(("CenterPane"))
                            .Centre()
                            .CaptionVisible(false)
                            .CloseButton(false)
                            .DestroyOnClose(false)
                            .Resizable(true)
                            .Floatable(false));

    wxStatusBar* statusBar = new wxStatusBar(this, ID_STATUSBAR,
            wxST_SIZEGRIP|wxSUNKEN_BORDER);
    statusBar->SetFieldsCount(3);
    SetStatusBar(statusBar);

    CreateToolbars();

    GetAuiManager().Update();

    m_TreeCtrl->SetImages(m_images);
    int widths[3] = {-1, 70, 170};
    SetStatusWidths(3, widths);
    SetStatusText("v" + wxGetApp().GetFileVersion() + "/" + m_db.GetVersion() + "/" + wxVERSION_STRING, 2);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateMenus()
{
    wxMenuBar* menuBar = new wxMenuBar;
    m_MnuFile = new wxMenu;

    m_MnuFile->Append(wxID_OPEN, _("&Open/Create"),
                      _("Open an existing database or create a new database"),
                      wxITEM_NORMAL);
    m_MnuFile->Append(ID_MNU_ATTACH_DB, _("&Attach"),
                      _("Attach an existing or new database"), wxITEM_NORMAL);

    m_MnuFile->Append(ID_MNU_CLOSE_DB, _("C&lose database"),
                      _("Close the current database"), wxITEM_NORMAL);
    m_MnuFile->Append(ID_MNU_DETACH_DB, _("&Detach database"),
                      _("Detach an attached database"), wxITEM_NORMAL);

    m_MnuFile->Append(ID_MNU_COMPACT, _("&Compact"),
                      _("Delete empty space in database"), wxITEM_NORMAL);
    m_MnuFile->Enable(ID_MNU_COMPACT, false);

    m_MnuFile->Append(ID_MNU_REKEY, _("Encryption key"),
                      _("Add/Change the encryption key of the main database"), wxITEM_NORMAL);
    m_MnuFile->AppendSeparator();
    m_MnuFile->Append(ID_MNU_BACKUP, _("Backup"),
                      _("Backup a database"), wxITEM_NORMAL);
    m_MnuFile->Append(ID_MNU_RESTORE, _("Restore"),
                      _("Restore a database"), wxITEM_NORMAL);

    m_MnuFile->AppendSeparator();
    m_MnuFile->Append(ID_MNU_REFRESHALL, _("Refresh"),
                      _("Refresh all database structure"), wxITEM_NORMAL);
    m_MnuFile->AppendSeparator();
    m_MnuFile->Append(wxID_PREFERENCES, _("&Preferences"),
                      _("Show the preferences dialog"), wxITEM_NORMAL);
    m_MnuFile->AppendSeparator();
    m_MnuFile->Append(wxID_EXIT, _("E&xit\tAlt+X"), wxEmptyString, wxITEM_NORMAL);
    menuBar->Append(m_MnuFile, _("&File"));

    m_MnuEdit = new wxMenu;
    m_MnuEdit->Append(wxID_UNDO, _("&undo\tCtrl+Z"),
                      _("Undo the last editor action"), wxITEM_NORMAL);
    m_MnuEdit->Append(wxID_REDO, _("&Redo\tShift+Ctrl+Z"),
                      _("Redo the last editor action"), wxITEM_NORMAL);
    m_MnuEdit->AppendSeparator();
    m_MnuEdit->Append(wxID_CUT, _("&Cut\tCtrl+X"),
                      _("Cut the selected text"), wxITEM_NORMAL);
    m_MnuEdit->Append(wxID_COPY, _("C&opy\tCtrl+C"),
                      _("Copy the selected text"), wxITEM_NORMAL);
    m_MnuEdit->Append(wxID_PASTE, _("&Paste\tCtrl+V"),
                      _("Paste in the editor the text in the clipboard"),
                      wxITEM_NORMAL);
    m_MnuEdit->AppendSeparator();
    m_MnuEdit->Append(wxID_CLEAR, _("C&lear"), _("Delete the selected text"),
                      wxITEM_NORMAL);
    m_MnuEdit->Append(ID_MNU_CLEAR_ALL, _("Clear A&ll"), _("Clear editor"),
                      wxITEM_NORMAL);
    m_MnuEdit->Append(wxID_SELECTALL, _("Select &All\tCtrl+A"),
                      _("Select all the content of the editor"), wxITEM_NORMAL);
    menuBar->Append(m_MnuEdit, _("&Edit"));

    m_MnuDatabase = new wxMenu;
    m_MnuDatabase->Append(ID_MNU_ADD_TABLE, _("Create &table"),
                          _("Show dialog to create new table"), wxITEM_NORMAL);
    m_MnuDatabase->Append(ID_MNU_DEL_TABLE, _("Delete t&able"),
                          _("Show dialog to delete table"), wxITEM_NORMAL);
    m_MnuDatabase->AppendSeparator();
    m_MnuDatabase->Append(ID_MNU_ADD_VIEW, _("Create &view"),
                          _("Show dialog to create new view"), wxITEM_NORMAL);
    m_MnuDatabase->Append(ID_MNU_DEL_VIEW, _("Delete v&iew"),
                          _("Show dialog to delete view"), wxITEM_NORMAL);
    m_MnuDatabase->AppendSeparator();
    m_MnuDatabase->Append(ID_MNU_ADD_INDEX, _("Create i&ndex"),
                          _("Show dialog to create new index"), wxITEM_NORMAL);
    m_MnuDatabase->Append(ID_MNU_DEL_INDEX, _("Delete inde&x"),
                          _("Show dialog to delete index"), wxITEM_NORMAL);
    m_MnuDatabase->AppendSeparator();
    m_MnuDatabase->Append(ID_MNU_ADD_TRIGGER, _("Create tr&igger"),
                          _("Show dialog to create new trigger"), wxITEM_NORMAL);
    m_MnuDatabase->Append(ID_MNU_DEL_TRIGGER, _("Delete tri&gger"),
                          _("Show dialog to delete trigger"), wxITEM_NORMAL);
    m_MnuDatabase->AppendSeparator();
    m_MnuDatabase->Append(ID_MNU_ADD_COLUMNS, _("Add &columns"),
                          _("Show dialog to add columns to table"), wxITEM_NORMAL);
    m_MnuDatabase->AppendSeparator();
    m_MnuDatabase->Append(ID_MNU_EXPLORE, _("Explore"),
                          _("Explore the selected object"), wxITEM_NORMAL);

    menuBar->Append(m_MnuDatabase, _("&Database"));

    wxMenu* mnuQuery = new wxMenu;
    mnuQuery->Append(ID_MNU_EXECUTESQL, _("E&xecute Statement\tF9"),
                     _("Execute the current SQL statement"), wxITEM_NORMAL);
    mnuQuery->Append(ID_MNU_EXECUTESCRIPT, _("Run &Script\tF5"),
                     _("Execute the SQL script"), wxITEM_NORMAL);
    mnuQuery->AppendSeparator();
    mnuQuery->Append(ID_MNU_EXPLAIN, _("Execute &Explain Plan\tF6"),
                     _("Explain the current SQL statement"), wxITEM_NORMAL);
    mnuQuery->Append(ID_MNU_DESCRIBE, _("&Describe\tF4"),
                     wxEmptyString, wxITEM_NORMAL);
    menuBar->Append(mnuQuery, _("Query"));

    m_MnuTransact = new wxMenu;
    m_MnuTransact->Append(ID_MNU_BEGINTRANSACT, _("&Begin transaction"),
                          _("Begin a new transaction"), wxITEM_NORMAL);
    m_MnuTransact->Append(ID_MNU_ENDTRANSACT, _("&End transaction"),
                          _("Stop the current transaction"), wxITEM_NORMAL);
    m_MnuTransact->AppendSeparator();
    m_MnuTransact->Append(ID_MNU_COMMIT, _("&Commit"),
                          _("Commit the current transaction"), wxITEM_NORMAL);
    m_MnuTransact->Append(ID_MNU_ROLLBACK, _("&Rollback"),
                          _("Rollback the current transaction"), wxITEM_NORMAL);
    m_MnuTransact->AppendSeparator();
    m_MnuTransact->Append(ID_MNU_AUTOTRANSACT, _("Auto start transaction"),
                          _("Set/unset the auto start transaction flag"),
                          wxITEM_CHECK);
    menuBar->Append(m_MnuTransact, _("&Transaction"));

    m_MnuWindow = new wxMenu;
    m_MnuWindow->Append(ID_MNU_SHOWTREE, _("Show Database &structure"),
                        _("Show/hide the database structure window"),
                        wxITEM_CHECK);
    m_MnuWindow->Check(ID_MNU_SHOWTREE, true);
    m_MnuWindow->Append(ID_MNU_SHOWHISTORY, _("Show SQL &History\tF8"),
                        _("Show the SQL history window"), wxITEM_NORMAL);
    m_MnuWindow->AppendSeparator();
    wxMenu* mnuToolbars = new wxMenu;
    mnuToolbars->Append(ID_MNU_TB_FILE, _("Show toolbar &File"),
                        _("Show/hide the file toolbar"), wxITEM_CHECK);
    mnuToolbars->Check(ID_MNU_TB_FILE, true);
    mnuToolbars->Append(ID_MNU_TB_EDIT, _("Show toolbar &Edit"),
                        _("Show/hide the edit toolbar"), wxITEM_CHECK);
    mnuToolbars->Check(ID_MNU_TB_EDIT, true);
    mnuToolbars->Append(ID_MNU_TB_DATABASE, _("Show toolbar &Database"),
                        _("Show/hide the database toolbar"), wxITEM_CHECK);
    mnuToolbars->Append(ID_MNU_TB_QUERY, _("Show toolbar &Query"),
                        _("Show/hide the query toolbar"), wxITEM_CHECK);
    mnuToolbars->Check(ID_MNU_TB_QUERY, true);
    mnuToolbars->Append(ID_MNU_TB_TRANSACT, _("Show toolbar &Transaction"),
                        _("Show/hide the transaction toolbar"), wxITEM_CHECK);
    mnuToolbars->Check(ID_MNU_TB_TRANSACT, true);
    m_MnuWindow->Append(ID_MNU_TOOLBAR, _("Toolbars"), mnuToolbars);
    menuBar->Append(m_MnuWindow, _("&Windows"));

    m_MnuHelp = new wxMenu;
    m_MnuHelp->Append(wxID_HELP, _("&Help"), _("Show the help window"),
                      wxITEM_NORMAL);
    m_MnuHelp->Append(wxID_ABOUT, _("&About"),
                      _("Show the information dialog"), wxITEM_NORMAL);
    menuBar->Append(m_MnuHelp, _("&Help"));
    SetMenuBar(menuBar);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateToolbars()
{
    wxBitmap BitmapDisabled = wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_TOOLBAR);;

    m_TbFile = new wxToolBar(this, ID_TB_FILE, wxDefaultPosition, wxDefaultSize,
                             wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
    m_TbFile->SetToolBitmapSize(wxSize(20, 20));
    wxBitmapBundle bmp1(wxGetApp().GetBmp(ID_BMP_OPENDB));
    m_TbFile->AddTool(wxID_OPEN, wxEmptyString, bmp1, BitmapDisabled,
                      wxITEM_NORMAL, _("Open/Create Database"),
                      _("Open an existing database or create a new database"));
    wxBitmapBundle bmp2(wxGetApp().GetBmp(ID_BMP_ATTACH));
    m_TbFile->AddTool(ID_MNU_ATTACH_DB, wxEmptyString, bmp2, BitmapDisabled,
                      wxITEM_NORMAL, _("Attach Database"),
                      _("Attach an existing or new database"));
    wxBitmapBundle bmp3(wxGetApp().GetBmp(ID_BMP_COMPACTDB));
    m_TbFile->AddTool(ID_MNU_COMPACT, wxEmptyString, bmp3, BitmapDisabled,
                      wxITEM_NORMAL, _("Compact Database"),
                      _("Delete empty space in database"));
    m_TbFile->EnableTool(ID_MNU_COMPACT, false);
    wxBitmapBundle bmp4(wxGetApp().GetBmp(ID_BMP_REFRESH));
    m_TbFile->AddTool(ID_MNU_REFRESHALL, wxEmptyString, bmp4, BitmapDisabled,
                      wxITEM_NORMAL, _("Refresh Structure"),
                      _("Refresh all database structure"));
    m_TbFile->Realize();
    GetAuiManager().AddPane(m_TbFile, wxAuiPaneInfo().ToolbarPane()
                            .Name(("ToolbarFile"))
                            .Caption(("File"))
                            .Top()
                            .LeftDockable(false)
                            .RightDockable(false)
                            .CaptionVisible(false)
                            .DestroyOnClose(false)
                            .Resizable(false)
                            .Gripper(true)
                            .PaneBorder(false));

    m_TbEdit = new wxToolBar(this, ID_TB_EDIT, wxDefaultPosition, wxDefaultSize,
                             wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
    m_TbEdit->SetToolBitmapSize(wxSize(20, 20));
    wxBitmapBundle bmp5(wxGetApp().GetBmp(ID_BMP_UNDO));
    m_TbEdit->AddTool(wxID_UNDO, wxEmptyString, bmp5, BitmapDisabled,
                      wxITEM_NORMAL, _("Undo"), _("Undo the last editor action"));
    wxBitmapBundle bmp6(wxGetApp().GetBmp(ID_BMP_REDO));
    m_TbEdit->AddTool(wxID_REDO, wxEmptyString, bmp6, BitmapDisabled,
                      wxITEM_NORMAL, _("Redo"), _("Redo the last editor action"));
    m_TbEdit->AddSeparator();
    wxBitmapBundle bmp7(wxGetApp().GetBmp(ID_BMP_CUT));
    m_TbEdit->AddTool(wxID_CUT, wxEmptyString, bmp7, BitmapDisabled,
                      wxITEM_NORMAL, _("Cut"), _("Cut the selected text"));
    wxBitmapBundle bmp8(wxGetApp().GetBmp(ID_BMP_COPY));
    m_TbEdit->AddTool(wxID_COPY, wxEmptyString, bmp8, BitmapDisabled,
                      wxITEM_NORMAL, _("Copy"), _("Copy the selected text"));
    wxBitmapBundle bmp9(wxGetApp().GetBmp(ID_BMP_PASTE));
    m_TbEdit->AddTool(wxID_PASTE, wxEmptyString, bmp9, BitmapDisabled,
                      wxITEM_NORMAL, _("Paste"),
                      _("Paste in the editor the text in the clipboard"));
    m_TbEdit->AddSeparator();
    wxBitmapBundle bmp10(wxGetApp().GetBmp(ID_BMP_CLEAR));
    m_TbEdit->AddTool(wxID_CLEAR, wxEmptyString, bmp10, BitmapDisabled,
                      wxITEM_NORMAL, _("Clear"),
                      _("Delete the selected text"));
    m_TbEdit->Realize();
    GetAuiManager().AddPane(m_TbEdit, wxAuiPaneInfo().ToolbarPane()
                            .Name(("ToolbarEdit"))
                            .Caption(("Edit"))
                            .Top()
                            .LeftDockable(false)
                            .RightDockable(false)
                            .CaptionVisible(false)
                            .DestroyOnClose(false)
                            .Resizable(false)
                            .Gripper(true)
                            .PaneBorder(false));

    m_TbQuery = new wxToolBar(this, ID_TBQUERY, wxDefaultPosition, wxDefaultSize,
                              wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
    m_TbQuery->SetToolBitmapSize(wxSize(20, 20));
    wxBitmapBundle bmp11(wxGetApp().GetBmp(ID_BMP_RUNSQL));
    m_TbQuery->AddTool(ID_MNU_EXECUTESQL, wxEmptyString, bmp11, BitmapDisabled,
                       wxITEM_NORMAL, _("Execute SQL"),
                       _("Execute the current SQL statement"));
    wxBitmapBundle bmp12(wxGetApp().GetBmp(ID_BMP_RUNSCRIPT));
    m_TbQuery->AddTool(ID_MNU_EXECUTESCRIPT, wxEmptyString, bmp12, BitmapDisabled,
                       wxITEM_NORMAL, _("Execute script"),
                       _("Execute the SQL script"));
    m_TbQuery->AddSeparator();
    wxBitmapBundle bmp13(wxGetApp().GetBmp(ID_BMP_EXPLAIN));
    m_TbQuery->AddTool(ID_MNU_EXPLAIN, wxEmptyString, bmp13, BitmapDisabled,
                       wxITEM_NORMAL, _("Explain"),
                       _("Explain the current SQL statement"));
    m_TbQuery->Realize();
    GetAuiManager().AddPane(m_TbQuery, wxAuiPaneInfo().ToolbarPane()
                            .Name(("Query"))
                            .Top()
                            .LeftDockable(false)
                            .RightDockable(false)
                            .CaptionVisible(false)
                            .DestroyOnClose(false)
                            .Resizable(false)
                            .Gripper(true)
                            .PaneBorder(false));

    m_TbTransact = new wxToolBar(this, ID_TB_TRANSACT, wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
    m_TbTransact->SetToolBitmapSize(wxSize(20, 20));
    wxBitmapBundle bmp14(wxGetApp().GetBmp(ID_BMP_STARTTRANSACT));
    m_TbTransact->AddTool(ID_MNU_BEGINTRANSACT, wxEmptyString, bmp14,
                          BitmapDisabled, wxITEM_NORMAL, _("Begin Transaction"),
                          _("Begin a new transaction"));
    wxBitmapBundle bmp15(wxGetApp().GetBmp(ID_BMP_STOPTRANSACT));
    m_TbTransact->AddTool(ID_MNU_ENDTRANSACT, wxEmptyString, bmp15,
                          BitmapDisabled, wxITEM_NORMAL, _("End Transaction"),
                          _("Stop the current transaction"));
    m_TbTransact->AddSeparator();
    wxBitmapBundle bmp16(wxGetApp().GetBmp(ID_BMP_COMMIT));
    m_TbTransact->AddTool(ID_MNU_COMMIT, wxEmptyString, bmp16, BitmapDisabled,
                          wxITEM_NORMAL, _("Commit"),
                          _("Commit the current transaction"));
    wxBitmapBundle bmp17(wxGetApp().GetBmp(ID_BMP_ROLLBACK));
    m_TbTransact->AddTool(ID_MNU_ROLLBACK, wxEmptyString, bmp17, BitmapDisabled,
                          wxITEM_NORMAL, _("Rollback"),
                          _("Rollback the current transaction"));
    m_TbTransact->AddSeparator();
    wxBitmapBundle bmp18(wxGetApp().GetBmp(ID_BMP_AUTOTRANSACT));
    m_TbTransact->AddTool(ID_MNU_AUTOTRANSACT, wxEmptyString, bmp18,
                          BitmapDisabled, wxITEM_CHECK,
                          _("Auto Start Transaction"),
                          _("Set/unset the auto start transaction flag"));
    m_TbTransact->Realize();
    GetAuiManager().AddPane(m_TbTransact, wxAuiPaneInfo().ToolbarPane()
                            .Name(("Pane1"))
                            .Caption(("Transaction"))
                            .Top()
                            .LeftDockable(false)
                            .RightDockable(false)
                            .CaptionVisible(false)
                            .DestroyOnClose(false)
                            .Resizable(false)
                            .Gripper(true)
                            .PaneBorder(false));

    m_TbDatabase = new wxToolBar(this, ID_MNU_TB_DATABASE, wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
    m_TbDatabase->SetToolBitmapSize(wxSize(20, 20));
    wxBitmapBundle bmp19(wxGetApp().GetBmp(ID_BMP_TABLECREATE));
    m_TbDatabase->AddTool(ID_MNU_ADD_TABLE, wxEmptyString, bmp19, BitmapDisabled,
                          wxITEM_NORMAL, _("Create Table"),
                          _("Show dialog to create new table"));
    wxBitmapBundle bmp20(wxGetApp().GetBmp(ID_BMP_TABLEDROP));
    m_TbDatabase->AddTool(ID_MNU_DEL_TABLE, wxEmptyString, bmp20, BitmapDisabled,
                          wxITEM_NORMAL, _("Drop Table"),
                          _("Show dialog to delete table"));
    m_TbDatabase->AddSeparator();
    wxBitmapBundle bmp21(wxGetApp().GetBmp(ID_BMP_VIEWCREATE));
    m_TbDatabase->AddTool(ID_MNU_ADD_VIEW, wxEmptyString, bmp21, BitmapDisabled,
                          wxITEM_NORMAL, _("Create View"),
                          _("Show dialog to create new view"));
    wxBitmapBundle bmp22(wxGetApp().GetBmp(ID_BMP_VIEWDROP));
    m_TbDatabase->AddTool(ID_MNU_DEL_VIEW, wxEmptyString, bmp22, BitmapDisabled,
                          wxITEM_NORMAL, _("Drop View"),
                          _("Show dialog to delete view"));
    m_TbDatabase->AddSeparator();
    wxBitmapBundle bmp23(wxGetApp().GetBmp(ID_BMP_INDEXECREATE));
    m_TbDatabase->AddTool(ID_MNU_ADD_INDEX, wxEmptyString, bmp23, BitmapDisabled,
                          wxITEM_NORMAL, _("Create Index"),
                          _("Show dialog to create new index"));
    wxBitmapBundle bmp24(wxGetApp().GetBmp(ID_BMP_INDEXEDROP));
    m_TbDatabase->AddTool(ID_MNU_DEL_INDEX, wxEmptyString, bmp24, BitmapDisabled,
                          wxITEM_NORMAL, _("Drop Index"),
                          _("Show dialog to delete index"));
    m_TbDatabase->AddSeparator();
    wxBitmapBundle bmp25(wxGetApp().GetBmp(ID_BMP_TRIGGERCREATE));
    m_TbDatabase->AddTool(ID_MNU_ADD_TRIGGER, wxEmptyString, bmp25,
                          BitmapDisabled, wxITEM_NORMAL, _("Create Trigger"),
                          _("Show dialog to create new trigger"));
    wxBitmapBundle bmp26(wxGetApp().GetBmp(ID_BMP_TRIGGERDROP));
    m_TbDatabase->AddTool(ID_MNU_DEL_TRIGGER, wxEmptyString, bmp26,
                          BitmapDisabled, wxITEM_NORMAL, _("Drop Trigger"),
                          _("Show dialog to delete trigger"));
    m_TbDatabase->Realize();
    GetAuiManager().AddPane(m_TbDatabase, wxAuiPaneInfo().ToolbarPane()
                            .Name(("Pane2"))
                            .Caption(("Database"))
                            .Top()
                            .LeftDockable(false)
                            .RightDockable(false)
                            .CaptionVisible(false)
                            .DestroyOnClose(false)
                            .Resizable(false)
                            .Gripper(true)
                            .Hide()
                            .PaneBorder(false));
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::ShowToolTips()
{
    return true;
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CmdOpen(const wxString& file)
{
    if (m_db.IsOpen())
    {   // Attach
        wxString alias;

        if (DatabaseExist(file, false, &alias))
        {
            wxSQLBook* book = (wxSQLBook*)GetPage("SQL");
            if (book)
            {
                wxString msg = wxString::Format(_("Error : Database \"%s\" is already opened as %s\n"),
                                                file.c_str(), alias.c_str());

                book->GetLogResult()->AppendText(msg);
                book->ShowLog();
            }
            return;
        }
        alias = wxFileName(file).GetName().Lower();
        alias.Replace((" "), ("_"));
        alias.Replace(("\""), (""));
        alias.Replace(("'"), (""));

        if (DatabaseExist(alias))
        {
            int i = 1;
            while (DatabaseExist(wxString::Format(("%s-%i"), alias.c_str(), i)))
                i++;
            alias = wxString::Format(("%s-%i"), alias.c_str(), i);
        }
        AttachDatabase(file, alias);
    }
    else  // Open
        OpenDatabase(file, wxFileName(file).GetFullName());
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::InitNodeParams()
{
    m_NodeType = -1;
    m_NodeText = wxEmptyString;
    m_NodeDbName = ("main");

    // Voir si un item sélectionné, si oui prendre les bons
    if (m_TreeCtrl != NULL)
    {
        wxTreeItemId item = m_TreeCtrl->GetSelection();
        if (item.IsOk()&& (item != m_TreeCtrl->GetRootItem()))
        {
            m_NodeType = m_TreeCtrl->GetItemImage(item);
            m_NodeText = m_TreeCtrl->GetItemText(item);
            m_NodeDbName = GetDbNameFromItem(item);
        }
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnDbtreeItemRightClick(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    m_NodeType = m_TreeCtrl->GetItemImage(item);
    m_NodeText = m_TreeCtrl->GetItemText(item);
    m_NodeDbName = GetDbNameFromItem(item);

    wxMenu* popMenu = GetPopupMenu(m_NodeType);
    if (popMenu)
    {
        PopupMenu(popMenu);
        delete popMenu;
    }
    event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnDbtreeItemSelChanged(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    m_NodeType = m_TreeCtrl->GetItemImage(item);
    m_NodeText = m_TreeCtrl->GetItemText(item);
    m_NodeDbName = GetDbNameFromItem(item);

    event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnDbtreeItemActivated(wxTreeEvent& event)
{
    int flags;
    wxTreeItemId item = m_TreeCtrl->HitTest(event.GetPoint(), flags);
    if (!item.IsOk())
        return;

    if ((flags & wxTREE_HITTEST_ONITEMLABEL) == wxTREE_HITTEST_ONITEMLABEL)
    {
        int image = m_TreeCtrl->GetItemImage(item);
        if (image == ID_TREE_TABLE || image == ID_TREE_TABLE_TMP ||
                image == ID_TREE_VIEW || image == ID_TREE_VIEW_TMP ||
                image == ID_TREE_FIELD)
        {
            wxString Str = ("\"") + m_TreeCtrl->GetItemText(item) + ("\"");
            int index = m_CenterNotebook->GetSelection();
            if (m_CenterNotebook->GetPageText(index) == "SQL")
            {
                wxSQLBook* window = (wxSQLBook*)m_CenterNotebook->GetPage(index);
                if (window)
                    window->WriteText(Str);
            }
        }
    }
    else if ((flags & wxTREE_HITTEST_ONITEMICON) == wxTREE_HITTEST_ONITEMICON)
        event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (!CloseDatabase())
    {
        event.Veto();
        return;
    }
    if (wxGetApp().GetSavePosition())
    {
        if (IsMaximized())
            wxGetApp().SetParamInt(PARAM_FRAME_MAXIMIZED, 1);
        else
        {
            wxPoint pos = GetPosition();
            wxSize size = GetSize();
            wxGetApp().SetParamInt(PARAM_FRAME_MAXIMIZED, 0);
            wxGetApp().SetParamInt(PARAM_FRAME_XPOS, pos.x);
            wxGetApp().SetParamInt(PARAM_FRAME_YPOS, pos.y);
            wxGetApp().SetParamInt(PARAM_FRAME_WIDTH, size.GetWidth());
            wxGetApp().SetParamInt(PARAM_FRAME_HEIGHT, size.GetHeight());
        }
    }

    if (wxGetApp().GetSavePerspective())
    {
        wxString perspective = GetAuiManager().SavePerspective();
        wxGetApp().SetParamStr(PARAM_FRAME_PERSPECTIVE, perspective);
    }

    int autotransact = (m_MnuTransact->IsChecked(ID_MNU_AUTOTRANSACT) ? 1 : 0);
    wxGetApp().SetParamInt(PARAM_AUTOTRANSACT, autotransact);

    event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnOpenCreateClick(wxCommandEvent& event)
{
    wxFileDialog fileDialog(this, _("Open/Create database"), wxEmptyString,
                            wxEmptyString,
                            _("SQLite files (*.db)|*.db|All files (*.*)|*.*"),
                            wxFD_OPEN/*|wxFILE_MUST_EXIST*/);
    if (fileDialog.ShowModal() == wxID_OK)
    {
        if (!wxFileExists(fileDialog.GetPath()))
        {
            wxString msg = wxString::Format(_("%s\nFile does not exist.\nDo you want to create it?"),
                                            fileDialog.GetPath().c_str());
            int answer = wxMessageBox(msg, _("Confirm"), wxYES_NO, this);
            if (answer == wxNO)
                return;
        }

        if (OpenDatabase(fileDialog.GetPath(),
                         wxFileName(fileDialog.GetPath()).GetFullName()))
            InitNodeParams();
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnAttachDbClick(wxCommandEvent& event)
{
    wxAttachDbDlg attachDbDlg(this);

    attachDbDlg.SetDatabase(&m_db);
    if (attachDbDlg.ShowModal() == wxID_OK)
    {
        wxString dbfile = attachDbDlg.GetFileName();
        wxString dbalias = attachDbDlg.GetAlias();
        wxString dbkey = attachDbDlg.GetKey();

        AttachDatabase(dbfile, dbalias, dbkey);
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnReKeyDbClick(wxCommandEvent& event)
{
    wxDbEncryptionKeyDlg encryptionKeyDlg(this);

    if (encryptionKeyDlg.ShowModal() == wxID_OK)
    {
        wxString dbkey = encryptionKeyDlg.GetKey();

        EncryptDatabase(dbkey);
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnRefreshClick(wxCommandEvent& event)
{
    DoRefresh();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnRefreshAllClick(wxCommandEvent& event)
{
    DoRefresh(true);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnPreferencesClick(wxCommandEvent& event)
{
    bool ValBold[STYLE_COUNT];
    bool ValItalic[STYLE_COUNT];
    bool ValUnderline[STYLE_COUNT];
    wxString ValItemFG[STYLE_COUNT];
    wxString ValItemBG[STYLE_COUNT];
    int ValCase[STYLE_COUNT];

    wxString ValOddColour = wxGetApp().GetParamStr(PARAM_ODDROWCOLOR);
    int ValMaxHistory = wxGetApp().GetParamInt(PARAM_SQLHISTOCOUNT);
    bool ValSavePos = wxGetApp().GetSavePosition() ? true : false;
    bool ValSavePers = wxGetApp().GetSavePerspective() ? true : false;
    bool ValShowEdge = wxGetApp().GetParamInt(PARAM_STYLE_EDGELINE) ? true : false;
    bool ValShowLineNumber = wxGetApp().GetParamInt(PARAM_ST_SHOW_LINENUM) ? true : false;
    wxString ValLineFG = wxGetApp().GetParamStr(PARAM_ST_FORE_LINENUM);
    wxString ValLineBG = wxGetApp().GetParamStr(PARAM_ST_BACK_LINENUM);
    bool ValUseTab = wxGetApp().GetParamInt(PARAM_ST_USE_TAB) ? true : false;
    bool ValTabIndent = wxGetApp().GetParamInt(PARAM_ST_TAB_INDENT) ? true : false;
    bool ValBackUnindent = wxGetApp().GetParamInt(PARAM_ST_BACKSP_UNINDT) ? true : false;
    int ValTabWidth = wxGetApp().GetParamInt(PARAM_ST_TAB_WIDTH);
    int ValIndentWidth = wxGetApp().GetParamInt(PARAM_ST_INDENT_WIDTH);
    wxString ValFontName = wxGetApp().GetParamStr(PARAM_STYLE_FONTNAME);
    int ValFontSize = wxGetApp().GetParamInt(PARAM_STYLE_FONTSIZE);

    for (int i = STYLE_DEFAULT; i < STYLE_COUNT; i++)
    {
        ValBold[i] = wxGetApp().GetParamInt(PARAM_ST_BOLD_DEF + (i * 6)) ? true : false;
        ValItalic[i] = wxGetApp().GetParamInt(PARAM_ST_ITALIC_DEF + (i * 6)) ? true : false;
        ValUnderline[i] = wxGetApp().GetParamInt(PARAM_ST_UNDER_DEF + (i * 6)) ? true : false;
        ValItemFG[i] = wxGetApp().GetParamStr(PARAM_ST_FORE_DEF + (i * 6));
        ValItemBG[i] = wxGetApp().GetParamStr(PARAM_ST_BACK_DEF + (i * 6));
        ValCase[i] = wxGetApp().GetParamInt(PARAM_ST_CASE_DEF + (i * 6));
    }

    wxPrefDlg prefDlg(this);

    prefDlg.SetValOddColour(ValOddColour);
    prefDlg.SetValMaxHistory(ValMaxHistory);
    prefDlg.SetValSavePos(ValSavePos);
    prefDlg.SetValSavePers(ValSavePers);
    prefDlg.SetValShowEdge(ValShowEdge);
    prefDlg.SetValShowLineNumber(ValShowLineNumber);
    prefDlg.SetValLineFG(ValLineFG);
    prefDlg.SetValLineBG(ValLineBG);
    prefDlg.SetValUseTab(ValUseTab);
    prefDlg.SetValTabIndent(ValTabIndent);
    prefDlg.SetValBackUnindent(ValBackUnindent);
    prefDlg.SetValTabWidth(ValTabWidth);
    prefDlg.SetValIndentWidth(ValIndentWidth);
    prefDlg.SetValFontName(ValFontName);
    prefDlg.SetValFontSize(ValFontSize);

    for (int i = STYLE_DEFAULT; i < STYLE_COUNT; i++)
    {
        prefDlg.SetValBold(i, ValBold[i]);
        prefDlg.SetValItalic(i, ValItalic[i]);
        prefDlg.SetValUnderline(i, ValUnderline[i]);
        prefDlg.SetValItemFG(i, ValItemFG[i]);
        prefDlg.SetValItemBG(i, ValItemBG[i]);
        prefDlg.SetValCase(i, ValCase[i]);
    }

    if (prefDlg.ShowModal() == wxID_OK)
    {
        if (prefDlg.GetValOddColour() != ValOddColour)
            wxGetApp().SetParamStr(PARAM_ODDROWCOLOR, prefDlg.GetValOddColour());
        if (prefDlg.GetValMaxHistory() != ValMaxHistory)
            wxGetApp().SetParamInt(PARAM_SQLHISTOCOUNT, prefDlg.GetValMaxHistory());
        if (prefDlg.GetValSavePos() != ValSavePos)
            wxGetApp().SetParamInt(PARAM_SAVEWINPOSITION, prefDlg.GetValSavePos());
        if (prefDlg.GetValSavePers() != ValSavePers)
            wxGetApp().SetParamInt(PARAM_SAVEWINPERSPECTIVE, prefDlg.GetValSavePers());

        if (prefDlg.GetValShowEdge() != ValShowEdge)
            wxGetApp().SetParamInt(PARAM_STYLE_EDGELINE, prefDlg.GetValShowEdge());
        if (prefDlg.GetValShowLineNumber() != ValShowLineNumber)
            wxGetApp().SetParamInt(PARAM_ST_SHOW_LINENUM, prefDlg.GetValShowLineNumber());
        if (prefDlg.GetValLineFG() != ValLineFG)
            wxGetApp().SetParamStr(PARAM_ST_FORE_LINENUM, prefDlg.GetValLineFG());
        if (prefDlg.GetValLineBG() != ValLineBG)
            wxGetApp().SetParamStr(PARAM_ST_BACK_LINENUM, prefDlg.GetValLineBG());
        if (prefDlg.GetValUseTab() != ValUseTab)
            wxGetApp().SetParamInt(PARAM_ST_USE_TAB, prefDlg.GetValUseTab());
        if (prefDlg.GetValTabIndent() != ValTabIndent)
            wxGetApp().SetParamInt(PARAM_ST_TAB_INDENT, prefDlg.GetValTabIndent());
        if (prefDlg.GetValBackUnindent() != ValBackUnindent)
            wxGetApp().SetParamInt(PARAM_ST_BACKSP_UNINDT, prefDlg.GetValBackUnindent());
        if (prefDlg.GetValTabWidth() != ValTabWidth)
            wxGetApp().SetParamInt(PARAM_ST_TAB_WIDTH, prefDlg.GetValTabWidth());
        if (prefDlg.GetValIndentWidth() != ValIndentWidth)
            wxGetApp().SetParamInt(PARAM_ST_INDENT_WIDTH, prefDlg.GetValIndentWidth());
        if (prefDlg.GetValFontName() != ValFontName)
            wxGetApp().SetParamStr(PARAM_STYLE_FONTNAME, prefDlg.GetValFontName());
        if (prefDlg.GetValFontSize() != ValFontSize)
            wxGetApp().SetParamInt(PARAM_STYLE_FONTSIZE, prefDlg.GetValFontSize());

        for (int i = STYLE_DEFAULT; i < STYLE_COUNT; i++)
        {
            if (prefDlg.GetValBold(i) != ValBold[i])
                wxGetApp().SetParamInt(PARAM_ST_BOLD_DEF + (i * 6), prefDlg.GetValBold(i));
            if (prefDlg.GetValItalic(i) != ValItalic[i])
                wxGetApp().SetParamInt(PARAM_ST_ITALIC_DEF + (i * 6), prefDlg.GetValItalic(i));
            if (prefDlg.GetValUnderline(i) != ValUnderline[i])
                wxGetApp().SetParamInt(PARAM_ST_UNDER_DEF + (i * 6), prefDlg.GetValUnderline(i));
            if (prefDlg.GetValItemFG(i) != ValItemFG[i])
                wxGetApp().SetParamStr(PARAM_ST_FORE_DEF + (i * 6), prefDlg.GetValItemFG(i));
            if (prefDlg.GetValItemBG(i)!= ValItemBG[i])
                wxGetApp().SetParamStr(PARAM_ST_BACK_DEF + (i * 6), prefDlg.GetValItemBG(i));
            if (prefDlg.GetValCase(i) != ValCase[i])
                wxGetApp().SetParamInt(PARAM_ST_CASE_DEF + (i * 6), prefDlg.GetValCase(i));
        }

        size_t count = m_CenterNotebook->GetPageCount();
        for (size_t i = 0; i < count; i++)
        {
            wxWindow* window = m_CenterNotebook->GetPage(i);
            if (window)
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_CMD_ED_REFRESH);

                window->GetEventHandler()->ProcessEvent(evt);
            }
        }

    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuDropObjectClick(wxCommandEvent& event)
{
    DropDbObject();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddTableClick(wxCommandEvent& event)
{
    CreateTable(m_NodeDbName);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuDelTableClick(wxCommandEvent& event)
{
    DropDbObject(otTable);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddViewClick(wxCommandEvent& event)
{
    CreateView(m_NodeDbName);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuDelViewClick(wxCommandEvent& event)
{
    DropDbObject(otView);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddIndexClick(wxCommandEvent& event)
{
    CreateIndex(m_NodeDbName, m_NodeText);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuDelIndexClick(wxCommandEvent& event)
{
    DropDbObject(otIndex);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddTriggerClick(wxCommandEvent& event)
{
    if (m_NodeType == ID_TREE_TABLE || m_NodeType == ID_TREE_VIEW ||
            m_NodeType == ID_TREE_TABLE_TMP || m_NodeType == ID_TREE_VIEW_TMP)
        CreateTrigger(m_NodeDbName, true,
                      m_NodeType == ID_TREE_TABLE||m_NodeType == ID_TREE_TABLE_TMP,
                      m_NodeText);
    else if ((m_NodeType == ID_TREE_TABLES)||(m_NodeType == ID_TREE_VIEWS))
        CreateTrigger(m_NodeDbName, true, (m_NodeType == ID_TREE_TABLES));
    else
        CreateTrigger(m_NodeDbName);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuDelTriggerClick(wxCommandEvent& event)
{
    DropDbObject(otTrigger);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddColumnsClick(wxCommandEvent& event)
{
    AddColumns(m_NodeDbName, m_NodeText);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuShowTreeClick(wxCommandEvent& event)
{
    HideShowPane(m_TreeCtrl);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbFileClick(wxCommandEvent& event)
{
    HideShowPane(m_TbFile);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbEditClick(wxCommandEvent& event)
{
    HideShowPane(m_TbEdit);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbDatabaseClick(wxCommandEvent& event)
{
    HideShowPane(m_TbDatabase);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbQueryClick(wxCommandEvent& event)
{
    HideShowPane(m_TbQuery);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbTransactClick(wxCommandEvent& event)
{
    HideShowPane(m_TbTransact);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuShowTreeUpdate(wxUpdateUIEvent& event)
{
    event.Check(IsShownPane(m_TreeCtrl));
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbFileUpdate(wxUpdateUIEvent& event)
{
    event.Check(IsShownPane(m_TbFile));
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbEditUpdate(wxUpdateUIEvent& event)
{
    event.Check(IsShownPane(m_TbEdit));
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbDatabaseUpdate(wxUpdateUIEvent& event)
{
    event.Check(IsShownPane(m_TbDatabase));
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbQueryUpdate(wxUpdateUIEvent& event)
{
    event.Check(IsShownPane(m_TbQuery));
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuTbTransactUpdate(wxUpdateUIEvent& event)
{
    event.Check(IsShownPane(m_TbTransact));
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnHelpClick(wxCommandEvent& event)
{
    event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnAboutClick(wxCommandEvent& event)
{
    wxAboutDlg aboutDlg(this);

    aboutDlg.ShowModal();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::InitTree()
{
    wxSQLite3ResultSet dbQRY;

    m_TreeCtrl->DeleteAllItems();

    wxTreeItemId RootItem = m_TreeCtrl->AddRoot(_("Databases"));
    if (m_db.IsOpen())
    {
        try
        {
            dbQRY = m_db.ExecuteQuery(ToUTF8(("PRAGMA database_list;")));
            while (dbQRY.NextRow())
            {
                wxString dbname = dbQRY.GetString(1);
                // La base temporaire est prise en compte avec main
                if (dbname != ("temp"))
                {
                    wxTreeItemId DatabaseItem = m_TreeCtrl->AppendItem(RootItem, dbname,
                                                ID_TREE_DATABASE);
                    RefreshDatabase(dbname);
                }
            }
        }
        catch(wxSQLite3Exception& ex)
        {
            wxGetApp().ShowError("InitTree", ex);
        }
        dbQRY.Finalize();
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::DoRefresh(bool refreshall)
{
    if ((m_db.IsOpen()&&!m_NodeDbName.IsEmpty())||refreshall)
    {
        if (m_NodeType == ID_TREE_TABLES || m_NodeType == ID_TREE_TABLE ||
                m_NodeType == ID_TREE_TABLE_TMP)
            RefreshTables(m_NodeDbName);
        else if (m_NodeType == ID_TREE_VIEWS || m_NodeType == ID_TREE_VIEW ||
                 m_NodeType == ID_TREE_VIEW_TMP)
            RefreshViews(m_NodeDbName);
        else if (m_NodeType == ID_TREE_INDEXES || m_NodeType == ID_TREE_INDEX ||
                 m_NodeType == ID_TREE_INDEX_TMP)
            RefreshIndexes(m_NodeDbName);
        else if (m_NodeType == ID_TREE_TRIGGERS || m_NodeType == ID_TREE_TRIGGER ||
                 m_NodeType == ID_TREE_TRIGGER_TMP)
            RefreshTriggers(m_NodeDbName);
        else if (m_NodeType == ID_TREE_DATABASE)
            RefreshDatabase(m_NodeDbName, false);
        else
            InitTree();

        m_TreeCtrl->Refresh();
        m_TreeCtrl->Update();
    }
    else
        InitTree();
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::RefreshDatabase(const wxString& dbname, bool forceexpand)
{
    wxTreeItemId Item;

    wxTreeItemId DatabaseItem = GetItem(dbname, ID_TREE_DATABASE);
    if (!DatabaseItem.IsOk()||!m_db.IsOpen())
        return;

    bool bExpanded = m_TreeCtrl->IsExpanded(DatabaseItem) || forceexpand;
    m_TreeCtrl->DeleteChildren(DatabaseItem);

    Item = m_TreeCtrl->AppendItem(DatabaseItem, _("Tables"), ID_TREE_TABLES);
    Item = m_TreeCtrl->AppendItem(DatabaseItem, _("Views"), ID_TREE_VIEWS);
    Item = m_TreeCtrl->AppendItem(DatabaseItem, _("Index"), ID_TREE_INDEXES);
    Item = m_TreeCtrl->AppendItem(DatabaseItem, _("Triggers"), ID_TREE_TRIGGERS);

    RefreshTables(dbname);
    RefreshViews(dbname);
    RefreshIndexes(dbname);
    RefreshTriggers(dbname);

    if (bExpanded)
        m_TreeCtrl->Expand(DatabaseItem);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::RefreshTables(const wxString& dbname)
{
    wxSQLite3ResultSet tblQRY, clmnQRY;

    wxTreeItemId TableItem = GetItem(dbname, ID_TREE_TABLES);
    if (!TableItem.IsOk()||!m_db.IsOpen())
        return;
    bool bExpanded = m_TreeCtrl->IsExpanded(TableItem);
    m_TreeCtrl->DeleteChildren(TableItem);

    try
    {
        tblQRY = GetObjNameList(otTable, dbname);
        while (tblQRY.NextRow())
        {
            wxString tablename = tblQRY.GetString(0);
            int temp = tblQRY.GetInt(1);
            wxTreeItemId CurrentItem = m_TreeCtrl->AppendItem(TableItem, tablename,
                                       ID_TREE_TABLE + temp);
            wxString sql = wxString::Format(("PRAGMA '%s'.table_info(\"%s\");"),
                                            dbname.c_str(), tablename.c_str());
            try
            {
                clmnQRY = m_db.ExecuteQuery(ToUTF8(sql));
                while (clmnQRY.NextRow())
                    m_TreeCtrl->AppendItem(CurrentItem, clmnQRY.GetString(1),
                                           ID_TREE_FIELD);
            }
            catch(wxSQLite3Exception& ex)
            {
                wxGetApp().ShowError("RefreshTables", ex);
            }
            clmnQRY.Finalize();
        }
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("RefreshTables", ex);
    }
    tblQRY.Finalize();
    if (bExpanded)
        m_TreeCtrl->Expand(TableItem);
    else
        m_TreeCtrl->Collapse(TableItem);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::RefreshViews(const wxString& dbname)
{
    wxSQLite3ResultSet tblQRY, clmnQRY;

    wxTreeItemId ViewItem = GetItem(dbname, ID_TREE_VIEWS);
    if (!ViewItem.IsOk()||!m_db.IsOpen())
        return;
    bool bExpanded = m_TreeCtrl->IsExpanded(ViewItem);
    m_TreeCtrl->DeleteChildren(ViewItem);

    try
    {
        tblQRY = GetObjNameList(otView, dbname);
        while (tblQRY.NextRow())
        {
            wxString viewname = tblQRY.GetString(0);
            int temp = tblQRY.GetInt(1);
            wxTreeItemId CurrentItem = m_TreeCtrl->AppendItem(ViewItem, viewname,
                                       ID_TREE_VIEW + temp);
            wxString sql = wxString::Format(("PRAGMA '%s'.table_info(\"%s\");"),
                                            dbname.c_str(), viewname.c_str());
            try
            {
                clmnQRY = m_db.ExecuteQuery(ToUTF8(sql));
                while (clmnQRY.NextRow())
                    m_TreeCtrl->AppendItem(CurrentItem, clmnQRY.GetString(1),
                                           ID_TREE_FIELD);
            }
            catch(wxSQLite3Exception& ex)
            {
                wxGetApp().ShowError("RefreshViews", ex);
            }
            clmnQRY.Finalize();
        }
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("RefreshViews", ex);
    }
    tblQRY.Finalize();
    if (bExpanded)
        m_TreeCtrl->Expand(ViewItem);
    else
        m_TreeCtrl->Collapse(ViewItem);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::RefreshIndexes(const wxString& dbname)
{
    wxSQLite3ResultSet tblQRY, clmnQRY;

    wxTreeItemId IndexItem = GetItem(dbname, ID_TREE_INDEXES);
    if (!IndexItem.IsOk()||!m_db.IsOpen())
        return;
    bool bExpanded = m_TreeCtrl->IsExpanded(IndexItem);
    m_TreeCtrl->DeleteChildren(IndexItem);

    try
    {
        tblQRY = GetObjNameList(otIndex, dbname);
        while (tblQRY.NextRow())
        {
            wxString indexname = tblQRY.GetString(0);
            int temp = tblQRY.GetInt(1);
            wxTreeItemId CurrentItem = m_TreeCtrl->AppendItem(IndexItem, indexname,
                                       ID_TREE_INDEX + temp);
            wxString sql = wxString::Format(("PRAGMA '%s'.index_info(\"%s\");"),
                                            dbname.c_str(), indexname.c_str());
            try
            {
                clmnQRY = m_db.ExecuteQuery(ToUTF8(sql));
                while (clmnQRY.NextRow())
                    m_TreeCtrl->AppendItem(CurrentItem, clmnQRY.GetString(2),
                                           ID_TREE_FIELD);
            }
            catch(wxSQLite3Exception& ex)
            {
                wxGetApp().ShowError("RefreshIndexes", ex);
            }
            clmnQRY.Finalize();
        }
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("RefreshIndexes", ex);
    }
    tblQRY.Finalize();
    if (bExpanded)
        m_TreeCtrl->Expand(IndexItem);
    else
        m_TreeCtrl->Collapse(IndexItem);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::RefreshTriggers(const wxString& dbname)
{
    wxSQLite3ResultSet tblQRY, clmnQRY;

    wxTreeItemId TriggerItem = GetItem(dbname, ID_TREE_TRIGGERS);
    if (!TriggerItem.IsOk()||!m_db.IsOpen())
        return;
    bool bExpanded = m_TreeCtrl->IsExpanded(TriggerItem);
    m_TreeCtrl->DeleteChildren(TriggerItem);

    try
    {
        tblQRY = GetObjNameList(otTrigger, dbname);
        while (tblQRY.NextRow())
        {
            wxString triggername = tblQRY.GetString(0);
            int temp = tblQRY.GetInt(1);
            m_TreeCtrl->AppendItem(TriggerItem, triggername, ID_TREE_TRIGGER + temp);
        }
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("RefreshTriggers", ex);
    }
    tblQRY.Finalize();
    if (bExpanded)
        m_TreeCtrl->Expand(TriggerItem);
    else
        m_TreeCtrl->Collapse(TriggerItem);
}
/*---------------------------------------------------------------------------*/
wxTreeItemId wxSQLitePlusFrame::GetItem(const wxString& dbname, int imageId)
{
    wxTreeItemIdValue cookie;
    wxString name;

    wxTreeItemId root = m_TreeCtrl->GetRootItem();
    if (!root.IsOk())
        return root;

    if (dbname.IsEmpty())
        name = ("main");
    else
        name = dbname;

    wxTreeItemId itemdb = m_TreeCtrl->GetFirstChild(root, cookie);
    while (itemdb.IsOk())
    {
        if (m_TreeCtrl->GetItemText(itemdb) == name)
        {
            if (imageId == ID_TREE_DATABASE)
                return itemdb;
            wxTreeItemId item = m_TreeCtrl->GetFirstChild(itemdb, cookie);
            while (item.IsOk())
            {
                if (m_TreeCtrl->GetItemImage(item) == imageId)
                    return item;
                item = m_TreeCtrl->GetNextChild(itemdb, cookie);
            }
        }
        itemdb = m_TreeCtrl->GetNextChild(root, cookie);
    }

    return root;
}
/*---------------------------------------------------------------------------*/
wxString wxSQLitePlusFrame::GetDbNameFromItem(wxTreeItemId item)
{
    wxString dbname;
    int index;
    wxTreeItemId itemparent;

    index = m_TreeCtrl->GetItemImage(item);
    if (index == ID_TREE_DATABASE)
        dbname = m_TreeCtrl->GetItemText(item);
    else if (index > ID_TREE_DATABASE)
    {
        itemparent = m_TreeCtrl->GetItemParent(item);

        while (m_TreeCtrl->GetItemImage(itemparent) != ID_TREE_DATABASE)
            itemparent = m_TreeCtrl->GetItemParent(itemparent);

        dbname = m_TreeCtrl->GetItemText(itemparent);
    }

    return dbname;
}
/*---------------------------------------------------------------------------*/
wxTreeItemId wxSQLitePlusFrame::GetItemFromDbName(const wxString& dbname)
{
    wxTreeItemId root;
    wxTreeItemId itemdb;
    wxTreeItemIdValue cookie;

    root = m_TreeCtrl->GetRootItem();
    if (!root.IsOk())
        return root;
    itemdb = m_TreeCtrl->GetFirstChild(root, cookie);
    while (itemdb.IsOk())
    {
        if (m_TreeCtrl->GetItemText(itemdb) == dbname)
            return itemdb;
        itemdb = m_TreeCtrl->GetNextChild(root, cookie);
    }

    return root;
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::DatabaseExist(const wxString& dbname, bool alias,
                                      wxString* val)
{
    wxSQLite3ResultSet dbQRY;

    if (m_db.IsOpen())
    {
        int index = alias ? 1 : 2;
        int ret = alias ? 2 : 1;
        try
        {
            dbQRY = m_db.ExecuteQuery(ToUTF8(("PRAGMA database_list;")));
            while (dbQRY.NextRow())
            {
                if (dbname == dbQRY.GetString(index))
                {
                    if (val)
                        *val = dbQRY.GetString(ret);
                    dbQRY.Finalize();
                    return true;
                }
            }
        }
        catch(wxSQLite3Exception& ex)
        {
            wxGetApp().ShowError("DatabaseExist", ex);
        }
        dbQRY.Finalize();
    }
    return false;
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::CloseDatabase()
{
    try
    {
        wxSQLBook* book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            if (!book->DoClose())
                return false;
            book->SetAutoClose(false);
        }
        while (m_CenterNotebook->GetPageCount())
            m_CenterNotebook->DeletePage(0);

        m_DbName = wxEmptyString;
        if (m_db.IsOpen())
        {
            m_db.SetUpdateHook(NULL);
            m_db.Close();
            InitTree();
        }
        return true;
    }
    catch(wxSQLite3Exception& /*ex*/)
    {
        wxMessageBox("Close Database Error");
        return false;
    }
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::OpenDatabase(const wxString& dbfullname, const wxString& name)
{
    wxString msg, l_dbkey;
    bool ret = false;

    try
    {
        if (!CloseDatabase())
            return false;

        m_DbName = name;
        m_db.Open(dbfullname);
        wxSQLite3Cipher* cipher = nullptr;
        if (m_db.IsOpen()&&m_db.HasEncryptionSupport())
        {
            do
            {
                try
                {
                    wxSQLite3ResultSet dbQRY = m_db.ExecuteQuery(ToUTF8(("SELECT * FROM sqlite_master;")));
                    ret = true;
                }
                catch(wxSQLite3Exception& ex)
                {
                    m_db.Close();
                    wxString ErrStr = wxString::Format(_("Error %i-%i : %s\n"),
                                                       ex.GetErrorCode(),
                                                       ex.GetExtendedErrorCode(),
                                                       ex.GetMessage().c_str());
                    wxMessageBox(ErrStr, "Open Database Error");
                    if (ex.GetErrorCode() == 26 && ex.GetExtendedErrorCode() == 26)
                    {
                        wxDbEncryptionKeyDlg encryptionKeyDlg(this);

                        if (encryptionKeyDlg.ShowModal() == wxID_OK)
                            l_dbkey = encryptionKeyDlg.GetKey();  // ok
                        else
                            return false;  // Annuler retour sans ouvrir
                        for (size_t index = 0; index < this->m_Cipher.size(); ++ index)
                        {
                            cipher = this->m_Cipher[index];
                            try
                            {
                                m_db.Open(dbfullname, *cipher, l_dbkey);
                                m_db.ExecuteQuery(ToUTF8(("SELECT * FROM sqlite_master;")));
                                break;
                            }
                            catch (wxSQLite3Exception& ex)
                            {
                                // silently and goto do-while
                            }
                        }
                    }
                    else
                        throw;
                }
            }
            while (ret == false);
        }
        else if (!m_db.IsOpen())
            return false;

        InitTree();
        // Le 'Hook' est posé obligatoirement après l'ouverture
        m_db.SetUpdateHook(&m_Hook);
        // Ajoute commit et rollback hook
        wxSQLBook* SQLBook = new wxSQLBook(m_CenterNotebook, ID_SQL_PANEL,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTAB_TRAVERSAL);
        SQLBook->SetDatabaseAndHook(&m_db, &m_Hook);
        SQLBook->SetAutoTransact(m_MnuTransact->IsChecked(ID_MNU_AUTOTRANSACT));
        m_CenterNotebook->AddPage(SQLBook, _("SQL"), true,
                                  wxGetApp().GetBmp(ID_BMP_RUNSQL));
        m_auiManager.Update();
        msg = wxString::Format(_("SQLite3 Version %s\n"), m_db.GetVersion().c_str());
        SQLBook->GetLogResult()->AppendText(msg);
        msg = wxString::Format(_("The database \"%s\" is opened as main with %s\n"), dbfullname.c_str(), cipher == nullptr ? wxString("empty cipher"): wxSQLite3Cipher::GetCipherName(cipher->GetCipherType()));
        SQLBook->GetLogResult()->AppendText(msg);

        SQLBook->ShowLog();
        SQLBook->GetSQLEdit()->SetFocus();
        return true;
    }
    catch(wxSQLite3Exception& ex)
    {
        wxString ErrStr = wxString::Format(_("Error %i-%i : %s\n"),
                                           ex.GetErrorCode(),
                                           ex.GetExtendedErrorCode(),
                                           ex.GetMessage().c_str());
        wxMessageBox(ErrStr, "Open Database Error");
        return false;
    }
    /*
        wxPanel* itemPanel5 = new wxPanel(m_CenterNotebook, ID_TABLE_PANEL,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxTAB_TRAVERSAL);

        wxBitmap itemPanel5Bitmap(wxGetApp().GetBmp(wxT("images/booktable.xpm")));
        m_CenterNotebook->AddPage(itemPanel5, _("Table"), true, itemPanel5Bitmap);
    */
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::AttachDatabase(const wxString& dbfile,
                                       const wxString& dbalias,
                                       const wxString& dbkey)
{
    wxString sql, msg, l_dbkey;
    wxTreeItemId root;
    wxTreeItemId DatabaseItem;
    wxSQLBook* book;
    bool ret = false;

    if (DoFinishTransaction()) // vérifier si transaction finie
    {
        l_dbkey = dbkey;
        try
        {
            wxSQLite3Cipher *cipher = nullptr;
            do
            {
                try
                {
                    for (size_t index = 0; index < this->m_Cipher.size(); ++ index)
                    {
                        cipher = this->m_Cipher[index];
                        try
                        {
                            m_db.AttachDatabase(dbfile, dbalias, *cipher, l_dbkey);
                            ret = true; // Ouverture correcte continuer
                            break;
                        }
                        catch (wxSQLite3Exception& ex)
                        {
                            if (index == this->m_Cipher.size() - 1)
                            {
                                throw ex;
                            }
                        }
                    }
                }
                catch(wxSQLite3Exception& ex)
                {   // Capture de l'exception n'est pas une base ou est criptée
                    if (ex.GetErrorCode() == 26 && ex.GetExtendedErrorCode() == 26)
                    {   // Affichage du message
                        wxString ErrStr = wxString::Format(_("Error %i-%i : %s\n"),
                                                           ex.GetErrorCode(),
                                                           ex.GetExtendedErrorCode(),
                                                           ex.GetMessage().c_str());
                        wxMessageBox(ErrStr, ("Open Database Error"));
                        // Affichage du dialog de saisie de la clef
                        wxAttachDbDlg attachDbDlg(this);

                        attachDbDlg.SetDatabase(&m_db);
                        attachDbDlg.SetFileName(dbfile);
                        attachDbDlg.SetAlias(dbalias);
                        attachDbDlg.SetKey(l_dbkey);
                        attachDbDlg.OnlyKey();  // Juste la clef
                        if (attachDbDlg.ShowModal() == wxID_OK)
                            l_dbkey = attachDbDlg.GetKey();  // ok
                        else
                            return false;  // Annuler retour sans ouvrir
                    }
                    else
                        throw;   // Autre Exception : propager l'exception
                }
            }
            while (ret == false);

            // Mettre à jour le wxTreeCtrl
            root = m_TreeCtrl->GetRootItem();
            if (!root.IsOk())
                return false;
            DatabaseItem = m_TreeCtrl->AppendItem(root, dbalias, ID_TREE_DATABASE);
            RefreshDatabase(dbalias, true);
            book = (wxSQLBook*)GetPage("SQL");
            if (book)
            {
                // si dbkey est non nulle modifgier le message pour indiquer que la base est chiffrée
                msg = wxString::Format(_("The database \"%s\" is attached as %s with %s\n"),
                                       dbfile.c_str(), dbalias.c_str(), l_dbkey.IsEmpty() ? wxString("empty cipher") : wxSQLite3Cipher::GetCipherName(cipher->GetCipherType()));

                book->GetLogResult()->AppendText(msg);
                book->ShowLog();
            }

            return true;
        }
        catch(wxSQLite3Exception& ex)
        {
            wxGetApp().ShowError("AttachDatabase", ex);
        }
    }
    return false;
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::EncryptDatabase(const wxString& dbkey)
{
    //if (DoFinishTransaction()) // vérifier si transaction finie
    {
        try
        {
            m_db.ReKey(*(this->m_Cipher.back()), dbkey);
        }
        catch(wxSQLite3Exception& ex)
        {
            wxGetApp().ShowError("EncryptDatabase", ex);
        }
    }
    return false;
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnEventClick(wxCommandEvent& event)
{
    int index = m_CenterNotebook->GetSelection();
    if ((index != -1)&&(m_CenterNotebook->GetPageText(index) == "SQL"))
    {
        wxWindow* window = m_CenterNotebook->GetPage(index);
        if (window)
            window->GetEventHandler()->ProcessEvent(event);
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnEventUpdate(wxUpdateUIEvent& event)
{
    int index = m_CenterNotebook->GetSelection();

    if ((index != -1)&&(m_CenterNotebook->GetPageText(index) == "SQL"))
    {
        wxWindow* window = m_CenterNotebook->GetPage(index);
        if (window)
            window->GetEventHandler()->ProcessEvent(event);
    }
    else
        event.Enable(false);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnCopyClick(wxCommandEvent& event)
{
    int index = m_CenterNotebook->GetSelection();
    if (index != -1)
    {
        wxWindow* window = m_CenterNotebook->GetPage(index);
        if (window)
            window->GetEventHandler()->ProcessEvent(event);
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnCopyUpdate(wxUpdateUIEvent& event)
{
    int index = m_CenterNotebook->GetSelection();

    if (index != -1)
    {
        wxWindow* window = m_CenterNotebook->GetPage(index);
        if (window)
            window->GetEventHandler()->ProcessEvent(event);
    }
    else
        event.Enable(false);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuCloseDbUpdate(wxUpdateUIEvent& event)
{
    bool b_enable = false;
    try
    {
        b_enable = m_db.IsOpen();
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("OnMnuCloseDbUpdate", ex);
    }
    event.Enable(b_enable);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnDetachDbUpdate(wxUpdateUIEvent& event)
{
    event.Enable(HaveAttachedDatabase());
}
/*---------------------------------------------------------------------------*/
wxWindow* wxSQLitePlusFrame::GetPage(const wxString& name)
{
    size_t count = m_CenterNotebook->GetPageCount();
    for (size_t i = 0; i < count; i++)
    {
        if (m_CenterNotebook->GetPageText(i) == name)
            return m_CenterNotebook->GetPage(i);
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAutotransactClick(wxCommandEvent& event)
{
    wxSQLBook* book = (wxSQLBook*)GetPage("SQL");
    if (book)
        book->SetAutoTransact(event.IsChecked());
    m_MnuTransact->Check(ID_MNU_AUTOTRANSACT, event.IsChecked());
    m_TbTransact->ToggleTool(ID_MNU_AUTOTRANSACT, event.IsChecked());
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuCloseDbClick(wxCommandEvent& event)
{
    CloseDatabase();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuBackupDbClick(wxCommandEvent& event)
{
    wxBackupRestoreDbDlg backupDbDlg(this, true);

    backupDbDlg.SetDatabase(&m_db, m_NodeDbName);
    if (backupDbDlg.ShowModal() == wxID_OK)
    {
        try
        {
            m_db.Backup(backupDbDlg.GetFileName(), backupDbDlg.GetKey(),
                        backupDbDlg.GetAlias());
        }
        catch(wxSQLite3Exception& ex)
        {
            wxString ErrStr = wxString::Format(_("Error %i-%i : %s\n"),
                                               ex.GetErrorCode(),
                                               ex.GetExtendedErrorCode(),
                                               ex.GetMessage().c_str());
            wxMessageBox(ErrStr, ("Backup Database Error"));
        }
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuRestoreDbClick(wxCommandEvent& event)
{
    wxBackupRestoreDbDlg restoreDbDlg(this, false);

    restoreDbDlg.SetDatabase(&m_db, m_NodeDbName);
    if (restoreDbDlg.ShowModal() == wxID_OK)
    {
        try
        {
            m_db.Restore(restoreDbDlg.GetFileName(), restoreDbDlg.GetKey(),
                         restoreDbDlg.GetAlias());
        }
        catch(wxSQLite3Exception& ex)
        {
            wxString ErrStr = wxString::Format(_("Error %i-%i : %s\n"),
                                               ex.GetErrorCode(),
                                               ex.GetExtendedErrorCode(),
                                               ex.GetMessage().c_str());
            wxMessageBox(ErrStr, ("Restore Database Error"));
        }
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnDetachDbClick(wxCommandEvent& event)
{
    wxArrayString aliaslist;
    wxTreeItemId root;
    wxTreeItemId DatabaseItem;
    int count  = 0;
    wxString sql;
    wxWindow* win;
    wxDbBook* dbBook = NULL;

    wxDetachDbDlg detachDbDlg(this);

    detachDbDlg.SetDatabase(&m_db, m_NodeDbName);
    if (detachDbDlg.ShowModal() == wxID_OK)
    {
        count = detachDbDlg.GetAliasList(aliaslist);
        if (count > 0 && DoFinishTransaction()) // vérifier si transaction finie
        {
            root = m_TreeCtrl->GetRootItem();
            if (!root.IsOk()||!m_db.IsOpen())
                return;
            for (int i = 0; i < count; i++)
            {
                sql = ("DETACH DATABASE ") + aliaslist[i] + (";");
                try
                {
                    m_db.ExecuteUpdate(sql);
                    // Supprimer les explorateur pour cet alias
                    size_t count_page = m_CenterNotebook->GetPageCount();
                    if (count_page)
                    {
                        for (int ii = (int)count_page - 1; ii >= 0 ; ii--)
                        {
                            win = m_CenterNotebook->GetPage(ii);
                            if (win)
                            {
                                dbBook = wxDynamicCast(win, wxDbBook);
                                if (dbBook && (dbBook->GetBaseName() == aliaslist[i]))
                                    m_CenterNotebook->DeletePage(ii);
                            }
                        }
                    }
                    // Mettre à jour le wxTreeCtrl
                    DatabaseItem = GetItem(aliaslist[i], ID_TREE_DATABASE);
                    if (DatabaseItem.IsOk())
                        m_TreeCtrl->Delete(DatabaseItem);
                }
                catch(wxSQLite3Exception& ex)
                {
                    wxGetApp().ShowError("OnDetachDbClick", ex);
                }
            }
        }
    }
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::DropDbObject(wxSQLiteObjectType type)
{
    wxString ObjName, SQL, dbname;
    wxSQLiteObjectType ObjType;
    wxSQLBook* book;

    wxDropObject dropObject(this);
    dropObject.SetDatabase(&m_db, m_NodeDbName);
    if ((type != otNone)&&(type != otTableAndView))
    {
        dropObject.SetObjectType(type);
        dropObject.DisableType();
        if (m_NodeType == ID_TREE_TABLE || m_NodeType == ID_TREE_VIEW ||
                m_NodeType == ID_TREE_INDEX || m_NodeType == ID_TREE_TRIGGER ||
                m_NodeType == ID_TREE_TABLE_TMP || m_NodeType == ID_TREE_VIEW_TMP ||
                m_NodeType == ID_TREE_INDEX_TMP || m_NodeType == ID_TREE_TRIGGER_TMP)
        {
            dropObject.SetObjectName(m_NodeText);
            dropObject.DisableName();
        }
    }

    if (dropObject.ShowModal() == wxID_OK)
    {
        ObjType = dropObject.GetObjectType();
        ObjName = dropObject.GetObjectName();
        if (m_NodeDbName.IsEmpty())
            dbname = ("main");
        else
            dbname = m_NodeDbName;
        SQL = ("DROP ") + GetObjTypeName(ObjType) + (" ");
        SQL += dbname + (".\"") + ObjName + _("\";");
        book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            book->ExecQuery(SQL);
            switch (ObjType)
            {
            case otTable   :
                RefreshTables(m_NodeDbName);
                break;
            case otView    :
                RefreshViews(m_NodeDbName);
                break;
            case otIndex   :
                RefreshIndexes(m_NodeDbName);
                break;
            case otTrigger :
                RefreshTriggers(m_NodeDbName);
                break;
            default :
                ;
            }
            m_TreeCtrl->Refresh();
            m_TreeCtrl->Update();
        }
    }
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateIndex(const wxString& dbname,
                                    const wxString& tablename)
{
    wxSQLBook* book;
    wxCreateIndexDlg createIndexDlg(this);

    createIndexDlg.SetDatabase(&m_db, dbname);
    if (!tablename.IsEmpty())
        createIndexDlg.SetTable(tablename);
    if (createIndexDlg.ShowModal() == wxID_OK)
    {
        book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            book->ExecQuery(createIndexDlg.GetCreateIndexDDL());
            RefreshIndexes(dbname);
            m_TreeCtrl->Refresh();
            m_TreeCtrl->Update();
        }
    }
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateTable(const wxString& dbname)
{
    wxSQLBook* book;
    wxCreateTableDialog createTableDlg(this);

    createTableDlg.SetDatabase(&m_db, dbname);
    if (createTableDlg.ShowModal() == wxID_OK)
    {
        book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            book->ExecQuery(createTableDlg.GetCreateTableDDL());
            RefreshTables(createTableDlg.GetTemporary() ? _("main") : dbname);
            m_TreeCtrl->Refresh();
            m_TreeCtrl->Update();
        }
    }
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateView(const wxString& dbname)
{
    wxSQLBook* book;
    wxCreateViewDialog createViewDlg(this);

    createViewDlg.SetDatabase(&m_db, dbname);
    if (createViewDlg.ShowModal() == wxID_OK)
    {
        book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            book->ExecQuery(createViewDlg.GetCreateViewDDL());
            RefreshViews(createViewDlg.GetTemporary() ? _("main") : dbname);
            m_TreeCtrl->Refresh();
            m_TreeCtrl->Update();
        }
    }
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::CreateTrigger(const wxString& dbname, bool useobj,
                                      bool istable, const wxString& objname)
{
    wxSQLBook* book;
    wxCreateTriggerDialog createTriggerDlg(this);

    createTriggerDlg.SetDatabase(&m_db, dbname);
    if (useobj)
        createTriggerDlg.SetObject(objname, istable);
    if (createTriggerDlg.ShowModal() == wxID_OK)
    {
        book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            book->ExecQuery(createTriggerDlg.GetCreateTriggerDDL());
            RefreshTriggers(createTriggerDlg.GetTemporary() ? _("main") : dbname);
            m_TreeCtrl->Refresh();
            m_TreeCtrl->Update();
        }
    }
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::AddColumns(const wxString& dbname,
                                   const wxString& tablename)
{
    wxSQLBook* book;
    wxAddColumnDialog addColumnDlg(this);

    addColumnDlg.SetDatabase(&m_db, dbname);
    if (!tablename.IsEmpty())
        addColumnDlg.SetTable(tablename);
    if (addColumnDlg.ShowModal() == wxID_OK)
    {
        book = (wxSQLBook*)GetPage("SQL");
        if (book)
        {
            book->ExecScript(addColumnDlg.GetAddColumnsScript());
            RefreshTables(dbname);
            m_TreeCtrl->Refresh();
            m_TreeCtrl->Update();
        }
    }
    InitNodeParams();
}
/*---------------------------------------------------------------------------*/
wxMenu* wxSQLitePlusFrame::GetPopupMenu(int type)
{
    wxMenu* menu = NULL;
    bool branch = false;
    switch (type)
    {
    case ID_TREE_DATABASE    :
        menu = CreateMenuDatabase();
        break;
    case ID_TREE_TABLES      :
        branch = true;
    case ID_TREE_TABLE       :
    case ID_TREE_TABLE_TMP   :
        menu = CreateMenuTables();
        break;
    case ID_TREE_VIEWS       :
        branch = true;
    case ID_TREE_VIEW        :
    case ID_TREE_VIEW_TMP    :
        menu = CreateMenuViews();
        break;
    case ID_TREE_INDEXES     :
        branch = true;
    case ID_TREE_INDEX       :
    case ID_TREE_INDEX_TMP   :
        menu = CreateMenuIndexes();
        break;
    case ID_TREE_TRIGGERS    :
        branch = true;
    case ID_TREE_TRIGGER     :
    case ID_TREE_TRIGGER_TMP :
        menu = CreateMenuTriggers();
    };
    if (menu)
    {
        if (branch)
            menu->Enable(ID_MNU_EXPLORE, false);
        //else
        //   menu->Enable(wxID_REFRESH, false);
    }
    return menu;
}
/*---------------------------------------------------------------------------*/
wxMenu* wxSQLitePlusFrame::CreateMenuDatabase()
{
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_REFRESH, _("Refresh"),
                 _("Refresh database structure"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(wxID_OPEN, _("Open/Create"),
                 _("Open an existing database or create a new database"),
                 wxITEM_NORMAL);
    menu->Append(ID_MNU_CLOSE_DB, _("Close"),
                 _("Close the current database"), wxITEM_NORMAL);
    menu->Append(ID_MNU_DETACH_DB, _("Detach database"),
                 _("Detach an attached database"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_BACKUP, _("Backup"),
                 _("Backup a database"), wxITEM_NORMAL);
    menu->Append(ID_MNU_RESTORE, _("Restore"),
                 _("Restore a database"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_DROP_OBJECT, _("Drop Object"),
                 _("Show dialog to delete database object"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_ADD_TABLE, _("Create Table"),
                 _("Show dialog to create new table"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_VIEW, _("Create View"),
                 _("Show dialog to create new view"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_INDEX, _("Create Index"),
                 _("Show dialog to create new index"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_TRIGGER, _("Create Trigger"),
                 _("Show dialog to create new trigger"), wxITEM_NORMAL);
    return menu;
}
/*---------------------------------------------------------------------------*/
wxMenu* wxSQLitePlusFrame::CreateMenuTables()
{
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_REFRESH, _("Refresh"),
                 _("Refresh tables database structure"), wxITEM_NORMAL);
    menu->Append(ID_MNU_EXPLORE, _("Explore"),
                 _("Explore the selected table"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_DEL_TABLE, _("Drop Table"),
                 _("Delete the selected table"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_TABLE, _("Create Table"),
                 _("Show dialog to create new table"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_INDEX, _("Create Index"),
                 _("Show dialog to create new index"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_TRIGGER, _("Create trigger"),
                 _("Show dialog to create new trigger"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_ADD_COLUMNS, _("Add columns"),
                 _("Show dialog to add columns to table"), wxITEM_NORMAL);
    return menu;
}
/*---------------------------------------------------------------------------*/
wxMenu* wxSQLitePlusFrame::CreateMenuIndexes()
{
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_REFRESH, _("Refresh"),
                 _("Refresh indexes database structure"), wxITEM_NORMAL);
    menu->Append(ID_MNU_EXPLORE, _("Explore"),
                 _("Explore the selected index"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_DEL_INDEX, _("Drop Index"),
                 _("Delete the selected index"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_INDEX, _("Create Index"),
                 _("Show dialog to create new index"), wxITEM_NORMAL);
    return menu;
}
/*---------------------------------------------------------------------------*/
wxMenu* wxSQLitePlusFrame::CreateMenuViews()
{
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_REFRESH, _("Refresh"),
                 _("Refresh views database structure"), wxITEM_NORMAL);
    menu->Append(ID_MNU_EXPLORE, _("Explore"),
                 _("Explore the selected view"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_DEL_VIEW, _("Drop View"),
                 _("Delete the selected view"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_VIEW, _("Create View"),
                 _("Show dialog to create new view"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_TRIGGER, _("Create &trigger"),
                 _("Show dialog to create new trigger"), wxITEM_NORMAL);
    return menu;
}
/*---------------------------------------------------------------------------*/
wxMenu* wxSQLitePlusFrame::CreateMenuTriggers()
{
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_REFRESH, _("Refresh"),
                 _("Refresh triggers database structure"), wxITEM_NORMAL);
    menu->Append(ID_MNU_EXPLORE, _("Explore"),
                 _("Explore the selected trigger"), wxITEM_NORMAL);
    menu->AppendSeparator();
    menu->Append(ID_MNU_DEL_TRIGGER, _("Drop Trigger"),
                 _("Delete the selected trigger"), wxITEM_NORMAL);
    menu->Append(ID_MNU_ADD_TRIGGER, _("Create Trigger"),
                 _("Show dialog to create new trigger"), wxITEM_NORMAL);
    return menu;
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnExitClick(wxCommandEvent& event)
{
    Close();
    event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuExploreClick(wxCommandEvent& event)
{
    if (!TryToRefresh())
    {
        if (m_NodeType == ID_TREE_TABLE || m_NodeType == ID_TREE_TABLE_TMP)
        {
            wxTableBook* TableBook;
            TableBook = new wxTableBook(m_CenterNotebook);
            m_CenterNotebook->AddPage(TableBook, m_NodeText, true,
                                      wxGetApp().GetBmp(ID_BMP_TABLE));
            TableBook->SetDbObjectName(&m_db, m_NodeText, m_NodeDbName);
            m_auiManager.Update();
        }
        else if (m_NodeType == ID_TREE_VIEW || m_NodeType == ID_TREE_VIEW_TMP)
        {
            wxViewBook* ViewBook;
            ViewBook = new wxViewBook(m_CenterNotebook);
            m_CenterNotebook->AddPage(ViewBook, m_NodeText, true,
                                      wxGetApp().GetBmp(ID_BMP_VIEW));
            ViewBook->SetDbObjectName(&m_db, m_NodeText, m_NodeDbName);
            m_auiManager.Update();
        }
        else if (m_NodeType == ID_TREE_INDEX || m_NodeType == ID_TREE_INDEX_TMP)
        {
            wxIndexBook* IndexBook;
            IndexBook = new wxIndexBook(m_CenterNotebook);
            m_CenterNotebook->AddPage(IndexBook, m_NodeText, true,
                                      wxGetApp().GetBmp(ID_BMP_INDEXE));
            IndexBook->SetDbObjectName(&m_db, m_NodeText, m_NodeDbName);
            m_auiManager.Update();
        }
        else if (m_NodeType == ID_TREE_TRIGGER || m_NodeType == ID_TREE_TRIGGER_TMP)
        {
            wxTriggerBook* triggerBook;
            triggerBook = new wxTriggerBook(m_CenterNotebook);
            m_CenterNotebook->AddPage(triggerBook, m_NodeText, true,
                                      wxGetApp().GetBmp(ID_BMP_TRIGGER2));
            triggerBook->SetDbObjectName(&m_db, m_NodeText, m_NodeDbName);
            m_auiManager.Update();
        }
    }
    InitNodeParams();
    event.Skip();
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuExploreUpdate(wxUpdateUIEvent& event)
{
    bool b_enable = false;

    if (m_NodeType >= ID_TREE_TABLE &&  m_NodeType <= ID_TREE_TRIGGER_TMP)
        b_enable = true;

    event.Enable(b_enable);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddIndexUpdate(wxUpdateUIEvent& event)
{
    bool b_enable = false;
    try
    {
        b_enable = m_db.IsOpen();
        if (b_enable)
        {
            wxTreeItemId item;
            if (m_NodeType == ID_TREE_TABLES)
            {
                item = GetItem(m_NodeDbName, ID_TREE_TABLES);
                b_enable = m_TreeCtrl->ItemHasChildren(item);
            }
        }
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("OnMnuAddIndexUpdate", ex);
    }
    event.Enable(b_enable);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnMnuAddTriggerUpdate(wxUpdateUIEvent& event)
{
    bool b_enable = false;
    try
    {
        b_enable = m_db.IsOpen();
        if (b_enable)
        {
            wxTreeItemId item;
            if (m_NodeType == ID_TREE_TABLES)
            {
                item = GetItem(m_NodeDbName, ID_TREE_TABLES);
                b_enable = m_TreeCtrl->ItemHasChildren(item);
            }
            else if (m_NodeType == ID_TREE_VIEWS)
            {
                item = GetItem(m_NodeDbName, ID_TREE_VIEWS);
                b_enable = m_TreeCtrl->ItemHasChildren(item);
            }
        }
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("OnMnuAddTriggerUpdate", ex);
    }
    event.Enable(b_enable);
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    wxWindow* win = ctrl->GetPage(evt.GetSelection());
    if (win && win->IsKindOf(CLASSINFO(wxSQLBook)))
    {
        if (!((wxSQLBook*)win)->DoClose())
            evt.Veto();
    }
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::TryToRefresh()
{
    wxWindow* win;
    wxDbBook* dbBook;
    wxString alias;

    if (m_NodeType < ID_TREE_TABLE || m_NodeType > ID_TREE_TRIGGER_TMP)
        return false;
    alias = m_NodeDbName;
    if (alias.IsEmpty())
        alias = ("main");
    size_t count = m_CenterNotebook->GetPageCount();
    for (size_t i = 0; i < count; i++)
    {
        if (m_CenterNotebook->GetPageText(i) == m_NodeText)
        {
            win = m_CenterNotebook->GetPage(i);
            if (win)
            {
                dbBook = wxDynamicCast(win, wxDbBook);
                if (dbBook && (dbBook->GetBaseName() == alias))
                {
                    dbBook->RefreshDbObject();
                    m_CenterNotebook->SetSelection(i);
                    return true;
                }
            }
        }
    }
    return false;
}
/*---------------------------------------------------------------------------*/
void wxSQLitePlusFrame::HideShowPane(wxWindow* win)
{
    wxAuiPaneInfo& pane =  m_auiManager.GetPane(win);

    pane.Show(!pane.IsShown());
    m_auiManager.Update();
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::IsShownPane(wxWindow* win)
{
    wxAuiPaneInfo& pane =  m_auiManager.GetPane(win);

    return pane.IsShown();
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::HaveAttachedDatabase()
{
    wxTreeItemId root;

    root = m_TreeCtrl->GetRootItem();
    if (!root.IsOk())
        return false;

    return (m_TreeCtrl->GetChildrenCount(root, false) > 1);
}
/*---------------------------------------------------------------------------*/
wxSQLite3ResultSet wxSQLitePlusFrame::GetObjNameList(wxSQLiteObjectType objtype,
        const wxString& dbname)
{
    wxSQLite3ResultSet objQRY;
    wxString sql, where;

    sql = ("SELECT name, 0 AS tmp ");
    if (objtype == otTableAndView)
        where = ("WHERE type IN ('table', 'view') AND name <> 'sqlite_sequence'");
    else
    {
        where = ("WHERE type = '") + GetObjTypeName(objtype) + ("'");
        if ((objtype == otTable)||(objtype == otView))
            where += (" AND name <> 'sqlite_sequence'");
    }
    if ((dbname == wxEmptyString)||(dbname == ("main")))
    {
        sql += ("FROM sqlite_master ");
        sql += where + (" UNION ");
        sql += ("SELECT name, 1 AS tmp FROM sqlite_temp_master ") + where;
    }
    else
    {
        sql += ("FROM '") + dbname + ("'.sqlite_master ") + where;
    }
    sql += (" ORDER by name;");

    objQRY = m_db.ExecuteQuery(ToUTF8(sql));

    return objQRY;
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::ExistDbObject(wxSQLiteObjectType objtype,
                                      const wxString& name,
                                      const wxString& dbname)
{
    wxString sql, where, whtype;

    if (objtype == otNone)
        return false;

    if (objtype == otTableAndView)
        whtype = ("type IN ('table', 'view') ");
    else
        whtype = ("type = '") + GetObjTypeName(objtype) + ("' ");

    where = ("WHERE name = '") + name + ("' AND ") + whtype;

    sql = ("SELECT MAX(c) FROM (");
    if ((dbname == wxEmptyString)||(dbname == ("main")))
    {
        sql += ("SELECT COUNT(*) c FROM sqlite_master ") + where;
        sql += ("UNION SELECT COUNT(*) c FROM sqlite_temp_master ") + where;
    }
    else
        sql += ("SELECT COUNT(*) c FROM '") + dbname + ("'.sqlite_master ") + where;
    sql += (")");

    try
    {
        if (m_db.ExecuteScalar(sql) != 0)
            return true;
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("ExistDbObject", ex);
    }
    return false;
}
/*---------------------------------------------------------------------------*/
wxString wxSQLitePlusFrame::GetObjTypeName(wxSQLiteObjectType objtype)
{
    if ((objtype == otNone)||(objtype == otTableAndView))
        return g_SQLiteObjTypeName[otTableAndView];
    else
        return g_SQLiteObjTypeName[objtype];
}
/*---------------------------------------------------------------------------*/
wxSQLiteObjectType wxSQLitePlusFrame::GetObjNameType(const wxString& name)
{
    wxString tmp = name.Lower();
    for (int i = otTable; i < otTableAndView; i++)
        if (GetObjTypeName((wxSQLiteObjectType)i) == tmp)
            return (wxSQLiteObjectType)i;
    return otNone;
}
/*---------------------------------------------------------------------------*/
wxString wxSQLitePlusFrame::GetDbObjectDDL(wxSQLiteObjectType objtype,
        const wxString& name,
        const wxString& dbname)
{
    wxString sql, basename, objname, ret;
    wxSQLite3ResultSet resultSet;

    if (dbname != wxEmptyString)
        basename = dbname;
    else
        basename = ("main");
    objname = GetObjTypeName(objtype);

    try
    {   // remplissage de l'éditeur DDL
        if (basename == ("main"))
        {
            sql = wxString::Format(("SELECT sql FROM sqlite_master\n"
                                    "WHERE type = '%s' AND name = '%s'\n"
                                    "UNION\n"
                                    "SELECT sql FROM sqlite_temp_master\n"
                                    "WHERE type = '%s' AND name = '%s';"),
                                   objname.c_str(), name.c_str(),
                                   objname.c_str(), name.c_str());
        }
        else
        {
            sql = wxString::Format(("SELECT sql FROM '%s'.sqlite_master\n"
                                    "WHERE type = '%s' AND name = '%s';"),
                                   basename.c_str(), objname.c_str(), name.c_str());
        }


        resultSet = m_db.ExecuteQuery(ToUTF8(sql));
        if (resultSet.NextRow())
            ret = resultSet.GetString(0);
        resultSet.Finalize();
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("GetDbObjectDDL", ex);
    }
    return ret;
}
/*---------------------------------------------------------------------------*/
bool wxSQLitePlusFrame::DoFinishTransaction()
{
    try
    {
        if (!m_db.GetAutoCommit())
        {
            int answer = wxMessageBox(_("A transaction is running.\n"
                                        "Do you want to COMMIT your change?"),
                                      _("Warning"), wxYES_NO | wxCANCEL);
            switch (answer)
            {
            case wxYES :
                m_db.ExecuteUpdate("COMMIT;");
                break;
            case wxNO  :
                m_db.ExecuteUpdate("ROLLBACK;");
                break;
            default    :
                return false;
            }
        }
        return true;
    }
    catch(wxSQLite3Exception& ex)
    {
        wxGetApp().ShowError("DoFinishTransaction", ex);
        return false;
    }
}
/*---------------------------------------------------------------------------*/
