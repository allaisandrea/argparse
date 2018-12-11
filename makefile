bin/test_argparse: src/argparse.h src/test_argparse.cpp
	g++ -std=c++11 -Wall -Wextra -Werror -pedantic \
	-o bin/test_argparse src/test_argparse.cpp \
	-pthread -lgtest -lgtest_main
clean:
	rm bin/test_argparse
lint:
	clang-format -i -style=file src/argparse.h
	clang-format -i -style=file src/test_argparse.cpp
