use todos::*;

fn main() -> Result<(), TodosError> {
    let path = get_file_path()?;
    let x = mk_appdata();
    save_data(&path, &x)?;
    println!("{x:?}");

    Ok(())
}


