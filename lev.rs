type LevResult = Result<(), u8>;

fn trip(mat: &Vec<usize>, y: usize, x: usize, matcol_size: usize) -> usize {
  std::cmp::min(
    std::cmp::min(
      mat[(y-1)*matcol_size+x],
      mat[y*matcol_size+(x-1)]
    ),
    mat[(y-1)*matcol_size+(x-1)]
  )
}

fn prtmat(mat: &Vec<usize>, matcol_size: usize, col_size: usize, row_size: usize) {
  for y in 0..row_size {
    for x in 0..col_size {
      print!("[{}]", mat[y*matcol_size+x])
    }
    print!("\n");
  }
}

fn wagfish(
  col_size: usize,
  row_size: usize,
  matcol_size: usize,
  mat: &mut Vec<usize>,
  target: &String, comp: &String
) -> usize {

  let target_chars = target.as_bytes();
  let comp_chars = comp.as_bytes();

  for y in 0..row_size {
    for x in 0..col_size {
      if y==0 && x==0 {
        mat[0] = 0;
        continue;
      } else if y!=0  && x==0 {
        mat[y*col_size] = y;
        continue;
      } else if x!=0 && y==0 {
        mat[x] = x;
        continue;
      }
      println!("[{y};{x}] = {},{} ", target_chars[y-1] as char, comp_chars[x-1] as char);

      if target_chars[y-1] == comp_chars[x-1] {
        mat[(y)*col_size+x] = trip(mat, y, x, matcol_size);
      } else {
        mat[(y)*col_size+x] = trip(mat, y, x, matcol_size)+1;
      }
      //target[y]
    }
  }


  println!("{target} -> {comp} = {}", mat[(row_size)*col_size+col_size-1]);
  prtmat(mat, matcol_size, col_size, row_size);
  return mat[(row_size)*col_size+col_size-1];
}

fn try_main(args: std::env::Args) -> LevResult {
  let words: Vec<String> = args.collect();
  if words.len() < 2 { return Err(1); }
  let target = &words[0];
  let col_size = target.len()+1;
  let row_size = words
    .iter()
    .skip(1)
    .map(|s| s.len()+1)
    .max()
    .ok_or(2)?;

  let mut mat: Vec<usize> = vec![0; col_size*row_size];
  let (index, (word, lev)) = words
    .iter()
    .skip(1)
    .map(|comp|->(&String, usize){
      (comp, wagfish(
        comp.len(),
        row_size-1,
        col_size,
        &mut mat,
        target, comp
      ))
    })
    .enumerate()
    .max_by_key(|(_, (_, lev_a))|{
      *lev_a
    }).ok_or(2)?;

  println!("[{}]: {word} -> {target} = {lev}", index);

  Ok(())
}

fn main() {
  let mut args = std::env::args();
  let progname = args.next().unwrap();
  match try_main(args) {
    Err(ecode)=>{
      println!("Usage: {progname} \"target\" [\"compare\" ...]");
      std::process::exit(ecode as i32);
    },
    Ok(_)=>{}
  }
}

