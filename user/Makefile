fs.img: mkfs $(UPROGS)
	./tools/mkfs fs.img $(UPROGS)

ULIB = lib/ulib.o lib/usys.o lib/printf.o lib/umalloc.o

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym


mkfs: tools/mkfs.c kernel/include/fs.h
	gcc -Werror -Wall -o tools/mkfs tools/mkfs.c

.PRECIOUS: %.o

UPROGS=\
	bin/_cat\
	bin/_echo\
	bin/_forktest\
	bin/_grep\
	bin/_init\
	bin/_kill\
	bin/_ln\
	bin/_ls\
	bin/_mkdir\
	bin/_rm\
	bin/_sh\
	bin/_stressfs\
	bin/_usertests\
	bin/_wc\
	bin/_zombie\