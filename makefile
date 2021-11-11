all: change clean load print query

change: change.c header.h
	gcc -Wall -o change change.c header.c
load: load.c header.h
	gcc -Wall -o load load.c header.c
print: print.c header.h
	gcc -Wall -o print print.c header.c
query: query.c header.h
	gcc -Wall -o query query.c header.c
clean: clean.c header.h
	gcc -Wall -o clean clean.c header.c
clear:
	rm -f change  clean  load  print  query  *~ core
