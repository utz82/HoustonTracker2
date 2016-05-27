//HoustonTracker 2 Savestate Manager Utility
//by utz 2015-2016

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;


void readLUT(int fileoffset, int htv, char statev);
void writeChecksum();
int getLUToffset(char statev, unsigned filesize);
int insertState(unsigned lutOffset, char statev);
int removeState(unsigned lutOffset, char statev, bool legacyFileEnd);
int extractState(char statev);
int decompressState(char statev);
string getOutfileName(string suffix);
string getInfileName();
int getSavestateNo();
unsigned getHTVersionNo(int fileoffset);
int getBaseOffset();
unsigned getBaseDiff(int model, int baseOffset);

unsigned statebeg[8], statelen[8];
const string calcversion[3] = { "82", "83", "83+/84+" };
unsigned basediff, htver;

fstream HTFILE;

int main(int argc, char *argv[]){

	cout << "HT2 FILE MANAGER UTILITY v0.1\n\n";
	
	
	//check if correct number of arguments has been passed
	if (argc <= 1) {
		cout << "USAGE: ht2util <infile>\n";
		return -1;	
	}	

	
	//open file, and exit if that fails
	string filename = "";
	
	HTFILE.open (argv[1], ios::in | ios::out | ios::binary);
	if (!HTFILE.is_open()) {
		cout << "Error: Could not open " << argv[1] << "\n";
		return -1;
	}
	else {
		filename = argv[1];
		cout << "analyzing " << filename << "...\n\n";	
	}

	//check filesize
	HTFILE.seekg(0,ios_base::end);
	unsigned filesize = HTFILE.tellg();
	
	//extract file extension
	string ext = "";
	
	size_t dot = filename.find_last_of(".");
	if (dot != std::string::npos) {
		ext = filename.substr(dot, filename.size() - dot);
	}
	
	//exit if file is not an 8*p program
	if (ext != ".82p" && ext != ".83p" && ext != ".8xp" && ext != ".82P" && ext != ".83P" && ext != ".8XP" && ext != ".8xP"  && ext != ".8Xp") {
		cout << "Error: not a valid HT2 program file\n";
		HTFILE.close();
		return -1;
	}
	
	//get base offset
	int baseOffset = getBaseOffset();
	if (baseOffset == -1) {
		cout << "Error: Not a valid HT2 program file.\n";
		return -1;
	}
		
	//get HT2 version number
	htver = getHTVersionNo(baseOffset);

	//determine calc type by file extension
	int tmodel;
	if (ext == ".82p" || ext == ".82P") tmodel = 0;
	if (ext == ".83p" || ext == ".83P") tmodel = 1;
	if (ext == ".8xp" || ext == ".8XP" || ext == ".8xP" || ext == ".8Xp") tmodel = 2;
	
	//check if user accidentally dumped an 8xp file as 83p
	if (tmodel == 1) {
		int tempfo = baseOffset - 11;
		char temp1, temp2;
		
		HTFILE.seekg(tempfo, ios::beg);
		HTFILE.read((&temp1), 1);
		tempfo++;
		HTFILE.seekg(tempfo, ios::beg);
		HTFILE.read((&temp2), 1);
		
		if (((temp1&0xff) == 0xbb) && (temp2 == 0x6d)) {	//if 8xp asmPRGM header found
			cout << "Error: TI-8x Plus file dumped as TI-83 file. Redump as .8xp.\n";
			return -1;
		}
	
	}
	
	//determine savestate version
	char statever;
	bool legacyFileEnd = false;
	if (tmodel == 0) {
		HTFILE.seekg(-4, ios::end);		//read savestate version
		HTFILE.read((&statever), 1);
	} else {
		HTFILE.seekg(-3, ios::end);		//detect legacy HT2 version: if val at offset -3 is 0, it's a legacy binary
		HTFILE.read((&statever), 1);
		
		if (statever != 0) {			//read actual savestate version
			HTFILE.seekg(-4, ios::end);
			HTFILE.read((&statever), 1);
		} else {
			HTFILE.seekg(-6, ios::end);
			HTFILE.read((&statever), 1);
			legacyFileEnd = true;
		}
	}	
	if (statever > 1) {
		cout << "Warning: " << filename << " is of a newer version than supported by this version of ht2util.\nSome functionality may not perform as expected.";	
	}
	
	//get basediff
	basediff = getBaseDiff(tmodel, baseOffset);
	
	unsigned lutOffset;
	int fOffset;
	fOffset = getLUToffset(statever, filesize);
	
	if (fOffset == -1) {
		cout << "Error: Savestate lookup table not found.\n";
		return -1;
	}
	lutOffset = static_cast<unsigned>(fOffset) + 1;
	
	cout << "TI version.............." << calcversion[tmodel] << "\n";
	cout << "HT version..............2." << +htver << "\n";
	cout << "savestate version......." << +statever << "\n\n";
	cout << "no.\tbegin\tend\tlength\n";
	

	//handle user actions	
	const string keys[16] = { "u", "U", "t", "T", "e", "E", "d", "D", "i", "I", "r", "R", "q", "Q" };
	const string *end;
	
	string cmd = "";
	
	while (cmd != "q" && cmd != "Q") {
		cout << "\n";
		readLUT(lutOffset, htver, statever);

		cout << "\nWhat would you like to do?\n(E)xtract a savestate\n(D)ecompress and disassemble a savestate\n(I)nsert a savestate\n(R)emove a savestate\n(Q)uit\n";
		cin >> cmd;
		end = find(keys, keys+14, cmd);
		if (end == keys+14) cout << "e|d|i|r|q only, please.\n";
		if (cmd == "e" || cmd == "E") extractState(statever);
		if (cmd == "d" || cmd == "D") decompressState(statever);
		if (cmd == "r" || cmd == "R") removeState(lutOffset, statever, legacyFileEnd);
		if (cmd == "i" || cmd == "I") insertState(lutOffset, statever);
	}
	//TODO: ext. ops: retune freq.tab, change samples
	
	
	//close open file(s) and exit
	HTFILE.close();
	return 0;
}

