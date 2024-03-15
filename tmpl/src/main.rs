use tmpl::*;
fn usage(progname: String) -> ! {
	eprintln!("usage: {} [project template] [project args]", progname);
	std::process::exit(1);
}

fn try_main(args: std::env::Args) -> anyhow::Result<()> {
	try_gather(args)?.run()
}

// objective: $ tmpl rust timecard2 axum maud -axum sqlx -postgres
//TODO add default project template
fn main() {
	let mut args = std::env::args();
	let prog_name = args.next().unwrap_or("tmpl".to_owned());

	if let Err(error) = try_main(args) {
		eprintln!("Error: {error}");
		usage(prog_name);
	}
}
