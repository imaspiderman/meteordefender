FILE=$1
/opt/gccvb/bin/v810-gcc -O -Wl,-Map,$FILE.map -o $FILE.o -T/home/greg/.wine/drive_c/vbde/gccvb/v810/lib/vb.ld -Xassembler -a=$FILE.lst -I ../../libs/libgccvb/ $FILE.c
/opt/gccvb/bin/v810-objdump -d $FILE.o > $FILE_disassembly.txt
/opt/gccvb/bin/v810-objcopy -S -O binary $FILE.o $FILE.vb
