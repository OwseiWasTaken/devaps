enum UnitIndicator {
	Fahrenheit,
	Kelvin,
	Celsius,
}

enum Unit {
	Fahrenheit(f64),
	Kelvin(f64),
	Celsius(f64),
}

struct Temperature {
	pub value: Unit,
}

//TODO: implement to_<Unit> as Unit's method instead of using Temperature as wrapper
impl Temperature {
	fn in_u(self, u: &UnitIndicator) -> f64 {
		match u {
			UnitIndicator::Fahrenheit => self.in_f(),
			UnitIndicator::Kelvin => self.in_k(),
			UnitIndicator::Celsius => self.in_c(),
		}
	}

	fn in_c(self) -> f64 {
		match self.value {
			Unit::Fahrenheit(f)=>f-32.0/1.8,
			Unit::Kelvin(k)=>k+273.15,
			Unit::Celsius(c)=>c,
		}
	}
	fn in_k(self) -> f64 {
		match self.value {
			Unit::Fahrenheit(f)=>f-32.0/1.8-273.15,
			Unit::Kelvin(k)=>k,
			Unit::Celsius(c)=>c-273.15,
		}
	}
	fn in_f(self) -> f64 {
		match self.value {
			Unit::Fahrenheit(f)=>f,
			Unit::Kelvin(k)=>k*1.8-241.15,
			Unit::Celsius(c)=>c*1.8+32.0,
		}
	}
}

fn str_to_temp(st: &str) -> Result<Temperature, String> {
	let last_chr = st.chars().last().ok_or_else(|| "Can't get last char")?;
	let unit = {
		if last_chr.is_digit(10) {
			'c'
		} else {
			last_chr
		}
	};
	let f_str: String = st.chars().take_while(|c|c.is_digit(10)).collect();
	let f:f64 = f_str.parse().or(Err("Can't parse number"))?;
	return Ok(Temperature{value: match unit {
		'f' | 'F' => Unit::Fahrenheit(f),
		'k' | 'K' => Unit::Kelvin(f),
		'c' | 'C' => Unit::Celsius(f),
		_ => {return Err("No such unit".to_string())}
	}})
}

fn try_main(args: std::env::Args) -> Result<(), u8> {
	let mut indic = UnitIndicator::Celsius;
	for arg in args.filter(|arg| arg.len() > 0) {
		if arg == "-f" { indic = UnitIndicator::Fahrenheit }
		else if arg == "-k" { indic = UnitIndicator::Kelvin }
		else if arg == "-c" { indic = UnitIndicator::Celsius }
		else {
			let temp = match str_to_temp(&arg) {
				Ok(temp)=>temp,
				Err(cause)=>{
					eprintln!("Error on {}: {}", arg, cause);
					return Err(1);
				}
			};
			let temp_in_u = temp.in_u(&indic);
			println!("{} -> {}", arg, temp_in_u);
		}
	}
	Ok(())
}

fn main() {
	let mut args = std::env::args();
	let progname = args.next().unwrap();
	match try_main(args) {
		Err(ecode)=>{
			println!("Usage: {progname} \"target\" [\"compare\" ...]");
			std::process::exit(ecode as i32);
		},
		Ok(_)=>{}
	}
}

