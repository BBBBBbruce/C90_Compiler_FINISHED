CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter -Wno-deprecated-register -Wno-unused-function
CPPFLAGS += -I include

all : bin/c_compiler

c : bin/comp

src/Translator_parser.tab.cpp src/Translator_parser.tab.hpp : src/Translator_parser.y
	bison -p zz -v -d src/Translator_parser.y -o src/Translator_parser.tab.cpp

src/Translator_lexer.yy.cpp : src/Translator_lexer.flex src/Translator_parser.tab.hpp
	flex -P zz -o src/Translator_lexer.yy.cpp  src/Translator_lexer.flex

src/MIPS_parser.tab.cpp src/MIPS_parser.tab.hpp : src/MIPS_parser.y
	bison -v -d src/MIPS_parser.y -o src/MIPS_parser.tab.cpp

src/MIPS_lexer.yy.cpp : src/MIPS_lexer.flex src/MIPS_parser.tab.hpp
	flex -o src/MIPS_lexer.yy.cpp  src/MIPS_lexer.flex

bin/c_compiler : src/c_main.o src/Translator_parser.tab.o src/Translator_lexer.yy.o src/Translator_parser.tab.o src/MIPS_parser.tab.o src/MIPS_lexer.yy.o src/MIPS_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

bin/comp: src/comp.cpp src/MIPS_parser.tab.o src/MIPS_lexer.yy.o src/MIPS_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/comp $^

clean :
	rm src/*.o
	rm src/*.output
	rm src/*.hpp
	rm src/*.tab.cpp
	rm src/*.yy.cpp
	rm -r bin/*
	rm -r running_data
