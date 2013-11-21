LIBLITMUS      = /usr/src/linux.3.10.5-litmusRT/liblitmus/
LIBTL2	       = ../../lib/
INCLUDE	       = include/
ARCH           = x86_64              
CFLAGS         = -O2  -Werror -g -Wdeclaration-after-statement
LDFLAGS        = -m64                
LDLIBS         = -L${LIBLITMUS} -L${LIBTL2} -llitmus -lpthread -ltl2 ../../lib/libtl2.a
CPPFLAGS       = -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -m64 -DARCH=x86_64 -DSTM -I${LIBLITMUS}include -I${LIBLITMUS}arch/x86/include -I${LIBLITMUS}arch/x86/include/uapi -I${LIBLITMUS}arch/x86/include/generated/uapi -I${INCLUDE} -I${INCLUDE}tl2
CC             = /usr/bin/gcc        
LD             = /usr/bin/ld         
AR             = /usr/bin/ar

% : %.c 
	$(CC) $(CFLAGS) -o $@ $< $(CPPFLAGS) $(LDLIBS) $(LDFLAGS) include/obj/*.o
	
all: $(basename $(wildcard *.c))
	
memorylib : include/memory.h
	$(CC) -c include/memory.c 
	
threadlib : include/thread.h
	$(CC) -c include/thread.c 
	
toolslib: memorylib threadlib
	mkdir include/obj
	mv thread.o include/obj/
	mv memory.o include/obj/
	
.PHONY: clean
clean:
	@rm -f *.o $(basename $(wildcard *.c))
	@rm -r include/obj