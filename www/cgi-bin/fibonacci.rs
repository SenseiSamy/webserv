use std::env;

fn fibonacci(n: usize) -> Option<u128> {
	let mut seq: Vec<u128> = Vec::new();
	seq.push(0);
	seq.push(1);

	for i in 2..n+1 {
		match seq[i - 1].checked_add(seq[i - 2]) {
			Some(res) => seq.push(res),
			None => return None,
		}
	}

	Some(seq[n])
}

fn main() {
	print!("Content-Type: text/plain\r\n\r\n");

	match env::var("QUERY_STRING") {
		Ok(val) => {
			for var in val.split('&') {
				if var.starts_with("n=") {
					let n = var[2..].to_string().parse::<usize>();
					match n {
						Ok(nb) => {
							match fibonacci(nb) {
								Some(val) => println!("F({}) = {}", nb, val),
								None => println!("Overflow !"),
							}
						},
						Err(_) => println!("Failed to convert the value of n"),
					}
					return;
				}
			}
			println!("Couldn't find the n var in QUERY_STRING");
		},
		Err(_) => println!("Couldn't find the QUERY_STRING vaiable in env"),
	}
}