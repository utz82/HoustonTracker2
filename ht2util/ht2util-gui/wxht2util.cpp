//ht2util-gui - ht2 savestate manager utility by utz 2015-16
//version 0.0.1

//TODO: add checksum recalculations on all ops


#include <wx/wxprec.h>		//use precompiled wx headers unless compiler does not support precompilation
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/file.h>
#include <wx/listctrl.h>

#include "ht2util-gui.h"

#define MAX_SUPPORTED_SAVESTATE_VERSION 1	//latest supported savestate version

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxPanel *mainPanel = new wxPanel(this, -1);

	wxBoxSizer *all = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *infoBox = new wxBoxSizer(wxVERTICAL);
	wxGridSizer *mainBox = new wxGridSizer(2,5,5);		//was (1,2,5,5)
	wxBoxSizer *leftSide = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *rightSide = new wxBoxSizer(wxVERTICAL);

	//main menu
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Open HT2 file...", "Open HT2 executable to be modified");
	menuFile->Append(wxID_SAVE, "&Save HT2 file", "Save the current HT2 executable");
	menuFile->Append(wxID_SAVEAS, "&Save HT2 file as...", "Save HT2 executable with a new name");
	menuFile->Append(wxID_CLOSE, "&Close HT2 file", "Close the current HT2 executable");
	menuFile->AppendSeparator();
	menuFile->Append(ID_InsertState, "&Insert savestate...\tIns", "Insert a savestate into the current HT2 executable");
	menuFile->Append(ID_ExtractState, "&Extract savestate...\tCtrl-E", "Extract a savestate and save to file");
	menuFile->Append(ID_DeleteState, "&Delete savestate\tDel", "Delete a savestate from the current HT2 executable");
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
	menuBar->Append( menuTools, "&Tools" );
	menuBar->Append( menuHelp, "&Help" );
	SetMenuBar( menuBar ); 
    
	//main window
	//currentFile = new wxStaticText(mainPanel, -1, wxT("file: "), wxPoint(-1, -1));
	htFileInfo = new wxStaticText(mainPanel, -1, wxT("model:\nHT2 version:\nsavestate version:"), wxPoint(-1, -1));
	//savestateList = new wxStaticText(mainPanel, -1, wxT("savestate table"), wxPoint(-1, -1));	//, wxSize(250, 150)
	savestateList = new wxListCtrl(mainPanel, -1, wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT);
	directoryList = new wxStaticText(mainPanel, -1, wxT("directory list"), wxPoint(-1, -1));	//, wxSize(250, 150)
	
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
	savestateList->InsertColumn(3, itemCol);
	savestateList->SetColumnWidth(3, wxLIST_AUTOSIZE );
	
	populateEmptySList();
	
	
	all->Add(new wxPanel(mainPanel, -1));
	
	all->Add(infoBox, 0, wxALIGN_LEFT | wxALL, 10);
		infoBox->Add(htFileInfo);
		
	all->Add(mainBox, 1, wxEXPAND | wxALL, 10);
	
	mainBox->Add(leftSide, 1, wxEXPAND);
		leftSide->Add(savestateList,1,wxEXPAND);
			
	mainBox->Add(rightSide, 1, wxEXPAND);
		rightSide->Add(directoryList);
	
	mainPanel->SetSizer(all);
    
	//status bar
	CreateStatusBar();
	SetStatusText( "" );
}

void mainFrame::OnExit(wxCommandEvent& WXUNUSED(event)) {

	if (unsavedChanges) {
	
		wxMessageDialog *unsavedChgMsg = new wxMessageDialog(NULL, wxT("Save changes?"), wxT("Question"), 
			wxCANCEL | wxYES_NO | wxCANCEL_DEFAULT | wxICON_QUESTION);

		wxInt16 response = unsavedChgMsg->ShowModal();
		if (response == wxID_CANCEL) return;
		else if (response == wxID_YES) saveHTFile();
	}
	
	Close(true);
}

void mainFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageBox( "htutil v0.1\n\nHoustonTracker 2 savestate manager\nby utz 2015-2016",
                  "About htutil", wxOK | wxICON_INFORMATION );
}

