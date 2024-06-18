#[derive(thiserror::Error, Debug)]
pub enum FilteError {
    #[error(transparent)]
    RegexSyntax(#[from] regex::Error),
    #[error(transparent)]
    GlobSyntax(#[from] glob::PatternError),

    #[error("No such filter mode {0}'")]
    NoFilter(char),
    
    #[error("Missing mode/filter division from '{0}'")]
    MissingDiv(String),
    
    #[error("Missing mode text")]
    MissingMod(String),
    
    #[error("Missing filter text")]
    MissingPattern(String),
    
    #[error("No text in filter")]
    NoText,
}
use FilteError::*;

#[derive(Debug)]
enum Filter {
    Is(String),
    Includes(String),
    Regex(regex::Regex),
    Glob(glob::Pattern),
}

#[derive(Debug)]
pub struct Transformer {
    filter: Filter,
    invert: bool,
}

impl TryFrom<String> for Transformer {
    type Error = FilteError;
    fn try_from(tx: String) -> Result<Transformer, FilteError> {
        use Filter::*;
        let (mode, pattern) = tx
            .split_once("/")
            .ok_or(MissingDiv(tx.to_owned()))?;
        let mut modes = mode.chars().rev();
        let mode = modes.next();
        let extra = modes.next();
        let invert = extra == Some('i');
        match mode {
            '='=>Is(pattern),
            's'=>Includes(pattern),
        }
        todo!()
    }
}
