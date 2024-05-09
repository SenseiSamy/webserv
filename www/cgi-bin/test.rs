fn main() {
	println!("Content-Type: text/plain\n");
	for (key, value) in std::env::vars() {
		println!("{key}={value}");
	}
}