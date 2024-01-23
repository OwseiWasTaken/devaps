//TODO: could use enum {single(Option<String>), plural(Vec<String>)}
//TODO: could make "--" take N args, "-" take 1 arg, "" take 0 args

// keys starting with "--" can have multiple values, the rest can only have one value
// special key "" always used
// special key "" doesn't start with '--' but it's also not single
use std::collections::HashMap;
fn parse_args(args: std::env::Args, requests: &[&str]) -> HashMap<String, Vec<String>> {
	let mut arg_map: HashMap<String, Vec<String>> = HashMap::new();
	let mut arg_section = "".to_string();
	let mut single = false;
	arg_map.insert( arg_section.to_string(), Vec::new() );

	for arg in args {
		if requests.contains(&arg.as_str()) {
			arg_section = arg.clone();
			arg_map.insert( arg_section.to_string(), Vec::new() );
			single = !arg.starts_with("--");
		} else {
			arg_map
				.get_mut(&arg_section)
				.unwrap()
				.push(arg);
			if single {
				arg_section = "".to_string();
				single = false;
			}
		}
	}

	arg_map
}

fn try_main(args: std::env::Args) -> Result<(), u8> {
	let mp = parse_args(
		args, &[
			"--out",
			"-l",
			"-o",
			"-O3",
		],
	);

	println!("{:?}", mp);

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

