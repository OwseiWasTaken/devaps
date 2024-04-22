use pwd_rs::*;

fn main() {
    let config = fancy_unwrap(get_config());
    let pwd = fancy_unwrap(std::env::var("PWD").or(Err("Can't find $PWD env variable")));
    let dir = Dir::new(pwd);
    let path = config
        .into_iter()
        .fold(dir, Dir::edit)
        .into_string();
    print!("{path}");
}
