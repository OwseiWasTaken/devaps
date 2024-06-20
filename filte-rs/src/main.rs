use filte_rs::*;
use std::io::{stdin, BufRead};

fn match_all(xs: &[Transformer], text: &str) -> bool {
    xs.iter().all(|t|t.compare(text))
}

const HELP: &'static str = "
usage filter [i](mode)(pattern)

excludes lines that don't fit the patterns with speficied modes

if 'i' is the prefix of a mode the filter will exclude lines the *do* fit the pattern

modes:
=   : equals
+|h : includes
-|e : excludes (shorthand for i+)
.|r : regex
?|g : glob

the only way to print this text is to execute with no arguments
";

fn help() -> ! {
    eprintln!("{}", HELP);
    std::process::exit(0)
}

fn main() -> eyre::Result<()> {
    let filters: Vec<Transformer> = std::env::args()
        .skip(1)
        .map(Transformer::try_from)
        .collect::<Result<_, _>>()?;
    if filters.len() == 0 {
        help();
    }
    let stdin = stdin();
    for line in stdin.lock().lines() {
        let line = line?;
        if match_all(&filters, &line) {
            println!("{line}");
        }
    }
    Ok(())
}

