#!/bin/sh

	if [ "$1" = "-82" ]
	then
		pasmo -d --equ MODEL=1 --alocal main.asm main.bin main.sym #> dump.lst
		if [ "$?" = "0" ]
		then
			./oysterpac.pl main.bin ht2.82p
			rm main.bin
			tilem2 -a -r "/path/to/your/rom/romfile.bin" -m ti82 ht2.82p -p "macro/ti82cr.txt"
		fi
	fi
	if [ "$1" = "-8p" ]
	then
		pasmo -d --equ MODEL=4 --alocal main.asm main.bin main.sym #> dump.lst
		if [ "$?" = "0" ]
		then
			./oysterpac.pl main.bin ht2.82p
			rm main.bin
			tilem2 -a -r "/path/to/your/rom/romfile.bin" -m ti82 ht2.82p
		fi
	fi
	if [ "$1" = "-83" ]
	then
		pasmo --equ MODEL=2 --alocal main.asm main.bin main.sym
		if [ "$?" = "0" ]
		then
			cp _bin/bin8x bin8x
			./bin8x -i main.bin -o ht2.83p -nHT2 -3 -x -v
			rm main.bin
			rm bin8x
			tilem2 -a -r "/path/to/your/rom/romfile.bin" -m ti83 ht2.83p
		fi
	fi
	if [ "$1" = "-8x" ]
	then
		pasmo -d --equ MODEL=3 --alocal main.asm main.bin main.sym
		if [ "$?" = "0" ]
		then
			cp _bin/bin8x bin8x
			./bin8x -i main.bin -o ht2.8xp -nHT2 -4 -v
			rm main.bin
			rm bin8x
			tilem2 -a -r "/path/to/your/rom/romfile.bin" -m ti83p ht2.8xp
		fi
	fi
# 	if [ "$1" = "-73" ]
# 	then
# 		cp _bin/as73.bat as73.bat
# 		cp _bin/asm73.exe asm73.exe
# 		./bin8x -i mark2.bin -o mark2.83p -nMARK2 -3 -v
# 		rm mark2.bin
# 		rm bin8x
# 		dosbox -c "cd texasi~1\PROG\mark-2" -exit "as73.bat"
# 		rm as73.bat
# 		rm asm73.exe
# 		mv MARK2.73P mark2.73p
# 		tilem2 -a -r "/path/to/your/rom/romfile.bin" -m ti73 mark2.73p
# 	fi

#fi
