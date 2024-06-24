#[derive(thiserror::Error, Debug)]
pub enum FilteError {
    #[error(transparent)]
    RegexSyntax(#[from] regex::Error),
    #[error(transparent)]
    GlobSyntax(#[from] glob::PatternError),

    #[error("No such filter mode {0}'")]
    NoFilter(char),

    #[error("Missing text")]
    MissingText,

    #[error("Missing mode")]
    MissingMode,
}
use FilteError::*;

#[derive(Debug)]
enum Filter {
    Is(String),
    Starts(String),
    Ends(String),
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
        let mut chars = tx.chars();
        let mut mode = chars.next().ok_or(MissingText)?;
        let mut invert = mode == 'i';
        if mode == 'i' {
            mode = chars.next().ok_or(MissingMode)?;
        }
        let pattern: String = chars.collect();
        let filter = match mode {
            '=' => Is(pattern),
            's' | '^' => Starts(pattern),
            'z' | '$' => Ends(pattern),
            'h' | '+' => Includes(pattern),
            'e' | '-' => {
                invert = !invert;
                Includes(pattern)
            }
            'r' | '.' => Regex(pattern.try_into()?),
            'g' | '?' => Glob(glob::Pattern::new(&pattern)?),
            other => Err(NoFilter(other))?,
        };
        Ok(Transformer { filter, invert })
    }
}

impl Filter {
    fn compare(&self, text: &str) -> bool {
        use Filter::*;
        match self {
            Is(rf) => rf == text,
            Starts(rf) => text.starts_with(rf),
            Ends(rf) => text.ends_with(rf),
            Includes(rf) => text.contains(rf),
            Regex(rf) => rf.is_match(text),
            Glob(rf) => rf.matches(text),
        }
    }
}

impl Transformer {
    pub fn compare(&self, text: &str) -> bool {
        self.filter.compare(text) != self.invert
    }
}
