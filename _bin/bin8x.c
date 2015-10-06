 /* Bin8x v1.4 for Linux,*BSD,...
  * Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>
  * Copyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>
  * Copyright (C) 2004 Guillaume Hoffmann <guillaume.h@ifrance.com>
  * Copyright (C) 2011 thibault Duponchelle <t.duponchelle@gmail.com>
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Library General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  * 
  * Thanks to: Solignac Julien for optimizing the code and making it look more 
  * like devpac.  
  *
  *
  * Compiling the source for your unix:
  * with gcc use: gcc bin8x.c -o bin8x
  */
  
  
/* TI-83 program format :
 Byte(s) (in decimal)             What It Contains
  ------------------------------------------------------
  1-11                      "**TI83**"[26][10][0]
  12-53                     Comment
  54-55                     file length - 57  =  Size of all data in the .8?? file,
                            from byte 55 to last byte before the checksum
  56-57                     [11][0]
  58-59                     Length of data (word)
  60                        program type : 5 (6 for protected)
  61-68                     program name (0-filled) 
  69-70                     Length of data
  71-72                     Length of program
  73-(n-2)                  Actual Program
  (n-1)-n 	            Checksum
 
 * All the length are one word, with Least Significant Byte first
 * Length of program = the length of all the program data (incredible !)
 * Length of data = length of program + 2 (cause it is length of the datablock,
   which contains program length(2 bytes) + program data)
 * Word at 53-54 = length of program + 17
 * The checksum is one word, the sum of all the bytes from byte 55 to
   the last byte before the checksum, modulo 2^16-1 to fit in one word
*/
  
 /* TI-82 program format :
 Byte(s) (in decimal)             What It Contains
  ------------------------------------------------------
  1-11                             **TI82**[26][10][0]
  12-53                            Comment
  54-55                            (69-70)+15 or (71-72)+17
  56-57                            [11][0]
  58-59                            Repeats 69-70
  60                               [5] unprotected
                                   [6] protected
  61-68                            Name of program
  69-70                            (71-72)+2
  71-72                            Length of file to end minus Checksum
  73-(n-2)                         Actual program
  (n-1)-(n)                        Checksum
*/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <getopt.h>
#include <malloc.h>

#define true 1
#define false 0

#define indef 1
#define outdef 2
#define namedef 4

/* An enum to define possible extensions */
enum {
	EXT_NULL = '0',
	EXT_82P = '2',
	EXT_83P = '3',
	EXT_8XP = '4'
};

/* The extensions */
const char      ext82[] = ".82p";
const char      ext83[] = ".83p";
const char      ext8x[] = ".8xp";

/* A structure for command line arguments */
typedef struct CmdLineArgs {
    char           *input;		/* The input filename */
    char           *output;		/* The output filename */
    char           *name;		/* The name on the calc */
    char            destcalc_id;	/* could be '2' or '3' or '4' */
    short int       uppercase;		/* Keep or not lowercase for calcname (uppercase is recommended!) */
    short int       executable;		/* Force executable (ti83plus) */
    unsigned char   progtype;		
    short int	    unsquish;		/* Unquish a ti83 program (no shell only) */
    short int	    verbose;		/* Be verbose */
    short int	    help;		/* Be verbose */
    int		    flag; 		/* A flag to know if at least one argument is parsed */ 
} CmdLineArgs;

/* This struct is used by getopt_long_only for argument parsing */
static struct option long_options[] =
             {
               {"ti82",     no_argument,       0, '2'},
               {"ti83",  no_argument,       0, '3'},
               {"ti83p",  no_argument,       0, '4'},
               {"input",  required_argument, 0, 'i'},
               {"output",  required_argument, 0, 'o'},
               {"name",    required_argument, 0, 'n'},
               {"executable",    no_argument, 0, 'x'},
               {"unprotected",    no_argument, 0, 'u'},
               {"unsquish",    no_argument, 0, 'q'},
               {"lowercase",    no_argument, 0, 'l'},
               {"verbose",    no_argument, 0, 'v'},
               {"help",    no_argument, 0, 'h'},
               {0, 0, 0, 0}
             };


