EXEC	= main
CXX	= gcc
MAIN	= main.c ./pdi/source/regiao.c ./pdi/source/deteccao.c ./pdi/source/pixel.c
OPTIONS	= -O3 -Wall -ggdb
LIBS	= -lX11 -lm

main:	clean
	$(CXX) $(OPTIONS) $(MAIN) $(LIBS) -o $(EXEC)

clean:
	clear
	rm -f include/*~
	rm -f lib/*~
	rm -f *~

