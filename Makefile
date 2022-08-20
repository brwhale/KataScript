build:
	g++ -std=c++20 src/Interpreter/Main.cpp -o KataScript

install:
	sudo mv KataScript /usr/local/bin/KataScript
	KataScript samples/demo.ks
