all: n64

OBJS = about.h asm.h crc.h defines.h drjr.h err.h exp.h opcode.h pre.h symbols.h

n64: $(OBJS)
	$(CXX) -o n64 n64.cpp

clean:
	rm -f n64
