use pwd_rs::*;
use std::io::prelude::*;
use std::os::unix::net::UnixListener;

fn format_path(path: String, edits: Vec<Edit>) -> String {
    edits
        .into_iter()
        .fold(Dir::new(path), Dir::edit)
        .into_string()
        .replace("\\e", "\x1b")
}

fn main() -> std::io::Result<()> {
    let sockname = std::env::var("FPWDRS_SOCKET_NAME")
        .expect("$FPWDRS_SOCKET_NAME must be set");
    let _ = std::fs::remove_file(&sockname);
    let listener = UnixListener::bind(&sockname)?;
    let config = fancy_unwrap(get_config());

    eprintln!("Listening @ {sockname}");
    loop {
        match listener.accept() {
            Ok((mut socket, _)) => {
                let mut response = String::new();
                socket.read_to_string(&mut response)?;
                socket.write_all(format_path(response, config.clone()).as_bytes())?;
            }
            Err(e) => println!("accept failed: {:?}", e),
        }
    }
}