unsigned char   LL(unsigned short int);
unsigned char   HH(unsigned short int);
void            help(char *name, int ret);	/* Print help */
int             getargs(int argc, char *argv[], CmdLineArgs * cmdline); /* Command Line Parsing */
void            cmdline_init(CmdLineArgs * cmdline);	 /* Set all variable to null or default values */ 
void            cmdline_finalize(CmdLineArgs * cmdline); /* Some after parsing tasks */
void print_cmdline(CmdLineArgs *cmdline); /* Some debug */
int unsquish(char* filename); /* Unsquish a programm for ti83 regular no shell */
void autoselect_ext(CmdLineArgs* cmdline, char* filename); /* Detect the input/output and use extension given */

/* Set all variables to null or default values */
void cmdline_init(CmdLineArgs * cmdline)
{
    cmdline->input = NULL;
    cmdline->output = NULL;
    cmdline->name = NULL;
    cmdline->destcalc_id = EXT_NULL;
    cmdline->uppercase = true;
    cmdline->executable = false;
    cmdline->progtype = 0x06;
    cmdline->unsquish = false;
    cmdline->verbose = false;
    cmdline->flag = 0;
}


/* Get args using getopt */
int getargs(int argc, char *argv[], CmdLineArgs * cmdline)
{
    char options;
    opterr = 0;
    int option_index = 0;


    if (argc <= 1)
	help(argv[0], -1);
	

    while((options = getopt_long_only(argc, argv, "i:o:n:234xulq", long_options, &option_index)) != -1) {
	
	switch (options) {
	case 'i':
	    cmdline->input = optarg;
	    cmdline->flag = 1;
	    break;

	case 'o':
	    cmdline->output = optarg;
	    cmdline->flag = 1;
	    break;
	
	case 'n':
	    cmdline->name = optarg;
	    cmdline->flag = 1;
	    break;

	case '2':
	    cmdline->destcalc_id = EXT_82P;
	    cmdline->flag = 1;
	    break;

	case '3':
	    cmdline->destcalc_id = EXT_83P;
	    cmdline->flag = 1;
	    break;

	case '4':
	    cmdline->destcalc_id = EXT_8XP;
	    cmdline->flag = 1;
	    break;

	case 'l':
	    cmdline->uppercase = false;
	    cmdline->flag = 1;
	    break;

	case 'x':
	    cmdline->executable = true;
	    cmdline->flag = 1;
	    break;

	case 'u':
	    cmdline->progtype = 0x05;
	    cmdline->flag = 1;
	    break;
	
	case 'q':
	    cmdline->unsquish = true;
	    cmdline->flag = 1;
	    break;
	
	case 'v':
	    cmdline->verbose = true;
	    cmdline->flag = 1;
	    break;
	
	case 'h':
	    help(argv[0], 0);
	    cmdline->flag = 1;
	    break;



	default:
	    fprintf(stderr, "Erreur d'option\n");
	    help(argv[0], -1);
	    break;
	}

   }

	/* Get the non option args and use it as input/output */
	int index;
	 for (index = optind; index < argc; index++) {
		if(!cmdline->output)
	        	autoselect_ext(cmdline, argv[index]);
	}


    /* If no option use the twice parameter */
    if((!cmdline->input) && (cmdline->flag == 0) && (argc == 3)) { 
	autoselect_ext(cmdline, argv[1]);
	autoselect_ext(cmdline, argv[2]);
    /* Use parameter as input (and output) */
    } else if((!cmdline->input) && (cmdline->flag == 0) && (argc == 2)) {
	cmdline->input = (char*) malloc (strlen(argv[1]) * sizeof(char) + 1);
	strcpy(cmdline->input, argv[1]);
    }
     
    if((!cmdline->input) && (!cmdline->output)) 
	help(argv[0], -1);

	

    return 0;
}

/* Use the filename as input or output depending on extension
 * Set the destcalc_id using comparaison with extension */
