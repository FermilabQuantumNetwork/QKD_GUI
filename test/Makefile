all: swabian_test

swabian_test: swabian_test.c ../source/swabian.cpp ../source/swabian.h
	g++ -o swabian_test ../source/swabian.cpp swabian_test.c -I ../include -I ../source -L ../lib -lTimeTagger -lokFrontPanel

clean:
	rm *.o swabian_test
