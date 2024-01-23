enum Unit {
	Fahrenheit,
	Kelvin,
	Celsius,
}

struct Temperature {
	value: f64,
	unit: Unit,
}

impl Unit {
	fn as_str(self) -> String {
		match self {
			Unit::Fahrenheit => "°f",
			Unit::Kelvin => "K",
			Unit::Celsius => "°C",
		}.to_string()
	}
}

impl Temperature {
	fn to_string(self) -> String {
		[self.value.to_string(), self.unit.as_str()].join("")
	}
	fn new(value: f64, unit: Unit) -> Temperature {
		Temperature{value, unit}
	}
	fn in_u(self, unit: &Unit) -> Temperature {
		match unit {
			Unit::Fahrenheit => self.in_f(),
			Unit::Kelvin => self.in_k(),
			Unit::Celsius => self.in_c(),
		}
	}

	fn in_f(self) -> Temperature {
		Temperature{
			value: match self.unit {
				Unit::Fahrenheit=>self.value,
				Unit::Kelvin=>self.value*1.8-241.15,
				Unit::Celsius=>self.value*1.8+32.0,
			},
			unit: Unit::Fahrenheit,
		}
	}

	fn in_k(self) -> Temperature {
		Temperature{
			value: match self.unit {
				Unit::Fahrenheit=>self.value-32.0/1.8-273.15,
				Unit::Kelvin=>self.value,
				Unit::Celsius=>self.value-273.15,
			},
			unit: Unit::Kelvin,
		}
	}

	fn in_c(self) -> Temperature {
		Temperature{
			value: match self.unit {
				Unit::Fahrenheit=>self.value-32.0/1.8,
				Unit::Kelvin=>self.value+273.15,
				Unit::Celsius=>self.value,
			},
			unit: Unit::Celsius,
		}
	}
}

fn str_to_temp(st: &str) -> Result<Temperature, String> {
	let last_chr = st.chars().last().ok_or_else(|| "Can't get last char")?;
	let unit_str = {
		if last_chr.is_digit(10) {
			'c'
		} else {
			last_chr
		}
	};

	let unit = match unit_str {
		'f' | 'F' => Unit::Fahrenheit,
		'k' | 'K' => Unit::Kelvin,
		'c' | 'C' => Unit::Celsius,
		_ => {return Err("No such unit".to_string())}
	};

	let f_str: String = st.chars().take_while(|c|c.is_digit(10)).collect();
	let f:f64 = f_str.parse().or(Err("Can't parse number"))?;

	Ok(Temperature::new(f, unit))
}

fn try_main(args: std::env::Args) -> Result<(), u8> {
	let mut indic = Unit::Celsius;
	for arg in args.filter(|arg| arg.len() > 0) {
		if arg == "-f" { indic = Unit::Fahrenheit }
		else if arg == "-k" { indic = Unit::Kelvin }
		else if arg == "-c" { indic = Unit::Celsius }
		else {
			let temp = match str_to_temp(&arg) {
				Ok(temp)=>temp,
				Err(cause)=>{
					eprintln!("Error on {}: {}", arg, cause);
					return Err(1);
				}
			};
			let temp_in_u = temp.in_u(&indic);
			println!("{} -> {}", arg, temp_in_u.to_string());
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

