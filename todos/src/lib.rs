use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(thiserror::Error, Debug)]
pub enum TodosError {
    #[error(transparent)]
    File(#[from] std::io::Error),
    #[error(transparent)]
    TOMLSER(#[from] toml::ser::Error),
    #[error(transparent)]
    TOMLDE(#[from] toml::de::Error),
    #[error(transparent)]
    RegexSyntax(#[from] regex::Error),

    #[error("File missing extension {0}")]
    NoExt(String),
    #[error("Missing regex for extension .{0}")]
    NoExtReg(String),
    #[error("Malformed command, {0} needs more values")]
    MissingArgs(String),
    #[error("Malformed command command previous to '{0}' didn't need this value")]
    ExtraArgs(String),
    #[error("No such command {0}")]
    UnknownCommand(String),
    #[error("Can't find neither $HOME nor $TODOS_RS env vars")]
    MissingEnv,
    #[error("Can't find neither $HOME nor $TODOS_RS_SCANCONF env vars for TODOS scanner")]
    MissingEnvConfig,
    #[error("Can't read scanner config file {0}")]
    Config(String),
}

pub fn get_file_path() -> Result<String, TodosError> {
    std::env::var("TODOS_RS")
        .or(std::env::var("HOME").map(|path| path + "/.todos.toml"))
        .or(Err(TodosError::MissingEnv))
}

pub fn get_conf_file_path() -> Result<String, TodosError> {
    std::env::var("TODOS_RS_SCANCONF")
        .or(std::env::var("HOME").map(|path| path + "/.config/.todos_scanner.toml"))
        .or(Err(TodosError::MissingEnv))
}

pub struct Scanner {
    config: HashMap<String, regex::Regex>,
}

impl Scanner {
    pub fn new() -> Result<Scanner, TodosError> {
        let path = get_conf_file_path()?;
        let conf = std::fs::read_to_string(&path).or(Err(TodosError::Config(path)))?;
        let conf: HashMap<String, String> = toml::from_str(&conf)?;
        let mut compiled = HashMap::new();
        for (ext, reg) in conf {
            compiled.insert(ext, regex::Regex::new(&reg)?);
        }
        Ok(Scanner { config: compiled })
    }
    fn get_ext(name: &str) -> Option<&str> {
        Some(name.rsplit_once(".")?.1)
    }
    fn get<'a>(&'a self, ext: &str) -> Option<&'a regex::Regex> {
        self.config.get(ext)
    }
    fn find_all(&self, file_name: &str, content: String) -> Result<Vec<String>, TodosError> {
        use TodosError::*;
        let ext = Scanner::get_ext(file_name).ok_or(NoExt(file_name.to_owned()))?;
        let rgx = self.get(ext).ok_or(NoExtReg(ext.to_owned()))?;
        Ok(rgx
            .find_iter(&content)
            .map(|m| String::from(m.as_str()))
            .collect())
    }
}

#[derive(Debug)]
pub enum Command {
    Scan(Vec<String>),
    File(String),
    Folder(String),
    ListFolder(String),
    Add {
        folder: String,
        value: String,
        meta: Option<String>,
    },
    Delete {
        folder: String,
        value: String,
    },
    DeleteFolder(String),
    ForceDeleteFolder(String),
    Edit {
        folder: String,
        value: String,
        new_value: String,
    },
    EditMeta {
        folder: String,
        value: String,
        new_meta: String,
    },
}

#[derive(thiserror::Error, Debug)]
pub enum TodosWarn {
    #[error("value '{0}' not found in folder '{1}'")]
    ValueNotFound(String, String),
    #[error("folder '{0}' not found")]
    FolderNotFound(String),
    #[error(transparent)]
    FatalError(#[from] TodosError),
    #[error("folder '{0}' not empty, use -Df to force deletion")]
    FolderNotEmpty(String),
    #[error(transparent)]
    File(#[from] std::io::Error),
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Entry {
    value: String,
    meta: Option<String>,
    created: chrono::DateTime<chrono::Local>,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct AppData {
    file: String,
    data: HashMap<String, Vec<Entry>>,
}

fn now() -> chrono::DateTime<chrono::Local> {
    chrono::Local::now()
}

impl AppData {
    pub fn apply(&mut self, cmd: Command) -> Result<(), TodosWarn> {
        use Command::*;
        use TodosWarn::*;
        match cmd {
            Command::File(file_name) => {
                self.save()?; //TODO: check if edits
                self.data = AppData::read_file(&file_name)?;
                self.file = file_name;
            }
            Folder(name) => {
                self.data.insert(name, vec![]);
            }
            ListFolder(_) => {}
            Add {
                folder,
                value,
                meta,
            } => {
                let folder = self.data.get_mut(&folder).ok_or(FolderNotFound(folder))?;
                folder.push(Entry {
                    value,
                    meta,
                    created: now(),
                });
            }
            Delete { folder, value } => {
                let folder_list = self
                    .data
                    .get_mut(&folder)
                    .ok_or(FolderNotFound(folder.clone()))?;
                let item_pos = folder_list
                    .iter()
                    .position(|entry| entry.value == value)
                    .ok_or(ValueNotFound(value, folder))?;
                folder_list.swap_remove(item_pos); // since this is swap_remove, not remove, apply can't be concurrent
            }
            DeleteFolder(folder_name) => {
                use std::collections::hash_map::Entry as HSEntry;
                let folder_entry = self.data.entry(folder_name.clone());
                if let HSEntry::Occupied(folder) = folder_entry {
                    if folder.get().is_empty() {
                        folder.remove();
                    } else {
                        Err(FolderNotEmpty(folder_name))?;
                    }
                } else {
                    Err(FolderNotFound(folder_name))?;
                }
            }
            ForceDeleteFolder(folder) => {
                self.data.remove(&folder).ok_or(FolderNotFound(folder))?;
            }
            Scan(files) => {
                let scanner = Scanner::new()?;
                let scans = unroll::<_, TodosError>(files
                    .into_iter()
                    .map(|file| {
                        let cont = std::fs::read_to_string(&file)?;
                        scanner.find_all(&file, cont)
                    })
                    .collect());
                println!("{:?}", scans);
                todo!()
            }
            other => {
                todo!("{:?}", other)
            }
        };
        Ok(())
    }

    pub fn markdown(&self) -> String {
        let mut out = String::new();
        for (key, value) in self.data.iter() {
            out.push_str(&format!("# {key}\n"));
            for entry in value {
                out.push_str(&format!("- {}\n", entry.value));
            }
            out.push('\n');
        }
        out
    }

    fn read_file(file: &str) -> Result<HashMap<String, Vec<Entry>>, TodosError> {
        use std::fs::read_to_string;
        toml::from_str(&read_to_string(file).unwrap_or(String::new()))
            .map_err(toml::de::Error::into)
    }

    pub fn from_file(from_file: String) -> Result<Self, TodosError> {
        let data = AppData::read_file(&from_file)?;
        Ok(AppData {
            data,
            file: from_file,
        })
    }
    pub fn save(&self) -> Result<(), TodosError> {
        use std::fs::File;
        use std::io::Write;
        let mut file = File::create(&self.file)?;
        file.write_all(toml::to_string(&self.data)?.as_bytes())?;
        Ok(())
    }
}

pub mod cli {
    use super::*;
    #[derive(Debug)]
    enum CommandWish<'arg> {
        Scan,
        File,
        Folder,
        ListFolder,
        Add,
        Delete,
        DeleteFolder,
        ForceDeleteFolder,
        Extra(&'arg str),
        Unknown(&'arg str),
    }

    impl<'arg> CommandWish<'arg> {
        fn nxt<I>(for_cmd: &str, args: &mut std::iter::Peekable<I>) -> Result<String, TodosError>
        where
            I: Iterator<Item = String>,
        {
            args.next()
                .ok_or(TodosError::MissingArgs(for_cmd.to_owned()))
        }
        fn meta<I>(args: &mut std::iter::Peekable<I>) -> Option<String>
        where
            I: Iterator<Item = String>,
        {
            if args.peek()?.starts_with('#') {
                args.next()
            } else {
                None
            }
        }
        fn unxt<I>(args: &mut std::iter::Peekable<I>) -> Option<Vec<String>>
        where
            I: Iterator<Item = String>,
        {
            let mut out = Vec::new();
            loop {
                if args.peek().map(|x| x.starts_with('-')).unwrap_or(false) {
                    return Some(out);
                } else {
                    match args.next() {
                        Some(arg) => out.push(arg),
                        None => {
                            return Some(out);
                        }
                    }
                }
            }
        }
        fn get(
            self,
            args: &mut std::iter::Peekable<impl Iterator<Item = String>>,
            for_cmd: &str,
        ) -> Result<Command, TodosError> {
            use Command as Cmd;
            use CommandWish as Wish;
            use CommandWish::*;
            use TodosError::{ExtraArgs, MissingArgs, UnknownCommand};
            let mut t1 = || Wish::nxt(for_cmd, args);
            match self {
                Scan => Ok(Cmd::Scan(
                    Wish::unxt(args).ok_or(MissingArgs("--scan".to_owned()))?,
                )),
                File => Ok(Cmd::File(t1()?)),
                Folder => Ok(Cmd::Folder(t1()?)),
                ListFolder => Ok(Cmd::ListFolder(t1()?)),
                Add => Ok(Cmd::Add {
                    folder: t1()?,
                    value: t1()?,
                    meta: Wish::meta(args),
                }),
                Delete => Ok(Cmd::Delete {
                    folder: t1()?,
                    value: t1()?,
                }),
                DeleteFolder => Ok(Cmd::DeleteFolder(t1()?)),
                ForceDeleteFolder => Ok(Cmd::ForceDeleteFolder(t1()?)),
                Extra(a) => Err(ExtraArgs(a.to_owned())),
                Unknown(a) => Err(UnknownCommand(a.to_owned())),
            }
        }
    }

    pub fn arg_parse() -> Result<Vec<Command>, TodosError> {
        let mut out: Vec<Command> = Vec::new();
        use CommandWish as Wish;
        let mut args = std::env::args().skip(1).peekable();

        while let Some(cmd) = args.next() {
            let wish = match cmd.as_ref() {
                "--scan" => Wish::Scan,
                "--file" => Wish::File,
                "-f" => Wish::Folder,
                "-l" => Wish::ListFolder,
                "-a" => Wish::Add,
                "-d" => Wish::Delete,
                "-df" => Wish::DeleteFolder,
                "-Df" => Wish::ForceDeleteFolder,
                other if other.starts_with('-') => Wish::Unknown(other),
                other => Wish::Extra(other),
            };
            let command = wish.get(&mut args, &cmd)?;
            out.push(command);
        }

        Ok(out)
    }
}

fn unroll<T, E>(items: Vec<Result<Vec<T>, E>>) -> Result<Vec<T>, E> {
    let mut unrolled = Vec::with_capacity(items.len());

    for result in items {
        unrolled.extend(result?);
    }

    Ok(unrolled)
}
