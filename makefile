A: A.c
	gcc -o A -pthread -std=c99 A.c

B: B.c
	gcc -o B -pthread -std=c99 B.c

all: 
	gcc -o A -pthread -std=c99 A.c
	gcc -o B -pthread -std=c99 B.c

clean:
	rm A B
