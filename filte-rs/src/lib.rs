#[derive(thiserror::Error, Debug)]
pub enum FilteError {
    #[error(transparent)]
    RegexSyntax(#[from] regex::Error),
    #[error(transparent)]
    GlobSyntax(#[from] glob::PatternError),

    #[error("No such filter mode {0}'")]
    NoFilter(char),

    #[error("Missing mode/pattern division from '{0}'")]
    MissingDiv(String),

    #[error("Missing mode text")]
    MissingMod,

    #[error("Missing pattern text")]
    MissingPattern,
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
        let pattern = pattern.to_owned();
        let mut modes = mode.chars().rev();
        let mode = modes.next().ok_or(MissingMod)?;
        let extra = modes.next();
        let mut invert = extra == Some('i');
        let filter = match mode {
            '='=>Is(pattern),
            'i'|'+'=>Includes(pattern),
            'e'|'-'=>{
                invert = !invert;
                Includes(pattern)
            },
            '.'|'r'=>Regex(pattern.try_into()?),
            '?'|'g'=>Glob(glob::Pattern::new(&pattern)?),
            other=>Err(NoFilter(other))?,
        };
        Ok(Transformer{
            filter,
            invert,
        })
    }
}

impl Filter {
    fn compare(&self, text: &str) -> bool {
        use Filter::*;
        match self {
            Is(rf)=>rf==text,
            Includes(rf)=>text.contains(rf),
            Regex(rf)=>rf.is_match(text),
            Glob(rf)=>rf.matches(text),
        }
    }
}

impl Transformer {
    pub fn compare(&self, text: &str) -> bool {
        self.filter.compare(text) != self.invert
    }
}
