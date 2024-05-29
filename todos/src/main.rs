use todos::*;

/*
-a is optional
--file isn't remembered for future executions
--html [with html feature] print html instead of markdown
list if with no args
todos
  -f AAC // make folter
  -l AAC // list folter
  -a AAC "hello" // [0] = hello
  -a AAC "help" // [1] = help
  -a AAC "HI" // [2] = HI
  -d AAC "hellp" // deleted "hello" entry
  -d AAC // failed to delete non-empty folder
  -d AAC.help // deleted "help"
  -dd AAC // force delete folder (still with HI entry)
  --file that.toml -f DIR -a DIR "hi!!"
  --html
*/

/*
listing (MD)
# AAC
- hello
- help
- hi

# ABC
- Yo?
- anyone here?
- HIIIIII
*/

fn main() -> Result<(), TodosError> {
    let args: Vec<String> = std::env::args().into_iter().skip(1).collect();
    println!("{args:?}");
    //let path = get_file_path()?;
    let x = mk_appdata();
    //save_data(&path, &x)?;
    println!("{}", x.markdown());

    Ok(())
}
