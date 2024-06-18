#[derive(thiserror::Error, Debug)]
pub enum FilterError {
    #[error(transparent)]
    RegexSyntax(#[from] regex::Error),
    #[error(transparent)]
    GlobSyntax(#[from] glob::PatternError),

    #[error("No such filter mode {0}'")]
    NoFilter(char),
    #[error("No text in filter")]
    NoText,
}

enum FilterType {
    Equals,
    Has,
    Excludes,
    Regex,
    Glob,
}

#[derive(Debug)]
pub enum Filter {
    Equals(String),      // =
    Has(String),         // +
    Excludes(String),    // -
    Regex(regex::Regex), // .
    Glob(glob::Pattern), // ?
}

impl TryFrom<String> for Filter {
    type Error = FilterError;
    fn try_from(text: String) -> Result<Filter, FilterError> {
        let mut chars = text.chars();
        let mode = chars.next().ok_or(FilterError::NoText)?;
        let mode = FilterType::try_from(mode)?;
        let pattern: String = chars.collect();
        mode.fill(pattern)
    }
}

impl FilterType {
    fn fill(self, pattern: String) -> Result<Filter, FilterError> {
        Ok(match self {
            FilterType::Equals => Filter::Equals(pattern),
            FilterType::Has => Filter::Has(pattern),
            FilterType::Excludes => Filter::Excludes(pattern),
            FilterType::Regex => Filter::Regex(pattern.try_into()?),
            FilterType::Glob => Filter::Glob(glob::Pattern::new(&pattern)?),
        })
    }
}

impl TryFrom<char> for FilterType {
    type Error = FilterError;
    fn try_from(v: char) -> Result<FilterType, FilterError> {
        Ok(match v {
            '=' => FilterType::Equals,
            '+' => FilterType::Has,
            '-' => FilterType::Excludes,
            '~' => FilterType::Regex,
            '?' => FilterType::Glob,
            x => Err(FilterError::NoFilter(x))?,
        })
    }
}

impl Filter {
    pub fn compare(&self, text: &str) -> bool {
        use Filter::*;
        match self {
            Equals(rf) => rf == text,
            Has(rf) => text.contains(rf),
            Excludes(rf) => !text.contains(rf),
            Regex(rf) => rf.is_match(text),
            Glob(rf) => rf.matches(text),
        }
    }
}

