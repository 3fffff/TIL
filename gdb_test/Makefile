
default: main

main.o: main.cpp
	g++ -c -g main.cpp -o  main.o

main: main.o
	g++ main.o -o main

clean:
	-rm	-f	main.o
	-rm	-f	main