fn dtc(arg: String, digits: Vec<u32>, piped: bool) {
	if piped {
		print!("{arg}: ");
	}
	for digit in digits {
		print!("{} ", char::from_u32(digit).unwrap());
	}
	print!("\n");
}

fn ctd(word: String, piped: bool) {
	let chars = word.chars();
	if piped {
		chars.for_each(|c|{ println!("{}", c as u32) })
	} else {
		chars.for_each(|c|{ println!("{c}: {}", c as u32) })
	}
}

//TODO: read from stdin
fn main() -> Result<(), String> {
	let mut args: Vec<String> = std::env::args().skip(1).collect();

	// piped
	let piped = args.first() == Some(&"-1".to_owned());
	// remove "-1" arg
	if piped {args.drain(0..1);}

	for arg in args {
		if arg.chars().all(|c|c.is_digit(10)||" ,".contains(c)) {
			let digits = arg
				.split_terminator(&[' ', ','])
				.map(|dgt| dgt.parse().unwrap() )
				.collect();
			dtc(arg, digits, piped);
		} else {
			ctd(arg, piped);
		}
	}

	Ok(())
}

