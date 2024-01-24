
fn dtc(arg: String, digits: Vec<u32>) {
	print!("{arg}: ");
	for digit in digits {
		print!("{} ", char::from_u32(digit).unwrap());
	}
	print!("\n");
}

fn ctd(word: String) {
	word.chars()
		.for_each(|c|{
		println!("{c}: {}", c as u64)
	})
}

fn main() -> Result<(), String> {
	let mut args = std::env::args();
	args.next();
	for arg in args {
		if arg.chars().all(|c|c.is_digit(10)||" ,".contains(c)) {
			let digits = arg
				.split_terminator(&[' ', ','])
				.map(|dgt| dgt.parse().unwrap() )
				.collect();
			dtc(arg, digits);
		} else {
			ctd(arg);
		}
	}

	Ok(())
}

