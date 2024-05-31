use todos::*;

/*
--file isn't remembered for future executions
list if with no args
todos
  -f AAC // make folter
  -l AAC // list folter
  -a AAC "HI" // AAC[1] = {HI}
  -aa AAC "hello" help"" // AAC[3] = {HI, hello, help}
  -d AAC "hellp" // deleted "hello" entry // AAC[2] = {HI, help}
  -df AAC // failed to delete non-empty folder
  -d AAC help // deleted "help" // AAC[1] = {HI}
  -Df AAC // force delete folder (had HI entry)
  --file that.toml -f DIR -a DIR "hi!!"
  --scan *.rs // reads files and checks for comments with TODO and writes
      them as entries in 'files' folder (or dirname folder in case --scan
      pointed to a directory) and puts file:line:col in meta
*/

/*
listing (MD)
# AAC
- hello (RHS)[auto]
- help (RHS)[auto]
- hi (RHS)[auto]

# ABC
- Yo? (RHS)[auto]
- anyone here? (RHS)[auto]
- HIIIIII (RHS)[auto]
*/

fn main() -> eyre::Result<()> {
    let x = std::time::SystemTime::now();
    println!("{x:?}");
    let path = get_file_path()?;
    let mut app_data = AppData::from_file(path)?;
    let cmds: Vec<Command> = cli::arg_parse()?;
    for cmd in cmds {
        let user_error = app_data.apply(cmd);
        if let Err(user_error) = user_error {
            eprintln!("[WARNING] {user_error}");
            eprintln!("Halting early because of user error");
            std::process::exit(1);
        }
    }
    app_data.save()?;
    println!("{}", app_data.markdown());

    Ok(())
}
