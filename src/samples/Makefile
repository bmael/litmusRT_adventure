LIB	       = ../../lib/
INCLUDE	       = include/
ARCHPATH       = arch/
ARCH           = x86_64              
CFLAGS         = -O2  -Werror -g -Wdeclaration-after-statement -DSTM
LDFLAGS        = -m64                
LDLIBS         = -L${LIB}liblitmus -L${LIB}libtl2 -lpthread $(LIB)libtl2.a $(LIB)liblitmus.a
CPPFLAGS       = -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -m64 -DARCH=x86_64 -I${INCLUDE} -I${ARCHPATH}x86/include -I${ARCHPATH}x86/include/uapi -I${ARCHPATH}arch/x86/include/generated/uapi -I${INCLUDE}tl2
CC             = /usr/bin/gcc        
LD             = /usr/bin/ld         
AR             = /usr/bin/ar

% : %.c 
	$(CC) $(CFLAGS) -o $@ $< $(CPPFLAGS) $(LDFLAGS) include/obj/*.o $(LDLIBS)
	
all: toolslib $(basename $(wildcard *.c))
	
memorylib : include/memory.h
	$(CC) -c include/memory.c 

uselessjobs: include/uselessjobs.h
	$(CC) $(CPPFLAGS) -c include/uselessjobs.c 
	
threadlib : include/thread.h 
	$(CC) $(CPPFLAGS) -c include/thread.c 
	
toolslib: memorylib threadlib uselessjobs
	mkdir -p include/obj
	mv thread.o include/obj/
	mv memory.o include/obj/
	mv uselessjobs.o include/obj/
	
	

	
.PHONY: clean
clean:
	@rm -f *.o $(basename $(wildcard *.c))
	@rm -r include/obj