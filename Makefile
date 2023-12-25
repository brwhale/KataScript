build:
	g++ --version
	g++ -std=c++20 -O3 src/Interpreter/Main.cpp -o KataScript

install:
	sudo mv KataScript /usr/local/bin/KataScript
	KataScript samples/demo.ks
