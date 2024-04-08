use serde::Deserialize;

#[derive(Deserialize, Debug, Clone)]
pub struct Edit {
    from: String,
    to: String,
    color: Option<String>,
}

#[derive(Debug)]
pub struct Dir {
    path: String,
    color: Option<String>,
}

//TODO imple Dir::edit for &mut self
//so I don't need to clone Edit
//on every iteration
impl Dir {
    pub fn into_string(self) -> String {
        if let Some(color) = self.color {
            color + &self.path
        } else {
            self.path
        }
    }
    pub fn new(path: String) -> Dir {
        Dir { path, color: None }
    }
    pub fn edit(self, cfg: Edit) -> Dir {
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

pub fn get_config() -> Result<Vec<Edit>, &'static str> {
    let home = std::env::var("HOME").or(Err("Can't read $HOME"))?;
    let filename = std::env::var("FPWDRS_CONFIG").or(Ok(home.clone()+"/.config/fpwd.lsp"))?;
    let file = std::fs::read_to_string(filename);
    match file {
        Ok(content) => serde_lexpr::from_str(&content).or(Err("Parse error in ~/.config/fpwd.lsp")),
        Err(_) => {
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

pub fn fancy_unwrap<T>(e: Result<T, &'static str>) -> T {
    match e {
        Ok(c) => c,
        Err(reason) => {
            eprintln!("pwd-rs ERROR: {reason}");
            std::process::exit(1)
        }
    }
}