void autoselect_ext(CmdLineArgs* cmdline, char* filename) {
	char* arg=(char*) malloc(strlen(filename) + 1);
	memcpy(arg, filename, strlen(filename));
	char* p = (char*)strrchr(arg, '.'); /* Get a pointer on the extension (null if no extension) */
	if(p) {
		/* Is it a destination file? */ 
		if((strcmp(p, ext82) == 0) ||(strcmp(p, ext83) == 0) || (strcmp(p, ext8x) == 0)) {
			if(strcmp(p, ext82) == 0) { 
				cmdline->destcalc_id = EXT_82P;
			} else if(strcmp(p, ext83) == 0) {
				cmdline->destcalc_id = EXT_83P;
			} else {
				cmdline->destcalc_id = EXT_8XP;
			}
			cmdline->output = (char*) malloc(strlen(arg) * sizeof(char) + 4);
			strcpy(cmdline->output, arg);	
			printf("%s\n", cmdline->output);
		} else {
			cmdline->input = (char*) malloc(strlen(arg) * sizeof(char) + 1);
			memcpy(cmdline->input, arg, strlen(arg));	
		}
	} else {
		cmdline->input = (char*) malloc(strlen(arg) * sizeof(char) +1 );
		memcpy(cmdline->input, arg, strlen(arg));	
	}
			
}


/* Some debugging informations */
void print_cmdline(CmdLineArgs *cmdline) {
	printf("input : %s\noutput: %s\nname: %s\ndestcalc_id: %c\nuppercase: %d\nexecutable: %d\nprogtype: %d\nflag: %d\n",cmdline->input,cmdline->output, cmdline->name, cmdline->destcalc_id, cmdline->uppercase, cmdline->executable, cmdline->progtype, cmdline->flag);
}


/* #1# Copy the input into the output if needed
 * #2# Copy the output into the input if needed
 * #3# Detect extension (may be redundancy) 
 * #4# Use default destcalc_id if needed 
 * #5# Copy the calcname if needed
 * #6# Keep only the first 8 char for the name
 * #7# Use uppercase for calcname if needed
 * #8# Add an special char for crash 
 */	
void cmdline_finalize(CmdLineArgs * cmdline)
{

    int i = 0;

    /* #1# Copy input into output if no output name */
    if(!cmdline->output) {
	if(cmdline->input) { /* Use input name */
		cmdline->output = (char*) malloc(strlen(cmdline->input) * sizeof(char) + 4);
		strcpy(cmdline->output, cmdline->input);
	}
    }

    /* #2# Copy output into input if no input name */
    if(!cmdline->input) {
	if(cmdline->output) { /* Use output name */
		cmdline->input = (char*) malloc(strlen(cmdline->output) * sizeof(char) + 1);
		strcpy(cmdline->input, cmdline->output);
		char* p = (char*)strrchr(cmdline->input, '.'); /* Get a pointer on the extension (null if no extension) */
		if(p) 
			strcpy(p, ".bin");
			
	}
    }
    
    /* #3# Use extension to define destcalc_id then drop extension (because it could be false!) */
    char* p = (char*)strrchr(cmdline->output, '.');
    if(p) {
    	//printf("destcalc : %s\n", p);
	if(strcmp(p, ext82) == 0) cmdline->destcalc_id = EXT_82P;
	if(strcmp(p, ext83) == 0) cmdline->destcalc_id = EXT_83P;
	if(strcmp(p, ext8x) == 0) cmdline->destcalc_id = EXT_8XP;
	if(p) strcpy(p, "\0");
    }
	
    /* #4# If no extension is defined, use 83p as default */
    if(cmdline->destcalc_id == EXT_NULL) 
	cmdline->destcalc_id = EXT_83P;

    /* #5# Copy the name if needed */
    if(!cmdline->name) {
	    cmdline->name = (char*) malloc(sizeof(char) * 8);
	    memset(cmdline->name, '\0', 8);
	    for(i = 0; i<8; i++) {
		cmdline->name[i] = cmdline->output[i]; 
	    }
    }
    
    /* #6# If calcname is too long, drop the end */	
    if(strlen(cmdline->name) > 8)
	cmdline->name[8] = '\0';

    /* #7# Convert automatically to uppercase calcname (only if -l is not given) */
    if (cmdline->uppercase == true) {
	for (i = 0; i < 8; i++) {
	    if (islower(cmdline->name[i])) {
		cmdline->name[i] = toupper(cmdline->name[i]);
	    }
	}
    }

    /* #8# for ti-82 CrASH, put an inverted lowercase 'a' as first character */
    if (cmdline->destcalc_id == EXT_82P) {
	for (i = 7; i; i--) {
	    cmdline->name[i] = cmdline->name[i - 1];
	}
	cmdline->name[0] = 220;
    }
}