void mainFrame::OnOpenHT(wxCommandEvent& WXUNUSED(event)) {

	//TODO: check if a file is currently opened and has unsaved changes

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
		
		htdata = new wxUint8[htsize];
		
		if (htfile.Read(htdata, (size_t) htsize) != htsize) {
			wxMessageDialog error3(NULL, wxT("Error: File could not be read."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error3.ShowModal();
		
			delete[] htdata;
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
			//wxPuts(wxT("using regular file end"));			
		} else {
			statever = htdata[htsize-6];
			legacyFileEnd = true;
			//wxPuts(wxT("using legacy file end"));
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
			return;
		}
		lutOffset = static_cast<unsigned>(fOffset) + 1;
		
		readLUT(lutOffset);
		
// 		wxMultiChoiceDialog sl(this, wxT(""), wxT("Savestate List"), stateList);
// 		sl.ShowModal();
		
		//currentFile->SetLabel("file: " + CurrentFileName);
		wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath);
		htFileInfo->SetLabel("model: " + calcversion[tmodel] + "\nHT2 version: 2." + htVerStr + "\nsavestate version: " + stateVerStr);
		
	}
	unsavedChanges = false;
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

		//currentFile->SetLabel("file:");
		wxTopLevelWindow::SetTitle("ht2util");
		htFileInfo->SetLabel("model:\nHT2 version:\nsavestate version:");
		delete[] htdata;
		clearSList();
		unsavedChanges = false;
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
			
			//add extension if not added by user... this is more complex than thought, because the wx team believes not auto-adding an extension
			//is "default behaviour" under gtk lol
// 			size_t dot = CurrentDocPath.find_last_of(".");			//get file extension
// 			wxString saveFileExt;
// 			if (dot != std::string::npos) saveFileExt = CurrentFileName.substr(dot, CurrentFileName.size() - dot);
// 			if (dot == std::string::npos || saveFileExt != fileExt) {
// 				CurrentDocPath += fileExt;
// 				CurrentFileName += fileExt;
// 			}
			
// 			
// 			wxFile htfile;
// 			if (!htfile.Open(CurrentDocPath, wxFile::write)) {
// 				wxMessageDialog error1(NULL, wxT("Error: Could not save file."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
// 				error1.ShowModal();
// 				return;
// 			}
// 			htfile.Write(htdata, (size_t) htsize);
// 			htfile.Close();
// 			wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath);
// 			unsavedChanges = false;
			saveHTFile();
	 	}

		//currentFile->SetLabel("file: " + CurrentFileName);
		
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
	int fileoffset;
	wxUint8 *sdata;
	wxString now;

	for (long i=0; i<8; i++) {

		if (savestateList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {		//woooo... using & instead of == to check flags
		
			now = wxNow();
		
			suggestedFileName = CurrentFileName + "-slot" + wxString::Format("%d",i) + "-" + now + ".ht2s";
			
			wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save state as?"), wxEmptyString, suggestedFileName,
				_("HT2 savestate (*.ht2s)|*.ht2s"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

			if (SaveDialog->ShowModal() == wxID_OK) {
			
				if (statelen[i] != 0) {		//unless savestate is empty
			
					currentStateDoc = SaveDialog->GetPath();
				
					wxFile statefile;
					if (!statefile.Open(currentStateDoc, wxFile::write)) {
						wxMessageDialog error1(NULL, wxT("Error: Could not save file."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
						error1.ShowModal();
						return;
					}
					
					fileoffset = statebeg[i] - baseDiff;
					
					sdata = new wxUint8[statelen[i]+1];
					
					sdata[0] = static_cast<unsigned char>(statever);
					
					for (int j=1; j<int(statelen[i]+1); j++) {
						sdata[j] = htdata[fileoffset];
						fileoffset++;
					}
					
					statefile.Write("HT2SAVE", 7);
					statefile.Write(sdata, (size_t) statelen[i]+1);
					
					statefile.Close();
					delete [] sdata;
					
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
	bool emptyStateAvailable = false;
	
	for (int i=0; i<8; i++) {
		if (statelen[i] == 0) emptyStateAvailable = true;
	}
	if (!emptyStateAvailable) {
		wxMessageDialog error0(NULL, wxT("Error: No free savestate slots available.\nTry deleting something first."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
		error0.ShowModal();
		return;
	}

	//initiate file dialog
	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
		_("HT2 savestate (*.ht2s)|*.ht2s"), wxFD_OPEN, wxDefaultPosition);
 
	//unless user clicked "cancel"
	if (OpenDialog->ShowModal() == wxID_OK) {
	
		currentStateDoc = OpenDialog->GetPath();
		
		//open state file and perform validity checks
		wxFile ht2s(currentStateDoc);
		if (!ht2s.IsOpened()) {
			wxMessageDialog error1(NULL, wxT("Error: File could not be opened."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error1.ShowModal();
			return;
		}
		
		stateSize = ht2s.Length();
		if (stateSize == wxInvalidOffset) {
			wxMessageDialog error2(NULL, wxT("Error: File is corrupt."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error2.ShowModal();
			return;
		}
		
		//read in data and perform more validity checks
		stateData = new wxUint8[stateSize];
		
		if (ht2s.Read(stateData, (size_t) stateSize) != stateSize) {
			wxMessageDialog error3(NULL, wxT("Error: File could not be read."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error3.ShowModal();
		
			delete[] stateData;
			return;
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
			return;		
		}
		
		//check version of the ht2s file against savestate version of the ht2 executable
		if (stateData[7] < statever) {
			wxMessageDialog warn1(NULL, wxT("Warning: The savestate you are trying to insert is outdated for this version of HT2.\nConsider upgrading the savestate."), wxT("Warning"), wxOK_DEFAULT|wxICON_ERROR);
			warn1.ShowModal();
		}
		if (stateData[7] > statever) {
			wxMessageDialog warn2(NULL, wxT("Error: The savestate you are trying to insert is not supported by this version of HT2."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			warn2.ShowModal();
			delete[] stateData;
			return;	
		}
		
		//get first free mem address
		unsigned firstFree = lutOffset + baseDiff + 32;
		for (int i = 0; i < 8; i++) {
			if (statebeg[i]+ statelen[i] > firstFree) firstFree = statebeg[i] + statelen[i] + 1;
// 			wxString debug = wxString::Format(("%i"),firstFree);
// 			wxPuts(debug);
		}

		
		if ((firstFree - baseDiff + stateSize - 8) > (htsize - 77)) {		//-checksum -padding -versionbyte -header (should be 75 on htver>1)
			wxMessageDialog error5(NULL, wxT("Error: Not enough space to insert savestate.\nTry deleting something first."), wxT("Error"), wxOK_DEFAULT|wxICON_ERROR);
			error5.ShowModal();
			delete[] stateData;
			return;
		}
		
		//get first available slot
		int stateno = 0;
		while (statelen[stateno] != 0) {
			stateno++;
		}
		
		//insert savestate
		int writeOffset = firstFree - baseDiff;
		for (int i=8; i<stateSize; i++) {
			htdata[writeOffset] = stateData[i];
			writeOffset++;
		}
		
		//rewrite savestate LUT
		writeOffset = lutOffset + (stateno * 4);
		htdata[writeOffset] = (unsigned char)(firstFree & 0xff);
		htdata[writeOffset+1] = (unsigned char)((firstFree/256) & 0xff);
		htdata[writeOffset+2] = (unsigned char)((firstFree+stateSize-8) & 0xff);
		htdata[writeOffset+3] = (unsigned char)(((firstFree+stateSize-8)/256) & 0xff);
		
		//recalculate checksum
		writeChecksum();
		
		readLUT(lutOffset);
// 		wxString statusmsg = "Savestate inserted into slot " + wxString::Format("%d",stateno);
// 		SetStatusText(statusmsg);
		unsavedChanges = true;
		wxTopLevelWindow::SetTitle("ht2util - " + CurrentDocPath + " [modified]");			
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
	
	
	for (long i=0; i<8; i++) {

		if (savestateList->GetItemState(i,wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) {
		
			now = wxNow();
		
			suggestedFileName = CurrentFileName + "-slot" + wxString::Format("%d",i) + "-" + now + ".asm";
			
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
			
// 				char readb;
// 				unsigned char ibyte1, ibyte2;
//
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

// 				HTFILE.seekg(fileoffset, ios::beg);
// 				HTFILE.read((&readb), 1);
// 				ibyte2 = static_cast<unsigned char>(readb);
				//TODO: fx patterns aren't recognized/written
				wxUint8 ctrlb = htdata[fileoffset];
 				l = 0;

 				if (ctrlb < 0xff) {
 					do {
// 						HTFILE.seekg(fileoffset, ios::beg);
// 						HTFILE.read((&readb), 1);
// 						ibyte2 = static_cast<unsigned char>(readb);
						ctrlb = htdata[fileoffset];
 						fileoffset++;
// 		
// 						cout << hex << +ibyte2 << endl;
// 		
						if (ctrlb == l) {
							//OUTFILE << "fxptn" << +ibyte2 << "\tdb ";
							asmFile.Write("fxptn" + wxString::Format("%x", ctrlb) + "\tdb ");
							for (j = 0; j < 32; j++) {
// 								HTFILE.seekg(fileoffset, ios::beg);
// 								HTFILE.read((&readb), 1);
// 								ibyte1 = static_cast<unsigned char>(readb);
// 								fileoffset++;
// 								OUTFILE << "#" << hex << +ibyte1;
								asmFile.Write("#" + wxString::Format("%x", htdata[fileoffset]));
								fileoffset++;
								if (j != 31) asmFile.Write(",");
							}
							l++;
						}
						else {
							for (; (ctrlb & 0x3f) > l; l++) {
								//cout << l << "\n";
								//OUTFILE << "fxptn" << l << "\tds 32\n";
								asmFile.Write("fxptn" + wxString::Format("%x", l) + "\tds 32\n");
							}
							//OUTFILE << "fxptn" << +(ibyte2 & 0x3f) << "\tdb ";
							asmFile.Write("fxptn" + wxString::Format("%x", ctrlb & 0x3f) + "\tdb ");
							for (j = 0; j < 32; j++) {
// 								HTFILE.seekg(fileoffset, ios::beg);
// 								HTFILE.read((&readb), 1);
// 								ibyte1 = static_cast<unsigned char>(readb);
// 								fileoffset++;
// 								OUTFILE << "#" << hex << +ibyte1;
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
