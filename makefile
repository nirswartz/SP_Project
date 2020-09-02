FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: cluster.o divide.o eigen.o errors.o linkedList.o maximization.o modMat.o spmat.o
	gcc cluster.o divide.o eigen.o errors.o linkedList.o maximization.o modMat.o spmat.o -o cluster $(LIBS)
clean:
	rm -rf *.o cluster

cluster.o: cluster.c errors.h linkedList.h divide.h modMat.h
	gcc $(FLAGS) -c cluster.c

divide.o: divide.c divide.h eigen.h modMat.h errors.h maximization.h
	gcc $(FLAGS) -c divide.c

eigen.o: eigen.c eigen.h spmat.h errors.h
	gcc $(FLAGS) -c eigen.c

errors.o: errors.c errors.h
	gcc $(FLAGS) -c errors.c

linkedList.o: linkedList.c linkedList.h errors.h
	gcc $(FLAGS) -c linkedList.c

maximization.o: maximization.c maximization.h errors.h modMat.h
	gcc $(FLAGS) -c maximization.c

modMat.o: modMat.c modMat.h errors.h spmat.h
	gcc $(FLAGS) -c modMat.c

spmat.o: spmat.c spmat.h errors.h
	gcc $(FLAGS) -c spmat.c