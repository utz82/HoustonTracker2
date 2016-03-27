//oysterpac 0.2 - TI program variable packer
//by utz 2014-2015

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

ifstream INFILE;
ofstream OUTFILE;

string getExtension(string filename);

int main(int argc, char *argv[]){

	cout << "oysterpac 0.2 - TI program variable packer\n\n";

	
	//check if correct number of arguments has been passed
	if (argc <= 2) {
		cout << "USAGE: oysterpac <infile> <outfile> [<on-calc name>]\n";
		return -1;	
	}

	
	//open infile, and exit if that fails
	INFILE.open (argv[1], ios::in | ios::binary);
	if (!INFILE.is_open()) {
		cout << "Error: Could not open " << argv[1] << "\n";
		return -1;
	}

	
	//validate output file type by extension
	string outfilename = argv[2];
	string ext = getExtension(outfilename);

		
	//get binary size
	INFILE.seekg(0,ios_base::end);
	int binsize = INFILE.tellg();

	
	//generate on-calc name
	string oncalcname;
	if (argc == 4) {				//determine on-calc name from command line arg, or if none given, from outfile name
		oncalcname = argv[3];
	}
	else {
		oncalcname = outfilename;
	}
	
	transform(oncalcname.begin(), oncalcname.end(), oncalcname.begin(), ::toupper);		//convert to uppercase
	
	if (ext == ".82p") {				//pack to 7 (ti82) resp. 8 bytes and pad with spaces if necessary
		oncalcname.resize(7,' ');
		oncalcname.insert(0,"\xdc");		//add inverse @ to on-calc name
	}
	else {
		oncalcname.resize(8,' ');
	}
	//cout << oncalcname << "..." << endl;		//DEBUG


	//create output file
	ofstream OUTFILE;
	OUTFILE.open (outfilename.c_str(), ios::out | ios::binary);
	unsigned char outb;

	//offset 0, write signature
	string signature;
	const char nullbyte = 0;			//necessary because C++ interprets 0-bytes either as string terminator or as "0" character (0x30)
	
	if (ext == ".82p") signature = "**TI82**\x1a\x0a";
	if (ext == ".83p") signature = "**TI83**\x1a\x0a";
	if (ext == ".8xp") signature = "**TI83F*\x1a\x0a";
	OUTFILE << signature << nullbyte;
	
	//offset 11, write comment
	OUTFILE << "packed with oysterpac                     ";
	
	//offset 53, write data section size
	int datasecsize, i;
	
	if (ext == ".82p") datasecsize = binsize + 15 + 5;
	if (ext == ".83p") datasecsize = binsize + 15 + 2;
	if (ext == ".8xp") datasecsize = binsize + 15 + 4;
	outb = datasecsize & 0xff;
	OUTFILE << outb;
	outb = char(datasecsize/256);
	OUTFILE << outb;
	
	//offset 55, data section
	long checksum = 0;
	
	//write data section header (0x0011)
	if (ext == ".82p" || ext == ".83p") outb = 0xb;
	if (ext == ".8xp") outb = 0xd;
	OUTFILE << outb;
	checksum += outb;
	OUTFILE << nullbyte;
	
	unsigned binsizelong;
	
	if (ext == ".82p") binsizelong = binsize + 5;
	if (ext == ".83p" || ext == ".8xp") binsizelong = binsize + 2;
	
	//write binsize
	outb = (binsizelong & 0xff);
	OUTFILE << outb;
	checksum += outb;
	outb = char(binsizelong / 256);
	OUTFILE << outb;
	checksum += outb;
	
	//write type ID
	outb = 0x6;
	OUTFILE << outb;
	checksum += outb;
	
	//write filename
	OUTFILE << oncalcname;
	unsigned char strbuf[8];
	copy(oncalcname.begin(), oncalcname.end(), strbuf);
	for (i = 0; i <= 7; i++) {
		checksum += strbuf[i];
	}
	
	//for 8xp files, write version and flag
	if (ext == ".8xp") OUTFILE << nullbyte << nullbyte;
	
	//write binsize again
	outb = (binsizelong & 0xff);
	OUTFILE << outb;
	checksum += outb;
	outb = char(binsizelong / 256);
	OUTFILE << outb;
	checksum += outb;
	
	//and again, this time as "number of tokens"
	outb = (binsizelong & 0xff) - 2;
	OUTFILE << outb;
	checksum += outb;
	outb = char((binsizelong - 2) / 256);
	OUTFILE << outb;
	checksum += outb;
	
	//TI82: write CrASH header
	if (ext == ".82p") {
		outb = 0xd5;
		OUTFILE << outb << nullbyte;
		checksum += outb;
		outb = 0x11;
		OUTFILE << outb;
		checksum += outb;
	}
	
	//copy binary
	int fileoffset = 0;
	char buffer[binsize];
	
	INFILE.seekg(fileoffset, ios::beg);
	INFILE.read(buffer, binsize);
	
	OUTFILE.write(buffer, binsize);
	
	//write checksum
	for (i = 0; i < binsize; i++) {
		checksum += (unsigned char)buffer[i];
	}
	//checksum = checksum - 86;		//seems TI82 actually doesn't give a sh*t about the checksum, so whatever...
	outb = checksum & 0xff;
	OUTFILE << outb;
	outb = (unsigned char)(checksum/256);	// & 0xff;
	OUTFILE << outb;
	
	
	INFILE.close();
	OUTFILE.close();
	return 0;
}

//extract file extension from file name and check if it's a valid .8*p extension
string getExtension(string filename) {

	string ext = "";

	size_t dot = filename.find_last_of(".");
	if (dot != std::string::npos) {
		ext = filename.substr(dot, filename.size() - dot);
	}

	if (ext != ".82p" && ext != ".83p" && ext != ".8xp" && ext != ".82P" && ext != ".83P" && ext != ".8XP" && ext != ".8xP"  && ext != ".8Xp") {
		cout << "Error: Output file must be of type 82p/83p/8xp.\n";
		INFILE.close();
		exit(-1);
	}
	
	transform(ext.begin(), ext.end(), ext.begin(), ::tolower);	//unify extension to all lowercase

	return ext;
}