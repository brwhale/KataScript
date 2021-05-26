

// The pre compiler recognizes this but the main compiler doesn't
// import ("Foundation");

class test {
	var x = 1;
	var y = "KataScript interpreter successfully installed!";
	func test() {
		x = 2;
	}
}

a = test();

if (a.x == 2) {
	print(a.y);
}