//get baseDiff
unsigned getBaseDiff(int model, int baseOffset) {
	const unsigned basediff[3] = { 0x9104, 0x932b, 0x9d99 };
	unsigned diff;
	
	diff = basediff[model] - baseOffset + 5;
	return diff;
}

//determine base offset by header length
//returns the first file position after the internal file name
int getBaseOffset() {
	const char vstr[5] = { 0x48, 0x54, 0x20, 0x32, 0x2e };	//"HT 2."
	int vno = 0;
	int fileoffset = 0x40;
	bool foundPrgmHeader = false;
	char temp;

	while ((!foundPrgmHeader) && (fileoffset < 0x80)) {
		fileoffset++;
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read(&temp, 1);
		
		if (temp == vstr[vno]) vno++;
		else vno = 0;
		
		if (vno == 5) foundPrgmHeader = true;
	}
	
	if (!foundPrgmHeader) return 0xffff;
	fileoffset++;
	return fileoffset;
}

//get HT2 version number
unsigned getHTVersionNo(int fileoffset) {
	char htverh, htverl;
	unsigned htver;

	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read(&htverh, 1);
	
	fileoffset++;
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read(&htverl, 1);
		
	htverh = htverh - 0x30;
	htverl = htverl - 0x30;
	htver = htverh * 10 + htverl;
	
	return htver;
}

