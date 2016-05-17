
class ht2UtilGUI: public wxApp
{
public:
    virtual bool OnInit();
};

class mainFrame: public wxFrame
{
public:
    mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    wxStaticText *htFileInfo;
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
    wxInt16 noDirs;
    wxInt16 noFiles;
    bool dotdot;
    wxString currentFBDir;			//current file browser path

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
    
    void OnRetune(wxCommandEvent& event);
    void OnChangeSamplePointers(wxCommandEvent& event);
    void OnReplaceKick(wxCommandEvent& event);
    
    void OnAbout(wxCommandEvent& event);
    
    int getBaseOffset(wxUint8 *htdata);
    int getLUToffset(char statev, wxFileOffset filesize);
    void readLUT(int fileoffset);
    void populateEmptySList();
    void populateDirList(wxString currentDir);
    void clearSList();
    unsigned getBaseDiff(int model, int baseOffset);
    void writeChecksum();
    
    void saveHTFile();
    
    void OnListItemActivated(wxListEvent& event);
    

    
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
    ID_DirList = 13

};

wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)
    EVT_MENU(wxID_OPEN,		mainFrame::OnOpenHT)
    EVT_MENU(wxID_SAVE,		mainFrame::OnSaveHT)
    EVT_MENU(wxID_SAVEAS,	mainFrame::OnSaveAsHT)
    EVT_MENU(wxID_CLOSE,	mainFrame::OnCloseHT)
    EVT_MENU(ID_ExtractState,	mainFrame::OnExtractState)
    EVT_MENU(ID_InsertState,	mainFrame::OnInsertState)
    EVT_MENU(ID_DeleteState,	mainFrame::OnDeleteState)
    EVT_MENU(ID_ExportAsm,	mainFrame::OnExportAsm)
    EVT_MENU(wxID_EXIT,		mainFrame::OnExit)
    
    EVT_MENU(ID_Retune,		mainFrame::OnRetune)
    EVT_MENU(ID_ChangeSamplePointers, mainFrame::OnChangeSamplePointers)
    EVT_MENU(ID_ReplaceKick,	mainFrame::OnReplaceKick)
    
    EVT_MENU(wxID_ABOUT,	mainFrame::OnAbout)
    
    EVT_LIST_ITEM_ACTIVATED(ID_DirList, mainFrame::OnListItemActivated)
    
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(ht2UtilGUI);

bool ht2UtilGUI::OnInit()
{
    mainFrame *frame = new mainFrame( "ht2util", wxPoint(50, 50), wxSize(640, 480) );
    frame->Show( true );
    return true;
}
