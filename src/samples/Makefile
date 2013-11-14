LIBLITMUS      = /usr/src/linux.3.10.5-litmusRT/liblitmus/
ARCH           = x86_64              
CFLAGS         = -O2 -Wall -Werror -g -Wdeclaration-after-statement
LDFLAGS        = -m64                
LDLIBS         = -L${LIBLITMUS} -llitmus -lpthread
CPPFLAGS       = -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -m64 -DARCH=x86_64 -I${LIBLITMUS}/include -I${LIBLITMUS}/arch/x86/include -I${LIBLITMUS}/arch/x86/include/uapi -I${LIBLITMUS}/arch/x86/include/generated/uapi
CC             = /usr/bin/gcc        
LD             = /usr/bin/ld         
AR             = /usr/bin/ar

% : %.c 
	$(CC) $(CFLAGS) -o $@ $< $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)
	
all: $(basename $(wildcard *.c))
	
.PHONY: clean
clean:
	@rm -f *.o $(basename $(wildcard *.c))