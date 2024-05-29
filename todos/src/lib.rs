use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(thiserror::Error, Debug)]
pub enum TodosError {
    #[error("Failed IO operation: {0}")]
    File(#[from] std::io::Error),
    #[error("Can't find neither $HOME nor $TODOS_RS env vars")]
    MissingEnv,
    #[error("Can't serialize data into TOML")]
    TOML(#[from] toml::ser::Error),
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Entry {
    value: String,
    created: chrono::DateTime<chrono::Local>,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct AppData {
    user: String,
    folders: HashMap<String, Vec<Entry>>,
}

pub fn save_data(file_name: &str, data: &AppData) -> Result<(), TodosError> {
    use std::fs::File;
    use std::io::Write;
    let mut file = File::create(file_name)?;
    file.write_all(toml::to_string(data)?.as_bytes())?;
    Ok(())
}

pub fn get_file_path() -> Result<String, TodosError> {
    std::env::var("TODOS_RS")
        .or(std::env::var("HOME").map(|path| path + "/.todos.toml"))
        .or(Err(TodosError::MissingEnv))
}

pub fn mk_appdata() -> AppData {
    AppData {
        user: "manse".to_owned(),
        folders: HashMap::from([
            mk_folder("AAC"),
            mk_folder("Paulista"),
            mk_folder("Timecard"),
        ]),
    }
}

fn mk_folder(name: &str) -> (String, Vec<Entry>) {
    (
        name.to_owned(),
        vec![mk_entry(); 3]
    )
}

fn mk_entry() -> Entry {
    Entry {
        value: "hi".to_owned(),
        created: chrono::Local::now(),
    }
}
