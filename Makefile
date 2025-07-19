CFLAGS=-g
LIBS=~cpen212/Public/lab3/lib/lib212alloc.a -lm

# Compile all `.c` files into `.o` files
%.o: %.c cpen212alloc.h cpen212common.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Add main.o to the final executable
cpen212alloc: cpen212alloc.o cpen212debug.o main.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Run the program after building
run: cpen212alloc
	./cpen212alloc

# Clean up object files and executable
.PHONY: clean
clean:
	$(RM) *.o cpen212allocCFLAGS=-g
LIBS=~cpen212/Public/lab3/lib/lib212alloc.a -lm

%.o: %.c cpen212alloc.h cpen212common.h
	$(CC) $(CFLAGS) -c -o $@ $<

cpen212alloc: cpen212alloc.o cpen212debug.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	$(RM) *.o cpen212alloc
