use filte_rs::*;
use std::io::{stdin, BufRead};

fn match_all(filters: &[Filter], text: &str) -> bool {
    filters.iter().all(|f| f.compare(text))
}

fn main() -> eyre::Result<()> {
    let args: Vec<Filter> = std::env::args()
        .skip(1)
        .map(Filter::try_from)
        .collect::<Result<_, _>>()?;
    println!("{args:?}");
    let stdin = stdin();
    let mut line_count = 0;
    for line in stdin.lock().lines() {
        println!("{:?}", line);
    }
    Ok(())
}
