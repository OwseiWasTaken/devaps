use std::path::{Path, PathBuf};
#[derive(Debug)]
pub enum TemplrsError {
	UnspecifiedName,
	EnvError,
	NonUtf8Path(std::ffi::OsString),
}

//TODO: set suffix .sh for linux, .ps1 for windows

impl std::fmt::Display for TemplrsError {
	fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
		use TemplrsError::*;
		write!(f, "Templ-rs encoutered an error: ")?;
		match self {
			UnspecifiedName=>write!(f, "Must specify template name"),
			EnvError=>write!(f, "can't find neither $HOME nor $TMPLRS_DIR in env"),
			NonUtf8Path(ostr)=>write!(f, "Can't parse '{ostr:?}' as utf-8"),
		}
	}
}

use std::error::Error;
impl Error for TemplrsError {}

pub struct ToRun {
	name: PathBuf,
	args: Vec<String>,
}

impl ToRun {
	pub fn new(name: PathBuf, args: Vec<String>) -> Self {
		ToRun{ name, args }
	}
	pub fn run(self) -> anyhow::Result<()> {
		std::process::Command::new(EXEC)
			.arg(self.name)
			.args(self.args)
			.spawn()?
			.wait()?;
		Ok(())
	}
}

pub const EXEC: &str = "bash";
pub const EXT: &str = ".sh";
pub fn try_gather(mut cli_args: std::env::Args) -> anyhow::Result<ToRun> {
	let mut name = cli_args
		.next()
		.ok_or(TemplrsError::UnspecifiedName)?;
	name.push_str(EXT); // add .sh or .ps1

	let args: Vec<String> = cli_args.collect();
	let config_dir = get_tmpl_dir()?;

	let name = Path::new(&config_dir).join(Path::new(&name));

	Ok(ToRun::new(name, args))
}

fn get_tmpl_dir() -> anyhow::Result<String> {
	use std::env;
	env::var("TMPLRS_DIR").or_else(|_|{
		let home = env::var("HOME").or(Err(TemplrsError::EnvError))?;
		let home = Path::new(&home);
		home.join(Path::new("Templates/tmpl-rs/"))
			.into_os_string()
			.into_string()
			.map_err(TemplrsError::NonUtf8Path)
			.map_err(|a|a.into()) // TemplrsError -> std::error::Error
	})
}

