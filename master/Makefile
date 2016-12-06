EXEC	= main
CXX	= gcc
MAIN	= main.c ./pdi/source/regiao.c ./pdi/source/deteccao.c ./pdi/source/pixel.c ./pdi/source/aquisition.c ./pdi/source/processing.c ./pdi/source/escrita.c
OPTIONS	= -O0 -Wall -ggdb
LIBS	= -lX11 -lm -L/usr/local/lib -lgsl -lgslcblas -Wa,-ahlms=myfile.lst

main:	clean
	$(CXX) $(OPTIONS) $(MAIN) $(LIBS) -o $(EXEC)

clean:
	clear
	rm -f include/*~
	rm -f lib/*~
	rm -f *~

