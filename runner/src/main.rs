#[allow(dead_code)]
#[derive(Debug)]
struct Config {
	name: String,
	has: Vec<String>,
	find_any: Vec<String>,
	exec: Vec<String>,
}

impl Config {
	fn new(name: String) -> Self {
		Config{
			name,
			has: Vec::new(),
			find_any: Vec::new(),
			exec: Vec::new(),
		}
	}

	fn append_has(&mut self, has: String) { self.has.push(has) }
	fn append_find(&mut self, find: String) { self.find_any.push(find) }
	fn append_exec(&mut self, exec: String) { self.exec.push(exec) }
}

fn read_config(content: &str) -> Result<Config, String> {
	todo!("read_config")
}

fn read_configs(content: &str) -> Result<Vec<Config>, String> {
	let scopes = content.split("\n[");
	scopes.map(read_configs)
	read_configs
	todo!("read_configs")
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
	let config = read_configs(&content)?;

	// TODO: parse agrs
	// --help
	// --config
	// --explain
	// --inspect
	//let args = std::env::args();
	println!("{config:?}");

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
