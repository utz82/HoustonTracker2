//ht2util-gui - ht2 savestate manager utility by utz 2015-16
//version 0.0.2

//done: add checksum recalculations on all ops
//done: add info about free mem / state sizes in files
//TODO: add ht version to savestate format, so we can check for necessary upgrade and incompatibility!!!


#include <wx/wxprec.h>		//use precompiled wx headers unless compiler does not support precompilation
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/listctrl.h>
#include <wx/dnd.h>
#include <wx/artprov.h>
#include <wx/imaglist.h>
#include <algorithm>

#define MAX_SUPPORTED_SAVESTATE_VERSION 1	//latest supported savestate version
#ifndef __WINDOWS__
	#define SEPERATOR "/"
#else
	#define SEPERATOR "\\"
#endif

#include "ht2util-gui.h"

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	htdata = NULL;
	stateData = NULL;
	dirList = NULL;
	fileList = NULL;
	fileSizeList = NULL;
	unsavedChanges = false;

	wxPanel *mainPanel = new wxPanel(this, -1);

	wxBoxSizer *all = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *infoBox = new wxBoxSizer(wxVERTICAL);
	wxGridSizer *mainBox = new wxGridSizer(2,5,5);		//was (1,2,5,5)
	wxBoxSizer *leftSide = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *rightSide = new wxBoxSizer(wxVERTICAL);

	//main menu
	menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Open HT2 file...", "Open HT2 executable to be modified");
	menuFile->Append(wxID_SAVE, "&Save HT2 file", "Save the current HT2 executable");
	menuFile->Append(wxID_SAVEAS, "&Save HT2 file as...", "Save HT2 executable with a new name");
	menuFile->Append(wxID_CLOSE, "&Close HT2 file", "Close the current HT2 executable");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_ADD, "&Insert savestate...\tIns", "Insert a savestate into the current HT2 executable");
	menuFile->Append(wxID_REMOVE, "&Delete savestate\tDel", "Delete a savestate from the current HT2 executable");
	menuFile->Append(ID_ExtractState, "&Extract savestate...\tCtrl-E", "Extract a savestate and save to file");
	menuFile->Append(ID_ExportAsm, "&Export .asm...", "Decompress and disassemble a savestate, and export as .asm");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenu *menuTools = new wxMenu;
	menuTools->Append(ID_Retune, "&Retune...", "Modify the gloabl tuning table");
	menuTools->Append(ID_ChangeSamplePointers, "&Change Sample Pointers...", "Change the standard sample pointers");
	menuTools->Append(ID_ReplaceKick, "&Replace Kick...", "Replace the standard kick sample");
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );
//	menuBar->Append( menuTools, "&Tools" );		//TODO disabled for now as functionality is not yet implemented
	menuBar->Append( menuHelp, "&Help" );
	SetMenuBar( menuBar ); 
	disableMenuItems();
    
	//main window
	htFileInfo = new wxStaticText(mainPanel, -1, wxT("model:\nHT2 version:\nsavestate version:"), wxPoint(-1, -1));
	htSizeInfo = new wxStaticText(mainPanel, -1, wxT("mem free:"), wxPoint(-1, -1));
	savestateList = new wxListCtrl(mainPanel, ID_StateList, wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT);
	directoryList = new wxListCtrl(mainPanel, ID_DirList, wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT);


	//construct savestate list view	
	wxListItem itemCol;
	itemCol.SetText(wxT("slot"));
	savestateList->InsertColumn(0, itemCol);
	savestateList->SetColumnWidth(0, wxLIST_AUTOSIZE );

	itemCol.SetText(wxT("begin"));
	savestateList->InsertColumn(1, itemCol);
	savestateList->SetColumnWidth(1, wxLIST_AUTOSIZE );

	itemCol.SetText(wxT("end"));
	savestateList->InsertColumn(2, itemCol);
	savestateList->SetColumnWidth(2, wxLIST_AUTOSIZE );

	itemCol.SetText(wxT("length"));
	itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
	savestateList->InsertColumn(3, itemCol);
	savestateList->SetColumnWidth(3, wxLIST_AUTOSIZE );
	
	stateDropTarget *mdt = new stateDropTarget(savestateList);
	savestateList->SetDropTarget(mdt);
	
	populateEmptySList();	
	
	//construct image list
	const wxSize iconSize = wxSize(24,24);
	const wxIcon folderIcon = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, iconSize);
	const wxIcon fileIcon = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
	fbIcons = new wxImageList(24, 24, false, 0);
	fbIcons->Add(folderIcon);
	fbIcons->Add(fileIcon);
	
	//construct directory listing
	wxListItem dirListCol;	
	directoryList->AssignImageList(fbIcons, wxIMAGE_LIST_SMALL);
	
	dirListCol.SetText(wxT(""));
	itemCol.SetImage(-1);
	directoryList->InsertColumn(0, dirListCol);
	directoryList->SetColumnWidth(0, wxLIST_AUTOSIZE );
	
	dirListCol.SetText(wxT("name"));
	directoryList->InsertColumn(1, dirListCol);
	directoryList->SetColumnWidth(1, wxLIST_AUTOSIZE );
	
	dirListCol.SetText(wxT("size"));
	dirListCol.SetAlign(wxLIST_FORMAT_RIGHT);
	directoryList->InsertColumn(2, dirListCol);
	directoryList->SetColumnWidth(2, wxLIST_AUTOSIZE );
	
	currentFBDir = wxGetCwd();
	populateDirList(currentFBDir);
	
	exportDropTarget *mdtx = new exportDropTarget(directoryList);
	directoryList->SetDropTarget(mdtx);
	
	
	//construct main layout
	all->Add(new wxPanel(mainPanel, -1));
	
	all->Add(infoBox, 0, wxALIGN_LEFT | wxALL, 10);
		infoBox->Add(htFileInfo);
		infoBox->Add(htSizeInfo);
		
	all->Add(mainBox, 1, wxEXPAND | wxALL, 10);
	
	mainBox->Add(leftSide, 1, wxEXPAND);
		leftSide->Add(savestateList,1,wxEXPAND);
			
	mainBox->Add(rightSide, 1, wxEXPAND);
		rightSide->Add(directoryList,1,wxEXPAND);
	
	mainPanel->SetSizer(all);
	
	
	//status bar
	CreateStatusBar();
	SetStatusText( "" );
}

