use std::env;

fn fibonacci(n: usize) -> usize {
	let mut seq = Vec::new();
	seq.push(0);
	seq.push(1);

	for i in 2..n+1 {
		seq.push(seq[i - 1] + seq[i - 2]);
	}

	seq[n]
}

fn main() {
	print!("Content-Type: text/plain\r\n\r\n");

	match env::var("QUERY_STRING") {
		Ok(val) => {
			for var in val.split('&') {
				if var.starts_with("n=") {
					let n = var[2..].to_string().parse::<usize>();
					match n {
						Ok(nb) => println!("F({}) = {}", nb, fibonacci(nb)),
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