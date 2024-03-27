use serde::Deserialize;

#[derive(Deserialize, Debug, Clone)]
struct Edit {
    from: String,
    to: String,
    color: Option<String>,
}

#[derive(Debug)]
struct Dir {
    path: String,
    color: Option<String>,
}

impl Dir {
    fn into_string(self) -> String {
        if let Some(color) = self.color {
            color + &self.path
        } else {
            self.path
        }
    }
    fn new(path: String) -> Dir {
        Dir { path, color: None }
    }
    fn edit(self, cfg: Edit) -> Dir {
        if let Some(path) = self.path.strip_prefix(&cfg.from) {
            Dir {
                path: cfg.to + path,
                color: cfg.color.or(self.color),
            }
        } else {
            self
        }
    }
}

fn get_config() -> Result<Vec<Edit>, &'static str> {
    let file = std::fs::read_to_string("~/.config/fpwd");
    match file {
        Ok(content) => serde_lexpr::from_str(&content).or(Err("Parse error in ~/.config/fpwd")),
        Err(_) => {
            let home = std::env::var("HOME").or(Err("Can't read $HOME"))?;
            let tilde = "~".to_owned();
            let color = "\\e[0m".to_owned();
            Ok(vec![Edit {
                from: home,
                to: tilde,
                color: Some(color),
            }])
        }
    }
}

fn fancy_unwrap<T>(e: Result<T, &'static str>) -> T {
    match e {
        Ok(c) => c,
        Err(reason) => {
            eprintln!("{reason}");
            std::process::exit(1)
        }
    }
}

fn main() {
    let config = fancy_unwrap(get_config());
    let pwd = fancy_unwrap(std::env::var("PWD").or(Err("Can't find $PWD env variable")));
    let dir = Dir::new(pwd);
    let path = config
        .into_iter()
        .fold(dir, Dir::edit)
        .into_string()
        .replace("\\e", "\x1b");
    print!("{path}");
}
