class test {
	var x = 1;
	var y = "KataScript interpreter sucessfully installed!";
	func test() {
		x = 2;
	}
}

a = test();

if (a.x == 2) {
	print(a.y);
}
