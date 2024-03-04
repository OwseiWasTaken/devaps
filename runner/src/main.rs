#[allow(dead_code)]
#[derive(Debug)]
struct Config {
	name: String,
	has: Vec<Vec<String>>,
	find_any: Vec<String>,
	exec: Vec<String>,
	env: Vec<String>,
}
//TODO implement hasAny

impl Config {
	fn new(name: String) -> Self {
		Config{
			name,
			has: Vec::new(),
			find_any: Vec::new(),
			exec: Vec::new(),
			env: Vec::new(),
		}
	}

	fn get_setting(&mut self, name: &str) -> Result<&mut Vec<String>, String> {
		Ok(match name {
			"findAny"=>&mut self.find_any,
			"exec"=>&mut self.exec,
			"env"=>&mut self.env,
			_=>{return Err(format!("Unknown name {name}"))}
		})
	}
	fn append(&mut self, name: &str, values: Vec<String>) -> Result<(), String>
	{
		if name == "has" {
			self.has.push(values);
		} else {
			let var = self.get_setting(name)?;
			values.into_iter().for_each(|value|var.push(value));
		}
		Ok(())
	}

	fn verify_verbose(&self) -> Result<bool, String> {
		todo!()
	}

	fn verify(&self, verbose: bool) -> Result<bool, String> {
		//TODO print _after_ check
		//println!("{}{{", &self.name);
		for files in &self.has {
			'found: {
				for file in files {
					let exists  = std::path::Path::new(&file)
						.try_exists()
						.map_err(|e|e.to_string())?;
					if exists {
						if verbose {
							//println!("\t\x1b[32m✓ {}\x1b[0m", &file);
						}
						break 'found
					}
				}
				if verbose {
					//println!("\t\x1b[31mX {}\x1b[0m", files.join(" : "));
				}
				return Ok(false)
			}
		}

		//println!("}}\x1b[32m✓ {}\x1b[0m", &self.name);
		Ok(true)
	}

	fn execute(self) -> () {
		todo!()
	}
}

impl std::str::FromStr for Config {
	type Err = String;

	fn from_str(content: &str) -> Result<Config, String> {
		let mut lines = content.split("\n");
		let scope = lines.next().ok_or("Can't read scope setter")?;
		let scope = scope[..scope.len()-1].to_owned();

		let mut config = Config::new(scope);
		for line in lines {
			if line.is_empty() {continue}
			let (name, values) = line.split_once("=").ok_or("non-empty line doens't have '=' to set a variable")?;
			let values = values.split(":").map(str::to_owned);
			config.append(name, values.collect::<Vec<_>>())?;
		}

		Ok(config)
	}
}

fn read_configs(content: &str) -> Result<Vec<Config>, String> {
	content.split("\n[")
		.filter(|e|!e.is_empty())
		.map(|scope|scope.strip_prefix("[").unwrap_or(scope))
		.map(|scope|scope.parse())
		.fold(Ok(Vec::new()), |may_vec: Result<Vec<Config>, String>, may_cfg|{
			match may_cfg {
				Ok(cfg)=>may_vec.map(|mut vec|{vec.push(cfg);vec}),
				Err(e)=>Err(e)
			}
		})
}

fn get_home_dir() -> Result<String, String> {
	std::env::vars()
		.filter(|(name, _value)| name == "HOME")
		.next()
		.map(|(_name, value)| value)
		.ok_or("$HOME not defined!".to_owned())
}

fn main() -> Result<(), String> {

	let mut configfile: Option<String> = None;
	let mut verify = false;
	let mut verbose = false;
	let mut help = false;

	let mut args = std::env::args();
	let proc_name = args.next();
	while let Some(arg) = args.next() {
		match arg.as_str() {
			"-h"|"--help"=>help = true,
			"-v"|"--verify"=>verify = true,
			"-V"|"-vv"|"--verbose_verify"=>verbose = true,
			"-c"|"--config"=>{
				//TODO
				configfile = Some(args.next().ok_or("--config must provide config file")?);
			}
			"-"=>{},
			_=>{
				println!("{}", display_help(proc_name));
				return Err(format!("Unknown argument '{}'", arg));
			},
		};
	}

	if help {
		println!("{}", display_help(proc_name));
		return Ok(())
	}

	let configfile = match configfile {
		None=>{
			let home = get_home_dir()?;
			format!("{home}/.config/runner.cfg")
		},
		Some(file)=>file,
	};
	let content = std::fs::read_to_string(&configfile).expect("can't read file");
	let configs = read_configs(&content)?;

	let scope = verify_all(configs, verbose)?
		.ok_or(format!("Can't find single run scope from {configfile}"))?;
	println!("{scope:?}");

	Ok(())
}

fn verify_all(configs: Vec<Config>, verbose: bool) -> Result<Option<Config>, String> {
	for config in configs {
		if config.verify(verbose)? {
			return Ok(Some(config))
		}
	}
	Ok(None)
}

fn display_help(progname: Option<String>) -> String {
	format!( "usage $ {}\n\
		\t[-v | --verify] prints what scope \x1b[3mwould\x1b[0m run\n\
		\t[-V | -vv | --verbose_verify] prints what scope would run, \x1b[3mand why\x1b[0m\n\
		\t[-h | --help] prints this\n\
		\t[-c \x1b[3mconfigfile\x1b[0m | --config \x1b[3mconfigfile\x1b[0m] use \x1b[3mconfigfile\x1b[0m instead of $HOME/.config.runner.cfg",
		progname.unwrap_or("run".to_owned())
	)
}

// TODO execute shell commands with $file as env var for first file found from the findAny vec
//		let child = std::process::Command::new("/bin/sh")
//			.env("found", self.Found)
//			.arg("-c")
//			.arg(&self.command)
//			// inherit from parent or pipe?
//			//.stdin(Stdio::piped())
//			//.stdout(Stdio::piped())
//			//.stderr(Stdio::piped())
//			.spawn()
//			.expect("Can't execute command");
