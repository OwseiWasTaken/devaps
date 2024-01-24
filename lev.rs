fn nei(y: usize, x: usize, matrix: &Vec<Vec<i32>>) -> i32 {
	std::cmp::min(
		matrix[y-1][x-1],
		std::cmp::min(
			matrix[y][x-1],
			matrix[y-1][x]
		)
	)
}

fn prepare_mat(ylen: &usize, xlen: &usize, matrix: &mut Vec<Vec<i32>>) {
	for y in 0..=*ylen {
		matrix[y][0] = y as i32;
	}
	for x in 0..=*xlen {
		matrix[0][x] = x as i32;
	}
}

fn collapse_mat(
	yword: &String, xword: &String,
	ymat_len: &usize, xmat_len: &usize,
	matrix: &mut Vec<Vec<i32>>
) {
	let mut ychars = yword.chars();
	for y in 0..*ymat_len {
		let mut xchars = xword.chars();
		let ychar = ychars.next();
		for x in 0..*xmat_len {
			let neib = nei(y+1, x+1, matrix);
			let xchar = xchars.next();
			let matched = ychar.map_or(false, |yc|{
				xchar.map_or(false, |xc|{
					yc==xc
				})
			});
			matrix[y+1][x+1] = neib+(if matched {0} else {1});
		}
	}
}

fn main() -> Result<(), String> {
	let mut args = std::env::args();
	let progname = args.next().ok_or("Can't find $0: program name")?;

	let xword = args.next().ok_or(format!("usage: {progname} 'target' 'compare'"))?;
	let xlen: usize = xword.len();

	let mut ywords: Vec<String> = Vec::new();
	let ylen: usize = args.map(|w|{
		ywords.push(w.clone());
		w.len()
	}).max().ok_or("must have at least one word to compare agains")?;

	let mut matrix = vec![vec![0; xlen+1]; ylen+1];

	prepare_mat(&ylen, &xlen,&mut matrix);
	for yword in ywords {
		collapse_mat(&yword, &xword, &ylen, &xlen, &mut matrix);
		println!("{} {}", matrix[yword.len()][xword.len()], yword);
	}

	Ok(())
}