/* This special function is used for generating TI83 regular programs (NO SHELL) */
int unsquish(char* filename) {
	FILE *fp, *fpr;
	int e;
	char end[] = {0x3F ,0xD4  ,0x3F ,0x30 ,0x30 ,0x30  ,0x30 ,0x3F ,0xD4};
			
	if((fpr = fopen(filename, "r+b"))) {
		if(!(fp=fopen("__temp__.bin","w+b"))) {
			fclose(fpr);
			return 1;
		}

		while((e=fgetc(fpr))!=EOF)
		{
			//printf("%c= %02X ",c, c); //Print hexa code could bug the console ;D
			fprintf(fp,"%02X",e);
		}
		fwrite(&end,9,1,fp);
		fclose(fp);
		fclose(fpr);
	}
	return 0;
}

/* The main function */
int main(int argc, char *argv[])
{
    struct utsname  system;
    FILE           *infile, *outfile;
    unsigned short int i, n, filesize;
    unsigned char   buffer, optiondefs = 0;
    unsigned char   programData[28000];
    unsigned short int checksum;

    /* --- Header File Values --- */
    /* The header will be modified if your destcalc_id is not 3... */
    unsigned char   header[11] = { '*', '*', 'T', 'I', '8', '3', '*', '*', 0x1A, 0x0A, 0x00 };
    unsigned char   comment[42] = "File created under ";
    unsigned char   fileLenLL = 0x00;
    unsigned char   fileLenHH = 0x00;
    unsigned char   varHeadLL = 0x00;
    unsigned char   varHeadHH = 0x00;
    unsigned char   dataLenLL = 0x00;
    unsigned char   dataLenHH = 0x00;
    unsigned char   programLenLL = 0x00;
    unsigned char   programLenHH = 0x00;
    if (uname(&system) == -1)
	strcat(comment, "unknown system");
    else
	strncat(comment, system.sysname, 20);
    /* --- Header File Values --- */


    /* Parse all arguments */
    struct CmdLineArgs *cmdline = (CmdLineArgs *) malloc(sizeof(CmdLineArgs));
    cmdline_init(cmdline);	/* set to null or default values all the fields of the struct */
    getargs(argc, argv, cmdline);	/* Get cmdline args */
    cmdline_finalize(cmdline);	/* Set to uppercase (or not). Add inverted a for crash (only 82p) */

    /* At this point, all cmdline args should be parsed correctly */ 

    	
    if(cmdline->verbose) { 
    	puts("Bin8x v1.3 Ti-82/83/83+ squisher");
    	puts("Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>");
    	puts("Copyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>");
    	puts("Copyright (C) 2004 Guillaume Hoffman <guillaume.h@ifrance.com>");
    	puts("Copyright (C) 2011 Thibault Duponchelle <t.duponchelle@gmail.com>");
    }

    /* Concat extension */
   
    switch(cmdline->destcalc_id) {
	case EXT_82P:
		header[5] = '2';
		strcat(cmdline->output, ext82);
		break;
	case EXT_83P:
		strcat(cmdline->output, ext83);
		break;
	case EXT_8XP:
		header[6] = 'F';
		strcat(cmdline->output, ext8x);
		break;
	default:
		puts("\nError : not a valid extension!");
		exit(2);
		break;
	}
   
	/* At this point, all the informations are correct, we can do the real job */
	if(cmdline->verbose)
		print_cmdline(cmdline);

    /* Unsquish the code if user asked for it (option -q) */
    if(cmdline->unsquish) {
	if((strcmp(cmdline->input, "__temp__.bin") == 0) || strcmp(cmdline->output, "__temp__.bin") == 0) {
		printf("You must use another input/output filename...\n");
		exit(2);
	}
	int ret = unsquish(cmdline->input);
	if(ret == 1)
		return 1;
    } 
	 


	
    /* Unsquisher use a different file */	
    if (cmdline->input) {
	if(cmdline->unsquish){ 
		if(!(infile = fopen("__temp__.bin", "r"))) {
		    	puts("Error opening inputfile!");
		    	printf("File: __temp__.bin\n");
		    	return (2);
		}
	} else {
		if (!(infile = fopen(cmdline->input, "r"))) {
		    puts("Error opening inputfile!");
		    printf("File: %s\n", cmdline->input);
		    return (2);
		}
	}
	printf("Using inputfile        : %s\n", cmdline->input);
    }

    		
	if (!(outfile = fopen(cmdline->output, "w"))) {
	    puts("Error opening outputfile!");
	    return (3);
	}
	printf("Using outputfile       : %s\n", cmdline->output);

    printf("Filename on calculator : %s\n", cmdline->name);

    filesize = -1;

    if (cmdline->executable && cmdline->destcalc_id == EXT_8XP) {
	filesize++;
	programData[filesize] = 0xBB;
	filesize++;
	programData[filesize] = 0x6D;
    }
    // UNTESTED : (works only for CrASH(19006))
    if (cmdline->destcalc_id == EXT_82P) {
	filesize++;
	programData[filesize] = 0xD5;
	filesize++;
	programData[filesize] = 0x00;
	filesize++;
	programData[filesize] = 0x11;
    }


    while (!feof(infile)) {
	filesize++;
	buffer = fgetc(infile);
	programData[filesize] = buffer;	/* put this byte in the data
					 * array, and increase the count */
    }
	
    //printf("dcid : %c\n", cmdline->destcalc_id);
    switch(cmdline->destcalc_id) {
	case EXT_82P:
		// THAT IT WORKS
		printf("Size on calculator     : %u bytes\n", filesize + strlen(cmdline->name) + 6);
		fileLenHH = HH(filesize + 0x11);	/* the file length = the size of the data array + 17 (0x11) */
		fileLenLL = LL(filesize + 0x11);
		varHeadLL = 0x0B;
		break;

	case EXT_83P:
		printf("Size on calculator     : %u bytes\n", filesize + strlen(cmdline->name) + 6);
		fileLenHH = HH(filesize + 0x11);	/* the file length = the size of the data array + 17 (0x11) */
		fileLenLL = LL(filesize + 0x11);
		varHeadLL = 0x0B;
		break;

	default: 
		//EXT_8XP:
		printf("Size on calculator     : %u bytes\n", filesize + strlen(cmdline->name) + 8);
		fileLenHH = HH(filesize + 0x13);	/* the file length = the size of the data array + 19 (0x13) */
		fileLenLL = LL(filesize + 0x13);
		varHeadLL = 0x0D;
		break;

	    }

    dataLenHH = HH(filesize + 2);	/* the length of the data includes the 2 checksum bytes */
    dataLenLL = LL(filesize + 2);

    programLenHH = HH(filesize);
    programLenLL = LL(filesize);

    checksum = 0x00;		/* begin with the checksum set on zero */

    for (i = 0; i < 11; i++)	/* write the static header */
	fputc(header[i], outfile);

    for (i = 0; i < 42; i++)
	fputc(comment[i], outfile);

    fputc(fileLenLL, outfile);	/* file length */
    fputc(fileLenHH, outfile);

    fputc(varHeadLL, outfile);	/* length of variable header */
    checksum += varHeadLL;	/* checksum calculs start here */
    fputc(varHeadHH, outfile);
    checksum += varHeadHH;

    fputc(dataLenLL, outfile);	/* length of data */
    checksum += dataLenLL;
    fputc(dataLenHH, outfile);
    checksum += dataLenHH;

    fputc(cmdline->progtype, outfile);	/* protected program */
    checksum += cmdline->progtype;



    for (i = 0; i < 8; i++) {	/* write the name of the variable */
	fputc(cmdline->name[i], outfile);
	checksum += cmdline->name[i];
    }

	
    switch(cmdline->destcalc_id) {
	case EXT_8XP:
		puts("\nTI-83 Plus file made!!");
		fputc(0x01, outfile);
		checksum += 0x01;
		fputc(0x00, outfile);
		break; /* checksum += 0x00; */
	case EXT_83P:
		puts("\nPlain TI-83 file made!!");
		break;
	case EXT_82P:
		puts("\nTI-82 file made!!");
		break;
	default:
		puts("\nError : not a valid extension!");
		exit(2);
		break;
    }
    

    fputc(dataLenLL, outfile);
    checksum += dataLenLL;
    fputc(dataLenHH, outfile);
    checksum += dataLenHH;

    fputc(programLenLL, outfile);	/* length of program */
    checksum += programLenLL;
    fputc(programLenHH, outfile);
    checksum += programLenHH;


    for (i = 0; i < filesize; i++) {	/* now write the data array to the 
					 * file */
	fputc(programData[i], outfile);
	checksum += programData[i];	/* and add it to the checksum */
    }


    fputc(LL(checksum), outfile);	/* write the checksum to the file */
    fputc(HH(checksum), outfile);


    /*
     * Close all Handles 
     */
    fclose(infile);
    fclose(outfile);

    free(cmdline);

    return (0);
}


