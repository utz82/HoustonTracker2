
class ht2UtilGUI: public wxApp { 
public:
	virtual bool OnInit(); 
};

class stateDropTarget: public wxTextDropTarget
{
public:
	stateDropTarget(wxListCtrl *owner);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	wxListCtrl *m_owner;
};

class exportDropTarget: public wxTextDropTarget
{
public:
	exportDropTarget(wxListCtrl *owner);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	wxListCtrl *m_owner;
};


class mainFrame: public wxFrame
{
public:
    mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    
    wxMenu *menuFile;

    wxStaticText *htFileInfo;
    wxStaticText *htSizeInfo;
    wxListCtrl *savestateList;
    wxListCtrl *directoryList;
    
    wxString CurrentDocPath;
    wxString CurrentFileName;
    wxString fileExt;
    wxString currentStateDoc;
    wxString currentAsmDoc;
    wxUint8 *stateData;
    wxFileOffset stateSize;
    unsigned totalSsize;			//total size of all savestates
    
    int tmodel,baseOffset,fOffset;
    unsigned htver,statever,lutOffset,baseDiff;
    bool legacyFileEnd, unsavedChanges;


    wxFileOffset htsize;
    wxUint8 *htdata;
    
    wxString *dirList;
    wxString *fileList;
    wxString *fileSizeList;
    wxInt16 noDirs;
    wxInt16 noFiles;
    bool dotdot;
    wxString currentFBDir;			//current file browser path
    wxImageList *fbIcons;

    unsigned statebeg[8], statelen[8];
    

    
    
private:
    void OnOpenHT(wxCommandEvent& event);
    void OnCloseHT(wxCommandEvent& event);
    void OnSaveHT(wxCommandEvent& event);
    void OnSaveAsHT(wxCommandEvent& event);
    void OnExtractState(wxCommandEvent& event);
    void OnInsertState(wxCommandEvent& event);
    void OnDeleteState(wxCommandEvent& event);
    void OnExportAsm(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void XExit(wxCloseEvent& event);
    
    void OnRetune(wxCommandEvent& event);
    void OnChangeSamplePointers(wxCommandEvent& event);
    void OnReplaceKick(wxCommandEvent& event);
    
    void OnAbout(wxCommandEvent& event);
    
    void disableMenuItems();
    void enableMenuItems();
    
    int getBaseOffset(wxUint8 *htdata);
    wxInt16 getFreeMem();
    int getLUToffset(char statev, wxFileOffset filesize);
    void readLUT(int fileoffset);
    void populateEmptySList();
    void populateDirList(wxString currentDir);
    void clearSList();
    unsigned getBaseDiff(int model, int baseOffset);
    void writeChecksum();
    
    void saveHTFile();
    
    void OnListItemActivated(wxListEvent& event);
    void OnDirListDrag(wxListEvent& event);
    
    void OnStateListDrag(wxListEvent& event);
    
    bool isEmptyStateAvailable();
    bool insertState(wxString currentStateDoc);
    bool exportState(wxString currentStateDoc, wxInt16 i);
    

    
    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_ExtractState = 5,
    ID_InsertState = 6,
    ID_DeleteState = 7,
    ID_ExportAsm = 8,
    ID_Retune = 10,
    ID_ChangeSamplePointers = 11,
    ID_ReplaceKick = 12,
    ID_DirList = 13,
    ID_StateList = 14

};

wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)
    EVT_MENU(wxID_OPEN,		mainFrame::OnOpenHT)
    EVT_MENU(wxID_SAVE,		mainFrame::OnSaveHT)
    EVT_MENU(wxID_SAVEAS,	mainFrame::OnSaveAsHT)
    EVT_MENU(wxID_CLOSE,	mainFrame::OnCloseHT)
    EVT_MENU(ID_ExtractState,	mainFrame::OnExtractState)
    EVT_MENU(wxID_ADD,		mainFrame::OnInsertState)
    EVT_MENU(wxID_REMOVE,	mainFrame::OnDeleteState)
    EVT_MENU(ID_ExportAsm,	mainFrame::OnExportAsm)
    EVT_MENU(wxID_EXIT,		mainFrame::OnExit)
    
    EVT_MENU(ID_Retune,		mainFrame::OnRetune)
    EVT_MENU(ID_ChangeSamplePointers, mainFrame::OnChangeSamplePointers)
    EVT_MENU(ID_ReplaceKick,	mainFrame::OnReplaceKick)
    
    EVT_MENU(wxID_ABOUT,	mainFrame::OnAbout)
    
    EVT_LIST_ITEM_ACTIVATED(ID_DirList, mainFrame::OnListItemActivated)
    EVT_LIST_BEGIN_DRAG(ID_DirList, mainFrame::OnDirListDrag)
    
    EVT_LIST_BEGIN_DRAG(ID_StateList, mainFrame::OnStateListDrag)
    
    EVT_CLOSE(mainFrame::XExit)
    
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(ht2UtilGUI);

bool ht2UtilGUI::OnInit()
{
    mainFrame *frame = new mainFrame( "ht2util", wxPoint(50, 50), wxSize(640, 480) );
    frame->Show( true );
    return true;
}
