OBJ= bit_mask.o bss_malloc.o comm.o error.o gs.o ivec.o queue.o stack.o xyt.o
CFLAGS = -g -Dr8 -DMPISRC -DMLSRC -I../../src  $(MPI_INC_PATH)

libxyt.a: $(OBJ)
	ar ruv libxyt.a *.o
	
clean:
	rm -f $(OBJ) *.a
