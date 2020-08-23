FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: cluster eigen errors help modMat spmat
clean:
	rm -rf *.o cluster eigen errors help modMat spmat

eigen: eigen.o
	gcc eigen.o -o eigen $(LIBS)
eigen.o: eigen.c eigen.h spmat.h errors.h help.h
	gcc $(FLAGS) -c eigen.c

errors: errors.o
	gcc errors.o -o errors $(LIBS)
errors.o: errors.c errors.h
	gcc $(FLAGS) -c errors.c

modMat: modMat.o
	gcc modMat.o -o modMat $(LIBS)
modMat.o: modMat.c modMat.h help.h errors.h spmat.h
	gcc $(FLAGS) -c modMat.c

spmat: spmat.o
	gcc spmat.o -o spmat $(LIBS)
spmat.o: spmat.c spmat.h errors.h
	gcc $(FLAGS) -c spmat.c

help: help.o 
	gcc help.o -o help $(LIBS)
help.o: help.c help.h modMat.h eigen.h errors.h spmat.h
	gcc $(FLAGS) -c help.c

cluster: cluster.o
	gcc cluster.o -o cluster $(LIBS)
cluster.o: cluster.c help.h
	gcc $(FLAGS) -c cluster.c