//determine savestate LUT offset
int getLUToffset(char statev, unsigned filesize) {

	bool foundLUT = false;
	int fileoffset = 0;
	char readb;
	int vno = 0;
	
	if (statev > 0) {			//for savestate version 1+, detect "XSAVE" string
		const char vstr[5] = { 0x58, 0x53, 0x41, 0x56, 0x45 };
	
		while ((!foundLUT) && (fileoffset < static_cast<int>(filesize))) {
			fileoffset++;
			HTFILE.seekg(fileoffset, ios::beg);
			HTFILE.read((&readb), 1);
		
			if (readb == vstr[vno]) vno++;
			else vno = 0;
		
			if (vno == 5) foundLUT = true;

		}
	} else {				//for legacy savestates, use slightly unsafe detection via the kick drum sample location
		const char vstr[49] = { 0x70, 0x70, 0x60, 0x60, 0x50, 0x50, 0x40, 0x40, 0x40, 0x30, 0x30, 0x30, 0x30,
					0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 
					0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x0 };
					
		while ((!foundLUT) && (fileoffset < static_cast<int>(filesize))) {
			fileoffset++;
			HTFILE.seekg(fileoffset, ios::beg);
			HTFILE.read((&readb), 1);
		
			if (readb == vstr[vno]) vno++;
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

//read savestate LUT and print finding to stdout
void readLUT(int fileoffset, int htv, char statev) {

	int i;
	char readb;
	unsigned int send;
	unsigned char startlo, starthi, endlo, endhi;

	for (i=0; i<8; i++) {
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read((&readb), 1);
		startlo = static_cast<unsigned char>(readb);
		fileoffset++;
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read((&readb), 1);
		starthi = static_cast<unsigned char>(readb);
		fileoffset++;
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read((&readb), 1);
		endlo = static_cast<unsigned char>(readb);
		fileoffset++;
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read((&readb), 1);
		endhi = static_cast<unsigned char>(readb);
		fileoffset++;
	
		statebeg[i] = starthi * 256 + startlo;
		send = endhi * 256 + endlo;
		statelen[i] = send - statebeg[i];

		if (statebeg[i] != 0 && send != 0) {
			cout << i << "\t" << statebeg[i] << "\t" << send << "\t" << statelen[i] << "\n";
		}
		else {
			cout << i << "\t-----\t-----\t----\n";
		}
	}
		
	return;	
}


//insert a savestate
int insertState(unsigned int lutOffset, char statev) {

	//check if there are free save slots available
	if (statelen[0] != 0 && statelen[1] != 0 && statelen[2] != 0 && statelen[3] != 0 && statelen[4] != 0 && statelen[5] != 0 && statelen[6] != 0 && statelen[7] != 0) {
		cout << "Error: No free savestates available. Try deleting something first.\n";
		return -1;
	}

	//get filename from user input and exit on file-not-found
	string statefilename = getInfileName();
	if (statefilename == "") return -1;
	
	//open state file
	ifstream STATEFILE;
	STATEFILE.open (statefilename.c_str(), ios::in | ios::binary);
	if (!STATEFILE.is_open()) {
		cout << "Error: Could not open " << statefilename << "\n";
		return -1;
	}
	
	//check header to verify that is is an actual HT2 savestate file
	char buffer[9];
	STATEFILE.seekg(0, ios::beg);
	STATEFILE.read(buffer, 9);
	
	if (buffer[0] != 'H' || buffer[1] != 'T' || buffer[2] != '2' || buffer[3] != 'S' || buffer[4] != 'A' || buffer[5] != 'V' || buffer[6] != 'E') {
		cout << "Error: " << statefilename << " is either corrupt or not a valid HT2 savestate file.\n";
		return -1;
	}
	
	//check savestate version against main ht2 file savestate version
	bool doUpgrade = false;
	
	if (buffer[7] > statev) {
		cout << "Error: This savestate version is not supported by this version of HT2. Consider using a newer version of HT2.\n";
		STATEFILE.close();
		return -1;
	}
// 	if (buffer[7] < statev) {
// 		cout << "Warning: " << statefilename << " is of an older version than the HT2 file you're trying to insert into. You will need to manually adjust the FX commands.\n";
// 		doUpgrade = true;
// 	}
	if (static_cast<unsigned>(buffer[8]) < htver) {
		cout << "Warning: " << statefilename << " was extracted from an older HT2 version than the one you're currently using. You will need to manually adjust the FX commands.\n";
		doUpgrade = true;
	}
	if (static_cast<unsigned>(buffer[8]) > htver) {
		cout << "Warning: " << statefilename << " was extracted from a newer HT2 version than the one you're currently using. Some settings and FX commands will not work as expected.\n";
	}
	
	//get state file size
	STATEFILE.seekg(0, ios::end);
	int statesize = STATEFILE.tellg();
	statesize = statesize - 9;		//don't count header
	
	//get first available slot
	int stateno = 0;
	while (statelen[stateno] != 0) {
		stateno++;
	}
	
	//get first free mem address
	int i;
//	unsigned int firstfree = 0;
	unsigned firstfree = lutOffset + basediff + 32;
	for (i = 0; i < 8; i++) {
		if (statebeg[i] + statelen[i] > firstfree) firstfree = statebeg[i] + statelen[i] + 1;
	}
	
	//check if there is sufficient space to insert the state
	HTFILE.seekg(0, ios::end);
	unsigned int htsize = HTFILE.tellg();
	if ((firstfree - basediff + statesize) > (htsize - 77)) {		//-checksum -padding -versionbyte -header (should be 75 on htver>1)
		cout << "Error: Not enough space to insert the savestate. Try deleting another savestate first.\n";
		return -1;
	}	
	
	//read state into buffer
	char fbuf[statesize];
	STATEFILE.seekg(9, ios::beg);
	STATEFILE.read(fbuf, statesize);
	
	
	//upgrade if necessary
	if (doUpgrade) {
		cout << "We should really upgrade this savestate, but this isn't implemented yet.\n";
	}
	
	//write buffer into HT2 file
	long fileoffset = firstfree - basediff;
	HTFILE.seekp(fileoffset, ios::beg);
	HTFILE.write(fbuf, statesize);
	
	//update savestate LUT
	fileoffset = lutOffset + (stateno * 4);
	char outb = char(firstfree & 0xff);
	HTFILE.seekp(fileoffset, ios::beg);
	HTFILE << outb;
	outb = char((firstfree/256) & 0xff);
	HTFILE << outb;
	outb = char((firstfree + statesize) & 0xff);
	HTFILE << outb;
	outb = char(((firstfree + statesize)/256) & 0xff);
	HTFILE << outb;
		
	//recalculate checksum
	writeChecksum();
	
	cout << "Savestate inserted into slot " << stateno << ".\n";
	STATEFILE.close();
	return 0;
}


//remove a savestate
int removeState(unsigned lutOffset, char statev, bool legacyFileEnd) {

	int stateno = getSavestateNo();
	if (statelen[stateno] == 0) {				//trap empty savestates
		cout << "Error: savestate is already empty.\n";
		return -1;
	}
	
		
	//calculate new savestate lookup table
	int i = 0;
	unsigned int limit = statebeg[stateno] + statelen[stateno];
	unsigned int newLUT[16];
	
	while (i <= 14) {
	
		if (int(i/2) == stateno) {
			newLUT[i] = 0;
			newLUT[i+1] = 0;
		}
		else {
			if (statebeg[int(i/2)] > limit) {
				newLUT[i] = statebeg[int(i/2)] - statelen[stateno] - 1;
				newLUT[i+1] = statebeg[int(i/2)] + statelen[int(i/2)] - statelen[stateno] - 1;
			}
			else {
				newLUT[i] = statebeg[int(i/2)];
				newLUT[i+1] = statebeg[int(i/2)] + statelen[int(i/2)];
			}
		}

		i += 2;
	}
	
	
	//buffer those savestates that need to be moved
	HTFILE.seekg(0, ios::end);
	int filesizefull = HTFILE.tellg();
		
	int fileoffset = statebeg[stateno] + statelen[stateno] - basediff + 1;
	int statesize = filesizefull - fileoffset;
	
	char buffer[statesize];
	
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read(buffer, statesize);

	
	//write new savestate lookup table
	fileoffset = lutOffset;
	char outbytes[4];
	
	for (i = 0; i < 8; i++) {
	
		outbytes[0] = newLUT[i*2] & 0xff;
		outbytes[1] = char(newLUT[i*2]/256) & 0xff;
		outbytes[2] = newLUT[(i*2)+1] & 0xff;
		outbytes[3] = char(newLUT[(i*2)+1]/256) & 0xff;
		
		HTFILE.seekp(fileoffset, ios::beg);
		HTFILE.write(outbytes, 4);
		
		fileoffset += 4;	
	}
	
	
	//move data after the savestate to be deleted down in memory, replace remaining mem with zeroes	
	fileoffset = statebeg[stateno] - basediff;
	int length;
	
// 	if (model == 0 || statev != 1) {	//TODO: this is no longer true, applies to legacy files only
// 		length = statesize - 3;		//2B checksum, 1B savestate version, 2 0-bytes if model != 0 && stateversion == 1)
// 	}
// 	else {
// 		length = statesize - 5;
// 	}
	if (legacyFileEnd) length = statesize - 6;
	else length = statesize - 4;


	HTFILE.seekp(fileoffset, ios::beg);
	HTFILE.write(buffer, length);
	
	char outb = 0;				//fill rest of savestate section with nullbytes
	for (unsigned k = 0; k < statelen[stateno]; k++) {
		HTFILE << outb;
	}

	
	//recalculate checksum and write it
	writeChecksum();

	cout << "Savestate " << stateno << " removed.\n";
	return 0;
}


//decompress and disassemble a savestate
int decompressState(char statev) {

	char readb;
	unsigned char ibyte1, ibyte2;
	
	
	//request user to input a state number and filename
	int stateno = getSavestateNo();
	if (statelen[stateno] == 0) {				//trap empty savestates
		cout << "Error: savestate is empty.\n";
		return -1;
	}
	string outfilename = getOutfileName(".asm");
	if (outfilename == "") return -1;			//back to main menu if user chose to not overwrite existing file
	
	int fileoffset = statebeg[stateno] - basediff;
	
	//create output file
	ofstream OUTFILE;
	OUTFILE.open (outfilename.c_str(), ios::out | ios::trunc);
	
	OUTFILE << ";savestate version " << +statev;
	
	//extract global vars
	OUTFILE << "\nspeed\n\tdb #";
	
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read((&readb), 1);
	ibyte1 = static_cast<unsigned char>(readb);
	
	OUTFILE << hex << +ibyte1 << "\n\nusrDrum\n\tdw #";
	
	fileoffset++;
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read((&readb), 1);
	ibyte1 = static_cast<unsigned char>(readb);
	fileoffset++;
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read((&readb), 1);
	ibyte2 = static_cast<unsigned char>(readb);
	
	OUTFILE << hex << +ibyte2 << hex << +ibyte1 << "\n\nlooprow\n\tdb #";
	
	fileoffset++;
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read((&readb), 1);
	ibyte1 = static_cast<unsigned char>(readb);
	
	OUTFILE << hex << +ibyte1 << "\n\nptns";
	
	//decrunch pattern sequence
	unsigned int i = 0;
	fileoffset++;
	do {
		if (i > statelen[stateno]) {			//trap broken savestates so we don't accidentally loop forever
			cout << "Error: savestate is corrupt or invalid.\n";
			return -1;
		}
		if ((i & 3) == 0) OUTFILE << "\n\tdb ";		//create a new line every 4 bytes
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read((&readb), 1);
		ibyte1 = static_cast<unsigned char>(readb);
		OUTFILE << "#" << hex << +ibyte1;
		if ((i & 3) != 3 && ibyte1 != 0xff) OUTFILE << ", ";
		i++;
		fileoffset++;
	} while (ibyte1 != 0xff);
	
	if (i != 1025) OUTFILE << "\n\tds " << dec << 1025-i << ",#ff";		//fill bytes not stored in compressed state
	
	//decrunch patterns
	OUTFILE << "\n\nptn00";
	unsigned int j,k;
	i = 0;
	do {
		
		HTFILE.seekg(fileoffset, ios::beg);
		HTFILE.read((&readb), 1);
		ibyte1 = static_cast<unsigned char>(readb);
		if ((i & 15) == 0 && ibyte1 < 0xe0) OUTFILE << "\n\tdb ";		//create a new line every 16 bytes
		if (ibyte1 < 0xd0) {
			OUTFILE << "#" << hex << +ibyte1;
			if ((i & 15) != 15 && ibyte1 != 0xff) OUTFILE << ", ";
			i++;
			
		}
		if (ibyte1 >= 0xd0 && ibyte1 < 0xe0) {
			j = ibyte1 - 0xcf;
			for (k = 0; k < j; k++) {
				i++;
				OUTFILE << "#0";
				if ((i & 15) != 15 && ibyte1 != 0xff) OUTFILE << ", ";
			}	
		}
		if (ibyte1 >= 0xe0 && ibyte1 < 0xff) {
			j = (ibyte1 - 0xdf) * 16;
			OUTFILE << "\n\tds " << dec << j << "\n\t";
			i = i + j;
		}
		
		fileoffset++;
		
	} while (ibyte1 != 0xff);
	
	i++;
	if (2048 - i != 0) OUTFILE << "\n\tds " << dec << 2049 - i << "\n\n";
	
	//decrunch fx patterns
	OUTFILE << "fxptn00\n";
	
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read((&readb), 1);
	ibyte2 = static_cast<unsigned char>(readb);
	i = 0;
	
	if (ibyte2 < 0xff) {
		do {
			HTFILE.seekg(fileoffset, ios::beg);
			HTFILE.read((&readb), 1);
			ibyte2 = static_cast<unsigned char>(readb);
			fileoffset++;
			
			cout << hex << +ibyte2 << endl;
			
			if (ibyte2 == i) {
				OUTFILE << "fxptn" << +ibyte2 << "\tdb ";
				for (j = 0; j < 32; j++) {
					HTFILE.seekg(fileoffset, ios::beg);
					HTFILE.read((&readb), 1);
					ibyte1 = static_cast<unsigned char>(readb);
					fileoffset++;
					OUTFILE << "#" << hex << +ibyte1;
					if (j != 31) OUTFILE << ",";
				}
				i++;
			}
			else {
				for (; (ibyte2 & 0x3f) > i; i++) {
					cout << i << "\n";
					OUTFILE << "fxptn" << i << "\tds 32\n";
				}
				OUTFILE << "fxptn" << +(ibyte2 & 0x3f) << "\tdb ";
				for (j = 0; j < 32; j++) {
					HTFILE.seekg(fileoffset, ios::beg);
					HTFILE.read((&readb), 1);
					ibyte1 = static_cast<unsigned char>(readb);
					fileoffset++;
					OUTFILE << "#" << hex << +ibyte1;
					if (j != 31) OUTFILE << ",";
				}
				i++;
			}
	
		} while (ibyte2 < 0x40);
	}
	else {
		OUTFILE << "\tds 2048";		//insert 2048 zerobytes if no fx patterns are found
	}
	
	cout << dec << "Done.\n";
	OUTFILE.close();
	return 0;
}

//extract a savestate to file
int extractState(char statev) {
	
	char buffer[5200];	
	
	//request user to input a state number and filename
	int stateno = getSavestateNo();
	if (statelen[stateno] == 0) {				//trap empty savestates
		cout << "Error: savestate is empty.\n";
		return -1;
	}
	string outfilename = getOutfileName(".ht2s");
	if (outfilename == "") return -1;			//back to main menu if user chose to not overwrite existing file

	int fileoffset = statebeg[stateno] - basediff;
	
	//create output file
	ofstream OUTFILE;
	OUTFILE.open (outfilename.c_str(), ios::out | ios::binary);
	
	OUTFILE << "HT2SAVE" << statev << static_cast<unsigned char>(htver);				//header
	
	HTFILE.seekg(fileoffset, ios::beg);
	HTFILE.read(buffer, statelen[stateno]);
	
	OUTFILE.write(buffer, statelen[stateno]);

	cout << "Done.\n";
	OUTFILE.close();
	return 0;
}


//get input filename from user input
string getInfileName() {

	string infilename;
	string suffix = ".ht2s";
	string cmd = "";
	string ext = "";
	
	cout << "Enter file name: ";
	cin >> infilename;
	
	//check if .ht2s ending was supplied, and add it if it wasn't
	size_t dot = infilename.find_last_of(".");
	if (dot != std::string::npos) {
		ext = infilename.substr(dot, infilename.size() - dot);
	}
	
	if (ext != ".ht2s") infilename += suffix;

	//test if file exists
	ifstream TESTFILE(infilename.c_str());
	
	if (!TESTFILE) {
		cout << "Error: " << infilename << " does not exist.\n";
		infilename = "";
	}
	
	return infilename;	
}


//get output filename from user input
string getOutfileName(string suffix) {

	string outfilename;
	string cmd = "";
	
	cout << "Enter file name: ";
	cin >> outfilename;
	outfilename = outfilename + suffix;

	//test if file exists, and ask if user wants to overwrite if necessary
	ifstream TESTFILE(outfilename.c_str());
	
	if (TESTFILE) {
		cout << "File already exists. Overwrite (y|n)? ";
		while (cmd != "y" && cmd != "n" && cmd != "Y" && cmd != "N") {
			cin >> cmd;
			if (cmd != "y" && cmd != "n" && cmd != "Y" && cmd != "N") cout << "y|n only, please.\n";
			if (cmd == "n") {
				outfilename = "";
				return outfilename;
			}
		}
	}
	
	return outfilename;	
}

//get savestate number from user input
int getSavestateNo() {

	int stateno = -1;
	
	while (stateno < 0 || stateno > 7) {
		cout << "Enter state number: ";
		cin >> stateno;
		//trap faulty input
		if (cin.fail() || stateno < 0 || stateno > 7) {
			cout << "Invalid savestate number, try again.\n";
			cin.clear();
			cin.ignore(10000,'\n');
			stateno = -1;
		}
	}
	
	cin.clear();			//flush accidental extra chars from input
	cin.ignore(10000,'\n');
	
	return stateno;
}

//recalculate checksum and write it to file
void writeChecksum() {
	
	HTFILE.seekg(0, ios::end);
	unsigned int htsize = HTFILE.tellg();
		
	char cbuf[htsize - 55 - 2];	//without header and checksum
	char outb;
	//fileoffset = 55;
	HTFILE.seekg(55, ios::beg);
	HTFILE.read(cbuf, (htsize-55-2));
	
	long checksum = 0;
	for (unsigned int i = 0; i < (htsize - 55 - 2); i++) {
		checksum += static_cast<unsigned char>(cbuf[i]);
	}
	
	HTFILE.seekp(-2, ios::end);

	checksum = checksum & 0xffff;
	outb = checksum & 0xff;
	HTFILE << outb;
	outb = char(checksum/256) & 0xff;
	HTFILE << outb;
	
	return;
}