void mainFrame::OnExit(wxCommandEvent& WXUNUSED(event)) {

	Close(true);
}

void mainFrame::XExit(wxCloseEvent& event) {

	if (unsavedChanges) {
	
		wxMessageDialog *unsavedChgMsg = new wxMessageDialog(NULL, wxT("Save changes?"), wxT("Question"), 
			wxCANCEL | wxYES_NO | wxCANCEL_DEFAULT | wxICON_QUESTION);

		wxInt16 response = unsavedChgMsg->ShowModal();
		if (response == wxID_CANCEL) return;
		else if (response == wxID_YES) saveHTFile();
	}
	
	event.Skip();

}

void mainFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageBox( "htutil v0.1\n\nHoustonTracker 2 savestate manager\nby utz 2015-2016",
                  "About htutil", wxOK | wxICON_INFORMATION );
}

void mainFrame::OnOpenHT(wxCommandEvent& WXUNUSED(event)) {

	//check if a file is currently opened and has unsaved changes
	if (unsavedChanges) {

		wxMessageDialog *unsavedChgMsg = new wxMessageDialog(NULL, wxT("Save changes?"), wxT("Question"), 
			wxCANCEL | wxYES_NO | wxCANCEL_DEFAULT | wxICON_QUESTION);

		wxInt16 response = unsavedChgMsg->ShowModal();
		if (response == wxID_CANCEL) return;
		else if (response == wxID_YES) saveHTFile();			
	}
	


	wxFileDialog *OpenDialog = new wxFileDialog(
		this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
		_("HT2 executable (*.82p, *.83p, *.8xp)|*.82p;*.83p;*.8xp"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition);
 
	if (OpenDialog->ShowModal() == wxID_OK) {	//unless user clicked "cancel"
	
		CurrentDocPath = OpenDialog->GetPath();
		
		//read ht2 file
		wxFile htfile(CurrentDocPath);
		if (!htfile.IsOpened()) {
			wxMessageDialog error1(NULL, wxT("Error: File could not be opened."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error1.ShowModal();
			return;
		}
		
		htsize = htfile.Length();
		if (htsize == wxInvalidOffset) {
			wxMessageDialog error2(NULL, wxT("Error: File is corrupt."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error2.ShowModal();
			return;
		}
		
		delete[] htdata;
		htdata = new wxUint8[htsize];
		
		if (htfile.Read(htdata, (size_t) htsize) != htsize) {
			wxMessageDialog error3(NULL, wxT("Error: File could not be read."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error3.ShowModal();
		
			delete[] htdata;
			htdata = NULL;
			return;
		}

		htfile.Close();
		
		CurrentFileName = OpenDialog->GetFilename();
		
		size_t dot = CurrentFileName.find_last_of(".");			//get file extension
			if (dot != std::string::npos) {
			fileExt = CurrentFileName.substr(dot, CurrentFileName.size() - dot);
		}
		
		const wxString calcversion[3] = { "TI-82", "TI-83", "TI-83 Plus/TI-84 Plus" };
		if (fileExt == ".82p" || fileExt == ".82P") tmodel = 0;
		if (fileExt == ".83p" || fileExt == ".83P") tmodel = 1;
		if (fileExt == ".8xp" || fileExt == ".8XP" || fileExt == ".8xP" || fileExt == ".8Xp") tmodel = 2;
		
		legacyFileEnd = false;
		
		//read savestate version
		if (tmodel == 0 || htdata[htsize-3] != 0) {
			statever = htdata[htsize-4];	//detect legacy HT2 version: if val at offset -3 is 0, it's a legacy binary			
		} else {
			statever = htdata[htsize-6];
			legacyFileEnd = true;
		}
		if (statever > MAX_SUPPORTED_SAVESTATE_VERSION) {
			wxMessageDialog error4(NULL, wxT("Warning: File is of a newer version than supported by this version of ht2util.\nSome functionality may not perform as expected."),
			wxT("Warning"), wxOK_DEFAULT|wxICON_INFORMATION);
			error4.ShowModal();
		}
		
			
		baseOffset = getBaseOffset(htdata);					//determine base offset
		if (baseOffset == 0xffff) {
			wxMessageDialog error5(NULL, wxT("Error: Not a valid HoustonTracker 2 file."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error5.ShowModal();
		
			delete[] htdata;
			htdata = NULL;
			return;
		}
		
		baseDiff = getBaseDiff(tmodel, baseOffset);	
				
		char htverh = htdata[baseOffset] - 0x30;				//determine HT2 version
		char htverl = htdata[baseOffset+1] - 0x30;
		htver = htverh * 10 + htverl;
		wxString htVerStr = wxString::Format(wxT("%i"),htver);

		wxString stateVerStr = wxString::Format(wxT("%i"),statever);
		
		fOffset = getLUToffset(statever, htsize);
	
		if (fOffset == -1) {
			wxMessageDialog error6(NULL, wxT("Error: Savestate lookup table not found."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error6.ShowModal();
		
			delete[] htdata;
			htdata = NULL;
			return;
		}
		lutOffset = static_cast<unsigned>(fOffset) + 1;
		
		readLUT(lutOffset);


		wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath);
		htFileInfo->SetLabel("model: " + calcversion[tmodel] + "\nHT2 version: 2." + htVerStr + "\nsavestate version: " + stateVerStr);
		wxString freeMem = wxString::Format("%i", getFreeMem());
		htSizeInfo->SetLabel("free mem: " + freeMem + " bytes");
		
	}
	unsavedChanges = false;
	enableMenuItems();
	return;
}

void mainFrame::OnCloseHT(wxCommandEvent& WXUNUSED(event)) {

	if (htdata) {
		if (unsavedChanges) {

			wxMessageDialog *unsavedChgMsg = new wxMessageDialog(NULL, wxT("Save changes?"), wxT("Question"), 
				wxCANCEL | wxYES_NO | wxCANCEL_DEFAULT | wxICON_QUESTION);

			wxInt16 response = unsavedChgMsg->ShowModal();
			if (response == wxID_CANCEL) return;
			else if (response == wxID_YES) saveHTFile();
		}

		wxTopLevelWindow::SetTitle("ht2util");
		htFileInfo->SetLabel("model:\nHT2 version:\nsavestate version:");
		htSizeInfo->SetLabel("mem free:");
		delete[] htdata;
		htdata = NULL;
		clearSList();
		unsavedChanges = false;
		disableMenuItems();
	}
	return;
}

void mainFrame::OnSaveHT(wxCommandEvent& WXUNUSED(event)) {

	if (htdata) saveHTFile();
	return;	
}

void mainFrame::OnSaveAsHT(wxCommandEvent& WXUNUSED(event)) {

	if (htdata) {
		wxString filetype = "HT2 executable (*" + fileExt + ")|*" + fileExt;
		wxString suggestedFileName = "[untitled]" + fileExt;
		wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save file as?"), wxEmptyString, suggestedFileName,
			filetype, wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	 	if (SaveDialog->ShowModal() == wxID_OK) {

	 		CurrentDocPath = SaveDialog->GetPath();
			CurrentFileName = SaveDialog->GetFilename();

			saveHTFile();
	 	}
		
		SaveDialog->Destroy();
		
	}
	return;
}

//extract a savestate and export to file
void mainFrame::OnExtractState(wxCommandEvent& WXUNUSED(event)) {

	//check if user has loaded a HT2 .8*p
	if (!htdata) {
		wxMessageDialog error(NULL, wxT("Error: No HT2 executable loaded."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error.ShowModal();
		return;
	}
	
	wxString suggestedFileName;
	wxDateTime dt;
	wxString now;

	for (long i=0; i<8; i++) {

		if (savestateList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {		//woooo... using & instead of == to check flags
		
			//now = wxNow();
			dt = wxDateTime::Now();
			now = dt.Format(wxT("-%y-%b-%d_%H-%M-%S"));
		
			suggestedFileName = CurrentFileName + "-slot" + wxString::Format("%d",static_cast<wxInt16>(i)) + "-" + now + ".ht2s";
			
			wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save state as?"), wxEmptyString, suggestedFileName,
				_("HT2 savestate (*.ht2s)|*.ht2s"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

			if (SaveDialog->ShowModal() == wxID_OK) {
			
				if (statelen[i] != 0) {		//unless savestate is empty
			
					currentStateDoc = SaveDialog->GetPath();
				
					exportState(currentStateDoc, i);
					
					directoryList->DeleteAllItems();
					populateDirList(currentFBDir);
					
				}
			}
			
			SaveDialog->Destroy();			
		}
	}

	return;
}

//load a savestate from file and insert it
void mainFrame::OnInsertState(wxCommandEvent& WXUNUSED(event)) {

	//check if user has loaded a HT2 .8*p
	if (!htdata) {
		wxMessageDialog error(NULL, wxT("Error: No HT2 executable loaded."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error.ShowModal();
		return;
	}

	//check if there are empty save slots available
	if (!isEmptyStateAvailable()) return;

	//initiate file dialog
	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
		_("HT2 savestate (*.ht2s)|*.ht2s"), wxFD_OPEN, wxDefaultPosition);
 
	//unless user clicked "cancel"
	if (OpenDialog->ShowModal() == wxID_OK) {
	
		currentStateDoc = OpenDialog->GetPath();
		
		if (!insertState(currentStateDoc)) return;
		
	}
	
	return;
}

void mainFrame::OnDeleteState(wxCommandEvent& WXUNUSED(event)) {
	
	//check if user has loaded a HT2 .8*p
	if (!htdata) {
		wxMessageDialog error(NULL, wxT("Error: No HT2 executable loaded."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error.ShowModal();
		return;
	}
	
	for (long i=0; i<8; i++) {

		if (savestateList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {
		
			//calculate new savestate lookup table
			wxInt16 j = 0;
			wxUint16 limit = statebeg[i] + statelen[i];
			wxUint16 newLUT[16];
	
			while (j <= 14) {
	
				if (static_cast<wxInt16>(j/2) == i) {
					newLUT[j] = 0;
					newLUT[j+1] = 0;
				} else {
					if (statebeg[static_cast<wxInt16>(j/2)] > limit) {
						newLUT[j] = statebeg[static_cast<wxInt16>(j/2)] - statelen[i] - 1;
						newLUT[j+1] = statebeg[static_cast<wxInt16>(j/2)] + statelen[static_cast<wxInt16>(j/2)] - statelen[i] - 1;
					} else {
						newLUT[j] = statebeg[static_cast<wxInt16>(j/2)];
						newLUT[j+1] = statebeg[static_cast<wxInt16>(j/2)] + statelen[static_cast<wxInt16>(j/2)];
					}
				}
				j += 2;
			}
			
			//buffer those savestates that need to be moved
			wxInt16 fileoffset = statebeg[i] + statelen[i] - baseDiff + 1;
			wxInt16 statesize = htsize - fileoffset;
	
			wxUint8 buffer[statesize];
	
			for (j = 0; j < statesize; j++) {
				buffer[j] = htdata[fileoffset];
				fileoffset++;	
			}

			//write new savestate lookup table
			fileoffset = lutOffset;
	
			for (j = 0; j < 8; j++) {
	
				htdata[fileoffset] = static_cast<wxUint8>(newLUT[j*2] & 0xff);
				htdata[fileoffset+1] = static_cast<wxUint8>((newLUT[j*2]/256) & 0xff);
				htdata[fileoffset+2] = static_cast<wxUint8>(newLUT[(j*2)+1] & 0xff);
				htdata[fileoffset+3] = static_cast<wxUint8>((newLUT[(j*2)+1]/256) & 0xff);		
				fileoffset += 4;	
			}
				
			//move data after the savestate to be deleted down in memory, replace remaining mem with zeroes	
			fileoffset = statebeg[i] - baseDiff;
			wxInt16 length;
	
			//if (legacyFileEnd) length = statesize - 6;
			if (!htdata[htsize-3]) length = statesize - 6;
			else length = statesize - 4;

			for (j = 0; j < length; j++) {
				htdata[fileoffset] = buffer[j];
				fileoffset++;
			}
	
			//fill rest of savestate section with nullbytes
			for (j = 0; j < static_cast<wxInt16>(statelen[i]); j++) {
				htdata[fileoffset] = 0;
				fileoffset++;
			}
			
			readLUT(lutOffset);
		}
	}
	
	writeChecksum();
	wxString freeMem = wxString::Format("%i", getFreeMem());
	htSizeInfo->SetLabel("free mem: " + freeMem + " bytes");
	unsavedChanges = true;
	wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath + " [modified]");
	return;
}


void mainFrame::OnExportAsm(wxCommandEvent& WXUNUSED(event)) {
	
	//check if user has loaded a HT2 .8*p
	if (!htdata) {
		wxMessageDialog error(NULL, wxT("Error: No HT2 executable loaded."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error.ShowModal();
		return;
	}
	
	wxString now,suggestedFileName;
	wxDateTime dt;
	
	
	for (long i=0; i<8; i++) {

		if (savestateList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {
		
			//now = wxNow();
			dt = wxDateTime::Now();
			now = dt.Format(wxT("-%y-%b-%d_%H-%M-%S"));
		
			suggestedFileName = CurrentFileName + "-slot" + wxString::Format("%d",static_cast<wxInt16>(i)) + "-" + now + ".asm";
			
			wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save state as?"), wxEmptyString, suggestedFileName,
				_("assembler source (*.asm)|*.asm"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

			if (SaveDialog->ShowModal() == wxID_OK) {
			
				currentAsmDoc = SaveDialog->GetPath();
				
				wxFile asmFile;
				if (!asmFile.Open(currentAsmDoc, wxFile::write)) {
					wxMessageDialog error1(NULL, wxT("Error: Could not save file."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
					error1.ShowModal();
					return;
				}
				
				wxFileOffset fileoffset = statebeg[i] - baseDiff;
				
				asmFile.Write(";HT version 2." + wxString::Format("%i", htver) + "\n;savestate version " + wxString::Format("%d", statever));
				asmFile.Write("\n\nspeed\n\tdb #" + wxString::Format("%x", htdata[fileoffset]));
				asmFile.Write("\n\nusrDrum\n\tdw #" + wxString::Format("%x", htdata[fileoffset+2]) + wxString::Format("%x", htdata[fileoffset+1]));
				asmFile.Write("\n\nlooprow\n\tdb #" + wxString::Format("%x", htdata[fileoffset+3]));
				asmFile.Write("\n\nptns");
				
				fileoffset += 4;
			
			
 				//decrunch pattern sequence
 				wxUint16 l = 0;

				do {
					if (l > statelen[i]) {			//trap broken savestates so we don't accidentally loop forever
						wxMessageDialog error2(NULL, wxT("Error: Savestate is corrupt."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
						error2.ShowModal();
						return;
					}
					if (!(l & 3)) asmFile.Write("\n\tdb ");		//create a new line every 4 bytes
					asmFile.Write("#" + wxString::Format("%x", htdata[fileoffset]));
					if (((l & 3) != 3) && (htdata[fileoffset] != 0xff)) asmFile.Write(", ");
					l++;
					fileoffset++;
				} while (htdata[fileoffset] != 0xff);

				if (l != 1025) asmFile.Write("\n\tds " + wxString::Format("%i", 1025-l) + ",#ff");		//fill bytes not stored in compressed state

 				//decrunch patterns
 				asmFile.Write("\n\nptn00");
 				wxUint16 j,k;
 				l = 0;
				
				do {

					if ((!(l & 15)) && (htdata[fileoffset] < 0xe0)) asmFile.Write("\n\tdb ");		//create a new line every 16 bytes
					if (htdata[fileoffset] < 0xd0) {
						asmFile.Write("#" + wxString::Format("%x", htdata[fileoffset]));
						if (((l & 15) != 15) && (htdata[fileoffset] != 0xff)) asmFile.Write(", ");
						l++;
		
					}
					if ((htdata[fileoffset] >= 0xd0) && (htdata[fileoffset] < 0xe0)) {
						j = htdata[fileoffset] - 0xcf;
						for (k = 0; k < j; k++) {
							l++;
							asmFile.Write("#0");
							if (((l & 15) != 15) && (htdata[fileoffset] != 0xff)) asmFile.Write(", ");
						}	
					}
					if ((htdata[fileoffset] >= 0xe0) && (htdata[fileoffset] < 0xff)) {
						j = (htdata[fileoffset] - 0xdf) * 16;
						asmFile.Write("\n\tds " + wxString::Format("%i", j) + "\n\t");
						l += j;
					}
	
					fileoffset++;
	
				} while (htdata[fileoffset] != 0xff);		//TODO: checking fileoffset-1 in original ht2util, verify that this actually works!

				fileoffset++;
				l++;
				if (2048 - l != 0) asmFile.Write("\n\tds " + wxString::Format("%i", 2049 - l) + "\n\n");

 				//decrunch fx patterns
 				asmFile.Write("fxptn00\n");

				wxUint8 ctrlb = htdata[fileoffset];
 				l = 0;

 				if (ctrlb < 0xff) {
 					do {

						ctrlb = htdata[fileoffset];
 						fileoffset++;

						if (ctrlb == l) {

							asmFile.Write("fxptn" + wxString::Format("%x", ctrlb) + "\tdb ");
							for (j = 0; j < 32; j++) {

								asmFile.Write("#" + wxString::Format("%x", htdata[fileoffset]));
								fileoffset++;
								if (j != 31) asmFile.Write(",");
							}
							l++;
						}
						else {
							for (; (ctrlb & 0x3f) > l; l++) {

								asmFile.Write("fxptn" + wxString::Format("%x", l) + "\tds 32\n");
							}
							//OUTFILE << "fxptn" << +(ibyte2 & 0x3f) << "\tdb ";
							asmFile.Write("fxptn" + wxString::Format("%x", ctrlb & 0x3f) + "\tdb ");
							for (j = 0; j < 32; j++) {

								asmFile.Write("#" + wxString::Format("%x", htdata[fileoffset]));
								fileoffset++;
								if (j != 31) asmFile.Write(",");
							}
							l++;
						}
//
 					} while (ctrlb < 0x40);
				} else {
					asmFile.Write("\tds 2048");		//insert 2048 zerobytes if no fx patterns are found
				}

				asmFile.Close();
				SaveDialog->Destroy();
			}
		}	
	}


	return;
}


void mainFrame::OnRetune(wxCommandEvent& WXUNUSED(event)) {
	wxMessageDialog error(NULL, wxT("This feature is not implemented yet"), wxT("Info"), wxOK_DEFAULT|wxICON_ERROR);
	error.ShowModal();
}
void mainFrame::OnChangeSamplePointers(wxCommandEvent& WXUNUSED(event)) {
	wxMessageDialog error(NULL, wxT("This feature is not implemented yet"), wxT("Info"), wxOK_DEFAULT|wxICON_ERROR);
	error.ShowModal();
}
void mainFrame::OnReplaceKick(wxCommandEvent& WXUNUSED(event)) {
	wxMessageDialog error(NULL, wxT("This feature is not implemented yet"), wxT("Info"), wxOK_DEFAULT|wxICON_ERROR);
	error.ShowModal();
}

//determine base offset by header length
//returns the first file position after the internal file name
int mainFrame::getBaseOffset(wxUint8 *htdata) {
	const char vstr[5] = { 0x48, 0x54, 0x20, 0x32, 0x2e };	//"HT 2."
	int vno = 0;
	int fileoffset = 0x40;
	bool foundPrgmHeader = false;

	while ((!foundPrgmHeader) && (fileoffset < 0x80)) {
		fileoffset++;

		if (htdata[fileoffset] == vstr[vno]) vno++;
		else vno = 0;
		
		if (vno == 5) foundPrgmHeader = true;
	}
	
	if (!foundPrgmHeader) return 0xffff;
	fileoffset++;
	return fileoffset;
}

//determine savestate LUT offset
int mainFrame::getLUToffset(char statev, wxFileOffset filesize) {

	bool foundLUT = false;
	int fileoffset = 0;
	int vno = 0;
	
	if (statev > 0) {			//for savestate version 1+, detect "XSAVE" string
		const char vstr[5] = { 0x58, 0x53, 0x41, 0x56, 0x45 };
	
		while ((!foundLUT) && (fileoffset < static_cast<int>(filesize))) {
			fileoffset++;
		
			if (htdata[fileoffset] == vstr[vno]) vno++;
			else vno = 0;
		
			if (vno == 5) foundLUT = true;

		}
	} else {				//for legacy savestates, use slightly unsafe detection via the kick drum sample location
		const char vstr[49] = { 0x70, 0x70, 0x60, 0x60, 0x50, 0x50, 0x40, 0x40, 0x40, 0x30, 0x30, 0x30, 0x30,
					0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 
					0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x0 };
					
		while ((!foundLUT) && (fileoffset < static_cast<int>(filesize))) {
			fileoffset++;
		
			if (htdata[fileoffset] == vstr[vno]) vno++;
			else vno = 0;
		
			if (vno == 49) {
				foundLUT = true;
				fileoffset+= 5125;	
			}
		}
	}	
	
	if (!foundLUT) fileoffset = -1;
	
	return fileoffset;
}

//read savestate LUT and construct a listing from it
void mainFrame::readLUT(int fileoffset) {

	unsigned stateEnd;
	wxString temp;

	for (int i=0; i<8; i++) {
	
		statebeg[i] = 256*htdata[fileoffset+1] + htdata[fileoffset];
		statelen[i] = 256*htdata[fileoffset+3] + htdata[fileoffset+2] -statebeg[i];
		fileoffset += 4;
		stateEnd = statebeg[i] + statelen[i];
		totalSsize += statelen[i];			//also get total size of all savestates
		
		if (statelen[i] != 0) {
			temp = wxString::Format(wxT("%i"),statebeg[i]);
			savestateList->SetItem(i, 1, temp);
			temp = wxString::Format(wxT("%i"),stateEnd);
			savestateList->SetItem(i, 2, temp);
			temp = wxString::Format(wxT("%i"),statelen[i]);
			savestateList->SetItem(i, 3, temp);
		} else {
			temp = "-----";
			savestateList->SetItem(i, 1, temp);
			savestateList->SetItem(i, 2, temp);
			savestateList->SetItem(i, 3, temp);
		}
		savestateList->SetColumnWidth(1,-1);
		savestateList->SetColumnWidth(2,-1);
		savestateList->SetColumnWidth(3,-1);
		
	}
		
	return;	
}

void mainFrame::populateEmptySList() {

	wxString temp;
	for (int i=0; i<8; i++) {
		
		temp.Printf(wxT("%d"), i);
		savestateList->InsertItem(i, temp, 0);
		savestateList->SetItemData(i, i);
		savestateList->SetItem(i, 0, temp);
		temp.Printf(wxT("-----"));
		savestateList->SetItem(i, 1, temp);
		savestateList->SetItem(i, 2, temp);
		savestateList->SetItem(i, 3, temp);
	}
	return;
}

//get current directory listing and display it
void mainFrame::populateDirList(wxString currentDir) {

	wxDir dir(currentDir);

	if (!dir.IsOpened()) {

		wxMessageDialog error(NULL, wxT("Error: Cannot load directory list."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error.ShowModal();
		return;
	}
	
	
	wxString filename;
	//wxInt16 i = 0;
	bool cont;

	
	//get # of directories
	noDirs = 0;
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
	while (cont) {
		noDirs++;
		cont = dir.GetNext(&filename);
	}
	
	//get # of files
	noFiles = 0;
	cont = dir.GetFirst(&filename, "*.ht2s", wxDIR_FILES);
	while (cont) {
		noFiles++;
		cont = dir.GetNext(&filename);
	}
	
	//get double dot
	dotdot = false;
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DOTDOT);
	if (cont) {
		if (filename == "..") dotdot = true;
		cont = dir.GetNext(&filename);
		if (cont) dotdot = true;

	}
	//if (currentDir == "") dotdot = false;	//TODO: prevent loading beyond root directory
	
	delete[] dirList;
	delete[] fileList;
	delete[] fileSizeList;
	dirList = new wxString[noDirs];
	fileList = new wxString[noFiles];
	fileSizeList = new wxString[noFiles];
	

	noDirs = 0;
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
	while (cont) {
		dirList[noDirs] = filename;
		noDirs++;
		cont = dir.GetNext(&filename);
	}
	if (noDirs) std::sort(dirList, dirList + noDirs);
	
	noFiles = 0;
	wxString fPath;
	cont = dir.GetFirst(&filename, "*.ht2s", wxDIR_FILES);
	while (cont) {
		fileList[noFiles] = filename;
		fPath = currentDir + SEPERATOR + filename;
		
		wxFile sFile(fPath);
		wxInt16 fSize = sFile.Length();
		if (sFile.IsOpened()) {
			fileSizeList[noFiles] = wxString::Format("%i", (fSize - 9));
			sFile.Close();
		}
		else fileSizeList[noFiles] = "broken";
				
		noFiles++;
		cont = dir.GetNext(&filename);
	}
	if (noFiles) std::sort(fileList, fileList + noFiles);
	
	wxInt16 noAllItems = noDirs + noFiles;
	if (dotdot) noAllItems++;
	
	wxInt16 j = 0;
	wxInt16 dd = 0;

	if (dotdot) {
		directoryList->InsertItem(0, "  ", -1);
		directoryList->SetItemData(0, 0);
		//directoryList->SetItem(0, 0, " ");
		directoryList->SetItem(0, 1, "..");
		directoryList->SetItem(j, 2, " ");
		j++;
		dd++;
	}

	for (; j < noAllItems-noFiles; j++) {
		directoryList->InsertItem(j, "", 0);
		directoryList->SetItemData(j, j);
		//directoryList->SetItem(j, 0, "");
		directoryList->SetItem(j, 1, dirList[j-dd]);
		directoryList->SetItem(j, 2, " ");	
	}
	
	for (; j < noAllItems; j++) {
		directoryList->InsertItem(j, "", 1);
		directoryList->SetItemData(j, j);
		//directoryList->SetItem(j, 0, "");
		directoryList->SetItem(j, 1, fileList[j-dd-noDirs]);
		directoryList->SetItem(j, 2, fileSizeList[j-dd-noDirs]);	
	}
	
	

	directoryList->SetColumnWidth(0,-1);
	directoryList->SetColumnWidth(1,-1);
	directoryList->SetColumnWidth(2,-1);
	
	return;
}

//clear savestate list
void mainFrame::clearSList() {

	wxString temp;
	for (int i=0; i<8; i++) {
		
		temp.Printf(wxT("-----"));
		savestateList->SetItem(i, 1, temp);
		savestateList->SetItem(i, 2, temp);
		savestateList->SetItem(i, 3, temp);
	}
	return;
}

//get baseDiff
unsigned mainFrame::getBaseDiff(int model, int baseOffset) {
	const unsigned basediff[3] = { 0x9104, 0x932b, 0x9d99 };	
	unsigned diff = basediff[model] - baseOffset + 5;
	return diff;
}

//recalculate checksum and write it to file
void mainFrame::writeChecksum() {

	long checksum = 0;
	for (int i=55; i < (htsize-2); i++) {
		checksum += htdata[i];
	}
	
	checksum = checksum & 0xffff;
	
	htdata[htsize-2] = static_cast<unsigned char>(checksum & 0xff);
	htdata[htsize-1] = static_cast<unsigned char>(long(checksum/256) & 0xff);
	
	return;
}

void mainFrame::saveHTFile() {
	
	wxFile htfile;
	if (!htfile.Open(CurrentDocPath, wxFile::write)) {
		wxMessageDialog error1(NULL, wxT("Error: Could not save file."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error1.ShowModal();
		return;
	}
	
	htfile.Write(htdata, (size_t) htsize);
	htfile.Close();
	unsavedChanges = false;
	wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath);	
	return;
}

void mainFrame::OnListItemActivated(wxListEvent& event) {

	long itemnr = event.GetIndex();
	
	if (dotdot && (!itemnr)) {
		if (currentFBDir.BeforeLast('/') != "") {
			currentFBDir = currentFBDir.BeforeLast('/');
			directoryList->DeleteAllItems();
			populateDirList(currentFBDir);
		}
		return;
	}

	if ((itemnr >= dotdot) && (itemnr < noDirs + dotdot)) {
	
		currentFBDir = currentFBDir + SEPERATOR + dirList[itemnr - dotdot];
		directoryList->DeleteAllItems();
		populateDirList(currentFBDir);
		return;
	} 

	return;	
}


//get available savestate memory
//TODO: seems calculation is inaccurate, available memory is slightly larger
wxInt16 mainFrame::getFreeMem() {

	//get first free mem address
	unsigned firstFree = lutOffset + baseDiff + 32;
	for (int i = 0; i < 8; i++) {
		if (statebeg[i]+ statelen[i] > firstFree) firstFree = statebeg[i] + statelen[i] + 1;
	}

	wxInt16 freeMem = (htsize - 75) - (firstFree - baseDiff);
	if (legacyFileEnd) freeMem -= 2;

	return freeMem;
}

//check if there are empty save slots available
bool mainFrame::isEmptyStateAvailable() {

	bool emptyStateAvailable = false;
	
	for (int i=0; i<8; i++) {
		if (statelen[i] == 0) emptyStateAvailable = true;
	}
	
	if (!emptyStateAvailable) {
		wxMessageDialog error0(NULL, wxT("Error: No free savestate slots available.\nTry deleting something first."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error0.ShowModal();
	}
	
	return emptyStateAvailable;
}


bool mainFrame::insertState(wxString currentStateDoc) {

	//open state file and perform validity checks
	wxFile ht2s(currentStateDoc);
	if (!ht2s.IsOpened()) {
		wxMessageDialog error1(NULL, wxT("Error: File could not be opened."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error1.ShowModal();
		return false;
	}
	
	stateSize = ht2s.Length();
	if (stateSize == wxInvalidOffset) {
		wxMessageDialog error2(NULL, wxT("Error: File is corrupt."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error2.ShowModal();
		return false;
	}
	
	//read in data and perform more validity checks
	stateData = new wxUint8[stateSize];
	
	if (ht2s.Read(stateData, (size_t) stateSize) != stateSize) {
		wxMessageDialog error3(NULL, wxT("Error: File could not be read."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error3.ShowModal();
	
		delete[] stateData;
		stateData = NULL;
		return false;
	}

	ht2s.Close();
	
	//check if we've got an actual ht2s file
	wxString sHeader = "";
	for (int i=0; i<7; i++) {
		sHeader += stateData[i];
	}
	if (sHeader != "HT2SAVE") {
		wxMessageDialog error4(NULL, wxT("Error: Not a valid HT2 savestate."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error4.ShowModal();
	
		delete[] stateData;
		return false;		
	}
	
	//check version of the ht2s file against savestate version of the ht2 executable
	if (stateData[7] > statever) {
		wxMessageDialog warn2(NULL, wxT("Error: The savestate you are trying to insert is not supported by this version of HT2."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		warn2.ShowModal();
		delete[] stateData;
		stateData = NULL;
		return false;	
	}
	
	
	//check version of the ht2s file again HT2 version
	if (stateData[8] < htver) {
	
		wxMessageDialog warn1(NULL, wxT("Warning: The savestate was extracted from an older version of HT2 than the one you're currently using.\nYou will need to manually adjust some effect commands."), wxT("Warning"), wxOK_DEFAULT|wxICON_WARNING);
 		warn1.ShowModal();
	
	}
	
	if (stateData[8] > htver) {
	
		wxMessageDialog warn1(NULL, wxT("Warning: This savestate was extracted from a newer version of HT2 than the one you're currently using.\nSome settings and effect commands may not work as intended."), wxT("Warning"), wxOK_DEFAULT|wxICON_WARNING);
 		warn1.ShowModal();
	
	}
	
	//get first free mem address
	unsigned firstFree = lutOffset + baseDiff + 32;
	for (int i = 0; i < 8; i++) {
		if (statebeg[i]+ statelen[i] > firstFree) firstFree = statebeg[i] + statelen[i] + 1;
	}

	
	if ((firstFree - baseDiff + stateSize - 9) > (htsize - 77)) {		//-checksum -padding -versionbyte -header (should be 75 on htver>1)
		wxMessageDialog error5(NULL, wxT("Error: Not enough space to insert savestate.\nTry deleting something first."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error5.ShowModal();
		delete[] stateData;
		stateData = NULL;
		return false;
	}
	
	//get first available slot
	int stateno = 0;
	while (statelen[stateno] != 0) {
		stateno++;
	}
	
	//insert savestate
	int writeOffset = firstFree - baseDiff;
	for (int i=9; i<stateSize; i++) {
		htdata[writeOffset] = stateData[i];
		writeOffset++;
	}
	
	//rewrite savestate LUT
	writeOffset = lutOffset + (stateno * 4);
	htdata[writeOffset] = (unsigned char)(firstFree & 0xff);
	htdata[writeOffset+1] = (unsigned char)((firstFree/256) & 0xff);
	htdata[writeOffset+2] = (unsigned char)((firstFree+stateSize-9) & 0xff);
	htdata[writeOffset+3] = (unsigned char)(((firstFree+stateSize-9)/256) & 0xff);
	
	//recalculate checksum
	writeChecksum();
	
	readLUT(lutOffset);
	
	wxString freeMem = wxString::Format("%i", getFreeMem());
	htSizeInfo->SetLabel("free mem: " + freeMem + " bytes");		
// 		wxString statusmsg = "Savestate inserted into slot " + wxString::Format("%d",stateno);
// 		SetStatusText(statusmsg);
	unsavedChanges = true;
	wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath + " [modified]");
	
	delete[] stateData;
	stateData = NULL;
	return true;

}

bool mainFrame::exportState(wxString currentStateDoc, wxInt16 i) {

	int fileoffset;
	wxUint8 *sdata;
	sdata = NULL;

	wxFile statefile;
	if (!statefile.Open(currentStateDoc, wxFile::write)) {
		wxMessageDialog error1(NULL, wxT("Error: Could not save file."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error1.ShowModal();
		return false;
	}
	
	fileoffset = statebeg[i] - baseDiff;
	
	sdata = new wxUint8[statelen[i]+2];
	
	sdata[0] = static_cast<unsigned char>(statever);
	sdata[1] = static_cast<unsigned char>(htver);
	
	for (int j=2; j<int(statelen[i]+2); j++) {
		sdata[j] = htdata[fileoffset];
		fileoffset++;
	}
	
	statefile.Write("HT2SAVE", 7);
	statefile.Write(sdata, (size_t) statelen[i]+2);
	
	statefile.Close();
	delete [] sdata;
	sdata = NULL;
	
	return true;

}

void mainFrame::disableMenuItems() {

	menuFile->Enable(wxID_SAVE, false);
	menuFile->Enable(wxID_SAVEAS, false);
	menuFile->Enable(wxID_CLOSE, false);
	menuFile->Enable(wxID_ADD, false);
	menuFile->Enable(wxID_REMOVE, false);
	menuFile->Enable(ID_ExtractState, false);
	menuFile->Enable(ID_ExportAsm, false);

}

void mainFrame::enableMenuItems() {

	menuFile->Enable(wxID_SAVE, true);
	menuFile->Enable(wxID_SAVEAS, true);
	menuFile->Enable(wxID_CLOSE, true);
	menuFile->Enable(wxID_ADD, true);
	menuFile->Enable(wxID_REMOVE, true);
	menuFile->Enable(ID_ExtractState, true);
	menuFile->Enable(ID_ExportAsm, true);

}


//drag'n'drop handling
stateDropTarget::stateDropTarget(wxListCtrl *owner) {

	m_owner = owner;
}

exportDropTarget::exportDropTarget(wxListCtrl *owner) {

	m_owner = owner;
}

bool stateDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {

	return true;
}

bool exportDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {

	return true;
}

void mainFrame::OnStateListDrag(wxListEvent& event) {

	if (htdata) {			//ignore dnd event if no htfile opened
	
		wxString text = "blabla";
		wxString now;
		wxDateTime dt;
		
		wxTextDataObject tdo(text);
		wxDropSource tds(tdo, savestateList);
		if (tds.DoDragDrop(wxDrag_CopyOnly)) {
		
			
			for (long i=0; i<8; i++) {

				if (savestateList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {
		
					if (statelen[i] != 0) {
					
						//now = wxNow();
						dt = wxDateTime::Now();
						now = dt.Format(wxT("-%y-%b-%d_%H-%M-%S"));
		
						currentStateDoc = currentFBDir + SEPERATOR + CurrentFileName + "-slot" + wxString::Format("%d",static_cast<wxInt16>(i)) + "-" + now + ".ht2s";
						
						if (!exportState(currentStateDoc, i)) return;
						
						directoryList->DeleteAllItems();
						populateDirList(currentFBDir);
										
					}
				}
			}
		}
	}
	
	return;

}


void mainFrame::OnDirListDrag(wxListEvent& event) {

	if (htdata) {			//ignore dnd event if no htfile opened
		long itemnr = event.GetIndex();
	
		if (itemnr >= dotdot + noDirs) {

			wxString text = "blabla";
  
			wxTextDataObject tdo(text);
			wxDropSource tds(tdo, directoryList);
			if (tds.DoDragDrop(wxDrag_CopyOnly)) {
				
				for (long i = itemnr; i < (dotdot + noDirs + noFiles); i++) {

					if (directoryList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {
				
						if (!isEmptyStateAvailable()) return;
						
						currentStateDoc = currentFBDir + SEPERATOR + fileList[itemnr - dotdot - noDirs];
						
						if (!insertState(currentStateDoc)) return;
						
					}	
				}
			}	
		}
	}
	return;
}