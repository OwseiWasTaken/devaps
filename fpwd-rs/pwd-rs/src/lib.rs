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

#[derive(Debug)]
pub struct BDir<'b> {
    path: String,
    color: Option<&'b str>,
}

impl<'b> BDir<'b> {
    pub fn into_string(self) -> String {
        if let Some(color) = self.color {
            color.to_owned() + &self.path
        } else {
            self.path
        }.replace("\\e", "\x1b")
    }
    pub fn new(path: String) -> BDir<'b> {
        BDir {
            path, color: None,
        }
    }
    pub fn edit(self, cfg: &'b Edit) -> BDir<'b> {
        if let Some(path) = self.path.strip_prefix(&cfg.from) {
            let color: Option<&str> = match &cfg.color {
                Some(col)=>Some(col.as_ref()),
                None=>self.color,
            };
            BDir {
                path: (cfg.to.clone() + path),
                color,
            }
        } else {
            self
        }
    }
}

impl Dir {
    pub fn into_string(self) -> String {
        if let Some(color) = self.color {
            color + &self.path
        } else {
            self.path
        }.replace("\\e", "\x1b")
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
    pub fn borrow_edit(self, cfg: &Edit) -> Dir {
        if let Some(path) = self.path.strip_prefix(&cfg.from) {
            Dir {
                path: cfg.to.clone() + path,
                color: cfg.color.clone().or(self.color),
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

