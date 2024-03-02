#[allow(dead_code)]
#[derive(Debug)]
struct Config {
	name: String,
	has: Vec<String>,
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
			"has"=>&mut self.has,
			"findAny"=>&mut self.find_any,
			"exec"=>&mut self.exec,
			"env"=>&mut self.env,
			_=>{return Err(format!("Unknown name {name}"))}
		})
	}
	fn append<T>(&mut self, name: &str, values: T) -> Result<(), String>
		where T: Iterator<Item = String>
	{
		let var = self.get_setting(name)?;
		values.for_each(|value|var.push(value));
		Ok(())
	}

	fn verify(&self) -> Result<bool, String> {
		let has_files = self.has
			.iter()
			.map(std::path::Path::new)
			.map(std::path::Path::try_exists);
		let errors: Vec<_> = has_files
			.clone()
			.filter(Result::is_err)
			.map(Result::unwrap_err)
			.collect();
		if let Some(error) = errors.first() {
			return Err(error.to_string());
		}
		let has_files = has_files
			.map(Result::unwrap) // safe unwrap, no errors in iterator
			.fold(true, |every, current|every&&current);

		//TODO find one of find_any
		Ok(has_files)
	}

	fn execute(self) -> () {
		todo!()
	}
}

fn read_config(content: &str) -> Result<Config, String> {
	let mut lines = content.split("\n");
	let scope = lines.next().ok_or("Can't read scope setter")?;
	let scope = scope[..scope.len()-1].to_owned();

	let mut config = Config::new(scope);
	for line in lines {
		if line.is_empty() {continue}
		let (name, values) = line.split_once("=").ok_or("non-empty line doens't have '=' to set a variable")?;
		let values = values.split(":").map(str::to_owned);
		config.append(name, values)?;
	}

	Ok(config)
}

fn read_configs(content: &str) -> Result<Vec<Config>, String> {
	let mut configs = Vec::new();
	for scope in content.split("\n[") {
		if !scope.is_empty() {
			// first line may be '[' without '\n'
			let scope = scope.strip_prefix("[").unwrap_or(scope);
			configs.push(read_config(scope)?);
		}
	}
	Ok(configs)
}

fn main() -> Result<(), String> {
	let vars = std::env::vars();
	let home = vars
		.filter(|(name, _)| name == "HOME")
		.next()
		.map(|(_, value)| value)
		.ok_or("$HOME not defined!".to_owned())?;

	let configfile = format!("{home}/.config/runner.cfg");
	let content = std::fs::read_to_string(configfile).expect("can't read file");
	let configs = read_configs(&content)?;
	for config in configs {
		println!("{}: {}", &config.name, config.verify()?);
	}

	// TODO: parse agrs
	// --help
	// --config
	// --explain
	// --inspect
	//let args = std::env::args();

	Ok(())
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
