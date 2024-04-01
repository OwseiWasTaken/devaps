#[derive(Debug)]
struct Config {
    name: String,
    has: Vec<Vec<String>>,
    find_any: Vec<String>,
    exec: Vec<String>,
    env: Vec<(String, String)>,
}

impl Config {
    fn new(name: String) -> Self {
        Config {
            name,
            has: Vec::new(),
            find_any: Vec::new(),
            exec: Vec::new(),
            env: Vec::new(),
        }
    }

    fn get_setting(&mut self, name: &str) -> Result<&mut Vec<String>, String> {
        Ok(match name {
            "findAny" => &mut self.find_any,
            "exec" => &mut self.exec,
            _ => return Err(format!("Unknown name {name}")),
        })
    }
    fn append(&mut self, name: &str, values: &str) -> Result<(), String> {
        if name == "env" {
            let (env_name, env_val) = values.split_once(' ').ok_or(format!(
                "env `{values}` definition must contain space to split name and value"
            ))?;
            self.env.push((env_name.to_owned(), env_val.to_owned()));
            return Ok(());
        }

        let values: Vec<String> = values.split(':').map(str::to_owned).collect();
        if name == "has" {
            self.has.push(values);
        } else {
            let var = self.get_setting(name)?;
            values.into_iter().for_each(|value| var.push(value));
        }
        Ok(())
    }

    fn find(&self) -> Result<String, String> {
        //self.find_any   // Vec<String>
        //    .map(.mper) // Iter<Result<Path, String>>
        //    .filter     // Iter<Result<Path, String>>
        //    .next       // Option<Result<Path, String>>
        //    .ok_or      // Result<Result<Path, String>, String>

        self.find_any
            .iter()
            .try_any_which(|file| {
                std::path::Path::new(file)
                    .try_exists()
                    .map_err(|e| e.to_string())
            })
            .map(|opt_str|{
                opt_str.cloned().ok_or("Can't find single findAny file".to_owned())
            })?
    }

    //TODO verify even if findAny doens't work
    fn verify(&self, verbose: bool) -> Result<bool, String> {
        //TODO print _after_ check
        //println!("{}{{", &self.name);
        for files in &self.has {
            'found: {
                for file in files {
                    let exists = std::path::Path::new(&file)
                        .try_exists()
                        .map_err(|e| e.to_string())?;
                    if exists {
                        if verbose {
                            //println!("\t\x1b[32m✓ {}\x1b[0m", &file);
                        }
                        break 'found;
                    }
                }
                if verbose {
                    //println!("\t\x1b[31mX {}\x1b[0m", files.join(" : "));
                }
                return Ok(false);
            }
        }
        if !self.find_any.is_empty() {
            self.find()?;
        }

        //println!("}}\x1b[32m✓ {}\x1b[0m", &self.name);
        Ok(true)
    }

    // TODO print command beeing executed
    fn execute(self) -> Result<(), String> {
        use std::process::Command;

        let file = if self.find_any.is_empty() {
            None
        } else {
            Some(self.find()?)
        };

        for cmd in self.exec.into_iter() {
            let mut child = Command::new("/bin/sh");
            child.arg("-c");
            child.arg(&cmd);
            if let Some(ref found) = file {
                child.env("found", found);
            }
            for (name, value) in self.env.iter() {
                child.env(name, value);
            }
            child
                .spawn()
                .map_err(|e| format!("can't spawn {}: {}", cmd, e))?
                .wait()
                .map_err(|e| format!("can't execute {}: {}", cmd, e))?;
        }

        Ok(())
    }
}

impl std::str::FromStr for Config {
    type Err = String;

    fn from_str(content: &str) -> Result<Config, String> {
        let mut lines = content.split('\n');
        let scope = lines.next().ok_or("Can't read scope setter")?;
        let scope = scope[..scope.len() - 1].to_owned();

        let mut config = Config::new(scope);
        for line in lines {
            if line.is_empty() {
                continue;
            }
            let (name, values) = line
                .split_once('=')
                .ok_or("non-empty line doens't have '=' to set a variable")?;
            config.append(name, values)?;
        }

        Ok(config)
    }
}

fn read_configs(content: &str) -> Result<Vec<Config>, String> {
    content
        .split("\n[")
        .filter(|e| !e.is_empty())
        .map(|scope| scope.strip_prefix('[').unwrap_or(scope))
        .map(|scope| scope.parse())
        .try_fold(
            Vec::new(),
            |mut vec: Vec<Config>, may_cfg| {
                vec.push(may_cfg?);
                Ok(vec)
            },
        )
}

fn get_home_dir() -> Result<String, String> {
    std::env::var("HOME").map_err(|e| format!("can't read $HOME: {e}"))
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
            "-h" | "--help" => help = true,
            "-v" | "--verify" => verify = true,
            "-V" | "-vv" | "--verbose_verify" => verbose = true,
            "-c" | "--config" => {
                //TODO
                configfile = Some(args.next().ok_or("--config must provide config file")?);
            }
            "-" => {}
            _ => {
                println!("{}", display_help(proc_name));
                return Err(format!("Unknown argument '{}'", arg));
            }
        };
    }

    if help {
        println!("{}", display_help(proc_name));
        return Ok(());
    }

    let configfile = match configfile {
        None => {
            let home = get_home_dir()?;
            format!("{home}/.config/runner.cfg")
        }
        Some(file) => file,
    };
    let content = std::fs::read_to_string(&configfile)
        .map_err(|e| format!("can't read file {configfile}: {e}"))?;
    let configs = read_configs(&content)?;

    let scope = configs
        .into_iter()
        .try_any_which(|config| config.verify(verbose))?
        .ok_or(format!("Can't find single run scope from {configfile}"))?;

    if verify || verbose {
        println!("Found scope {}", scope.name);
    } else {
        scope.execute()?;
    }

    Ok(())
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

trait All: IntoIterator {
    fn try_any_which<F, E>(self, fnc: F) -> Result<Option<Self::Item>, E>
    where
        Self: Sized,
        F: Fn(&Self::Item) -> Result<bool, E>,
    {
        for item in self.into_iter() {
            if fnc(&item)? {
                return Ok(Some(item));
            }
        }
        Ok(None)
    }
}

impl<I: Iterator> All for I {}