/*
 * I use the following functions to ensure that the words 
 * (16 bit) are stored in LLHH order,
 * since we don't know what type of Endian the target platform uses
 */

unsigned char
LL(unsigned short int toSplit)
{
    unsigned char   LL;
    LL = (toSplit & 0x00FF);
    return (LL);
}

unsigned char
HH(unsigned short int toSplit)
{
    unsigned char   HH;
    toSplit >>= 8;
    HH = (toSplit & 0x00FF);
    return (HH);
}

/* Print help */
void help(char *name, int ret)
{
    fprintf(stdout, "Bin8x v1.4 Ti-82/83/83+ squisher\n"
	    "\t\tCopyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>\n"
	    "\t\tCopyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>\n"
	    "\t\tCopyright (C) 2004 Guillaume Hoffmann <guillaume.h@ifrance.com>\n\n"
	    "\t\t\033[01mSyntax:\n"
	    "\t\t\033[01m%s\033[0m binfile [options]\n"
	    "\t\t\033[01m%s\033[0m -i infile -o outfile -n name [options]\n\n"
	    "\t\tbinfile  Binary output from your assembler, the .bin extension is assumed when\n"
	    "\t\t        not using the -i option. You must not give it.\n\n"
	    "\t\t\033[01mOptions:\033[0m\n"
	    "\t\t-o   or -output       specific outputfile ([binfile].8?p if none given.\n"
	    "\t\t-n   or -calcname     specific filename on the calculator ([binfile] if\n"
	    "\t\t                      none given.\n"
	    "\t\t-i   or -input        specific inputfile, you should use -o and -n if you\n"
	    "\t\t                      use it. Also give the complete filename with extension.\n"
	    "\t\t-2   or -ti82         specify ti-82 file (CrASH(19006)).\n"
	    "\t\t-3   or -ti83         specify ti-83 file.\n"
	    "\t\t-4   or -ti83plus     specify ti-83 plus file.\n"
	    "\t\t-l   or -lowercase    do not convert calculator file name to uppercase.\n"
	    "\t\t-x   or -executable   force \"TIOS-executable\" bytes to the TI-83 Plus program\n"
	    "\t\t-u   or -unprotected  generate unprotected files (I don't see any use for\n"
	    "\t\t                      that, but you might...).\n"
	    "\t\t-q   or -unsquish     use it for regular 83 without shell\n"
	    "\t\t-v   or -verbose      provide a lot of informations while running\n"
	    "\t\tYou can only specify one calculator (with -82/ -83 / -83p) at a time.\n\n"
	    "\t\tPublised under the Gnu Public License - Greets to the Ti8x Assembly Scene\n",
	    name, name);

    exit(ret);
}
