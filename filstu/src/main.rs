use filstu::*;

#[allow(dead_code)]
fn prt(e: &Entry) {
    #[cfg(not(feature="display"))]
    println!("{e:#?}");
    #[cfg(feature="display")]
    println!("{e}");
}

fn main() {
    let d = std::path::Path::new("./src");
    let d = read_dir(d).unwrap();
    let mut files = vec![];
    d.map(
        |_, _| { },
        |p| {
            files.push(p);
        },
    );
    println!("{files:#?}");
}
