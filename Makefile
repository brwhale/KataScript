build:
	g++-9 -std=c++2a src/Interpreter/Main.cpp -o KataScript

install:
	sudo mv KataScript /usr/local/bin/KataScript
