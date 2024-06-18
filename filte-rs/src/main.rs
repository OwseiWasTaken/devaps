use filte_rs::*;
use std::io::{stdin, BufRead};

fn main() -> eyre::Result<()> {
    let filters: Vec<Transformer> = std::env::args()
        .skip(1)
        .map(Transformer::try_from)
        .collect::<Result<_, _>>()?;
    println!("{filters:?}");
    //let stdin = stdin();
    //for line in stdin.lock().lines() {
    //    let line = line?;
    //    if match_all(&filters, &line) {
    //        println!("{line}");
    //    }
    //}
    Ok(())
}
