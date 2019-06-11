CC		= gcc
CFLAGS	= -Wall -Wextra -O2 -msse -msse2 -msse3 -msse4.2
MP_CFLAGS = -Wall -Wextra -O2 -msse -msse2 -msse3 -msse4.2 -fopenmp

TARGET	= sha1
OBJS	= sha1.o sha1test.o
TIME	= sha1time
TOBJS	= sha1.o sha1time.o
OMP	= sha1mp
MPOBJS	= sha1.o sha1mp.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

time: $(TIME)

$(TIME): $(TOBJS)
	$(CC) $(CFLAGS) $(TOBJS) -o $@

omp: $(OMP)

$(OMP): $(MPOBJS)
	$(CC) $(MP_CFLAGS) $(MPOBJS) -o $@

.c.o:
	$(CC) $(MP_CFLAGS) -c $<

clean:
	rm -f ./*.o
	rm -f sha1 sha1time sha1mp
