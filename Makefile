build:
	g++ -std=c++2a src/Interpreter/Main.cpp -o KataScript

install:
	sudo mv KataScript /usr/local/bin/KataScript
	KataScript samples/demo.ks
