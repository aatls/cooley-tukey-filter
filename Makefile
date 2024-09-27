CPPFLAGS = -std=c++20 -Wall -Wextra -pedantic -Wshadow -march=native

HEADERS = src/include/fft.hpp src/include/wavtools.hpp

main: src/main.cpp $(HEADERS)
	g++ $(CPPFLAGS) -Ofast -o ctf src/main.cpp

clean:
	rm -f ctf