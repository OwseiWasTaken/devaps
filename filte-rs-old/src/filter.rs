use filte_rs::*;
use std::io::{stdin, BufRead};

fn match_all(filters: &[Filter], text: &str) -> bool {
    filters.iter().all(|f| f.compare(text))
}

fn main() -> eyre::Result<()> {
    let filters: Vec<Filter> = std::env::args()
        .skip(1)
        .map(Filter::try_from)
        .collect::<Result<_, _>>()?;
    let stdin = stdin();
    for line in stdin.lock().lines() {
        let line = line?;
        if match_all(&filters, &line) {
            println!("{line}");
        }
    }
    Ok(())
}
