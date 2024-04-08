fn main() {
	let out = std::process::Command::new("git")
		.arg("status")
		.output()
		.expect("not in git repo");
	let out = std::str::from_utf8(&out.stdout).expect("can't convert output to UTF-8");

	let branch = {
		let lines: Vec<&str> = out.lines().collect();
		lines
			.first().expect("expected `git status` to output a line")
			.to_owned()
			.strip_prefix("On branch ").expect("expected first line of `git status`")
	};

	print!("\x1b[1;95m{branch} ");
	if out.contains("working tree clean"           ) { print!("\x1b[0;32mü"); }
	if out.contains("Untracked files"              ) { print!("\x1b[0;31m+"); }
	if out.contains("Changes to be committed"      ) { print!("\x1b[0;31m→"); }
	if out.contains("Your branch is ahead"         ) { print!("\x1b[5;34m↑"); }
	if out.contains("behind"                       ) { print!("\x1b[5;91m↓"); }
	if out.contains("diverged"                     ) { print!("\x1b[5;91m↓\x1b[5;34m↑"); }
	if out.contains("Changes not staged for commit") { print!("\x1b[0;31m*"); }
	print!("\x1b[0m");
}
