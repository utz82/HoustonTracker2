Bin8x IMPROVED v2.0 for Linux,*BSD,...
Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>
Copyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>
Copyright (C) 2004 Guillaume Hoffmann <guillaume.h@ifrance.com>
Copyright (C) 2011 thibault Duponchelle <t.duponchelle@gmail.com> (the improved version)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Library General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

Thanks to: Solignac Julien for optimizing the code and making it look more 
like devpac, and for the autodetection of filetypes.  


Here it is, the new bin8x squisher for unix, I've been busy and made a few updates:

- devpac compatibility (bin8x binfile-with-no-extension)
- TI82 CrASH compatibility.
- quite a few options to ensure usefulness:

-o  or -output      : specific outputfile ([binfile].8?p if none given.
-n  or -name        : specific filename on the calculator ([binfile] if
                        none given.
-i  or -input       : specific inputfile, you must use -o and -n if you
                        use it. Also give the complete filename with extension.
-2  or -ti82        : specify ti-82 file (CrASH(19006)).
-3  or -ti83        : specify ti-83 file.
-p  or -ti83plus    : specify ti-83 plus file.
-l  or -lowercase   : keep calculator name lowercase
-x  or -executable  : force  "TIOS-executable" bytes to the TI-83 Plus 
-q  or -unsquish     use it for regular 83 without shell program.
-u  or -unprotected : generate unprotected files (I don't see any use for
                        that, but you might...).
-v  or -verbose     : provide a lot of informations while running
-h  or -help        : display help screen.

syntax:

bin8x binfile [-options]

 or

bin8x -i infile -o outfile -n name [options]

or 

bin8x binfile output.8xx

or 

bin8x output.8xx binfile

or 

bin8x output.8xx

or 

bin8x output
(use 83p as default)

or 

bin8x binfile

or 

bin8x [options] binfile [options] output.8xx [options]

or 

bin8x [options] output.8xx [options] binfile [options]


Where binfile/infile	: the binary output from your assembler, binfile is 
		          in 8.3 dos format,
			  
Or some other ways...

You should not use options as -qlv but always use -q -v -l
It could be corrected in the future, but currently it's not working.


I hope all of you have use for this program.

BTW. It's open source, so don't hesitate to send me updates or bug fixes.


Compiling the source for your unix:

with gcc use: gcc bin8x.c -o bin8x

>>>>>>
Thibault Duponchelle (31/03/2011) : 

bin8x is the real unix-like calc var converter...
But it was not really user friendly...
Some functionnality are very cool (unprotect, print the size of output etc...)
but I always had prefered bin2var because it was very simple and works well.
That's why I decided to improve bin8x to make it better for me (maybe for others too?!)
Now, if you're Linux user, there's no reason to do not use bin8x !!! 
And the most important thing is there's a unsquisher inside (for asm noshell ti83).


New :
- Add a completely new command line parser using getopt :
There's a lot of new ways to use bin8x and lesser possibility to crash it
You can use -o alone, -i alone, or do not use options.
You can give arguments in the order you want, bin8x detect wich one is output/input.
If no extension given and no (-2 -3 -p), it uses 83p by default.

- Add a new option to unsquish a program :
This is really useful for TI83 regular designed to be used without shell
(with Send(9pgrmNAME )
But the program will be really bigger (2 times bigger) and slower.

- By default, convert to uppercase the calc name.
There's no reason to keep lowercase because the program will not launch on calc
But you can specify to keep lowercase (-l or -lowercase)

- Some (a lot of?) other improvements... 

I've added some script to test the tool but you don't need it. Just ignore them :)
I've found and fixed a little bug (8xp recognition was failing sometimes).
Sorry for this issue, I will continue to fix bugs if possible in the futur.

Have fun with bin8x xD
<<<<<<
