type LevResult = Result<(), u8>;

fn k_to_c(k: f64) -> f64 { k-273.15 }
fn f_to_c(f: f64) -> f64 { (f-32.0)/1.8}
fn c_to_k(c: f64) -> f64 { c+273.15 }
fn c_to_f(c: f64) -> f64 { c*1.8+32.0 }

fn print_vec(suffix: &str, unit: &str, to_x: Vec<Temperature>, conv: fn(f64) -> f64) {
	if to_x.is_empty() { return; }
	println!("to {}", unit);
	to_x.iter()
		.for_each(|f|{
			match f.temp {
				Some(temp)=>{
					println!("{} -> {:.3}{}", f.strform, conv(temp), suffix)
				},
				None=>{eprintln!("Can't parse \"{}\" into float", f.strform)}
			}
		});
}

struct Temperature {
	pub strform: String,
	pub temp: Option<f64>,
}

enum Unit {
	Fahrenheit,
	Kelvin,
	Celsius,
}

fn str_to_c(mut st: String) -> Option<f64> {
	let lastchar;
	match st.pop() {
		Some(chr) => lastchar = chr,
		None=> {return None;},
	};

	let temptype;
	if lastchar.is_digit(10) {
		temptype = Unit::Celsius;
		st.push(lastchar);
	} else {
		match lastchar {
			'f'|'F'=>temptype = Unit::Fahrenheit,
			'c'|'C'=>temptype = Unit::Celsius,
			'k'|'K'=>temptype = Unit::Kelvin,
			_=>{return None;}
		};
	}

	let ft = st.parse().unwrap();

	return Some(match temptype {
		Unit::Fahrenheit=>f_to_c(ft),
		Unit::Celsius=>ft,
		Unit::Kelvin=>k_to_c(ft),
	})
}

fn try_main(args: std::env::Args) -> LevResult {
	let mut to_k: Vec<Temperature> = Vec::new();
	let mut to_c: Vec<Temperature> = Vec::new();
	let mut to_f: Vec<Temperature> = Vec::new();
	let mut unit_conv = Unit::Celsius;

	args
		.filter(|arg| arg.len() > 0)
		.for_each(|arg|{
			match arg.to_lowercase().as_ref() {
				"-f" | "-°f" => { unit_conv = Unit::Fahrenheit; }
				"-k"         => { unit_conv = Unit::Kelvin; }
				"-c" | "-°c" => { unit_conv = Unit::Celsius; }
				_ => {
					match unit_conv {
						Unit::Fahrenheit => {to_f.push(Temperature{strform: arg.clone(), temp: str_to_c(arg)});}
						Unit::Celsius    => {to_c.push(Temperature{strform: arg.clone(), temp: str_to_c(arg)});}
						Unit::Kelvin     => {to_k.push(Temperature{strform: arg.clone(), temp: str_to_c(arg)});}
					}
				}
			}
		});

	print_vec("K", "Kelvin", to_k, c_to_k);
	print_vec("°F", "Fahrenheit", to_f, c_to_f);
	print_vec("°C", "Celsius", to_c, |c|c);

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

