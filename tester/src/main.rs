#[derive(Debug)]
struct Test {
	id: i32,
	name: String,
	command: String,
	ecode: u8,
	stream_stdin: String,
	stream_stdout: String,
	stream_stderr: String,
}

enum TestStatus {
	StreamOut(String, String),
	StreamErr(String, String),
	Code(u8, u8),
}

impl Test {
	fn new(
		id: i32,
		name: Option<String>,
		command: String,
		ecode: Option<u8>,
		stream_stdin:  Option<&str>,
		stream_stdout: Option<&str>,
		stream_stderr: Option<&str>,
	) -> Self {
		let name = name.unwrap_or_else(||format!("Unnamed test #{id}"));
		let ecode = ecode.unwrap_or(0);
		let stream_stdin = stream_stdin.unwrap_or("").to_owned();
		let stream_stdout = stream_stdout.unwrap_or("").to_owned();
		let stream_stderr = stream_stderr.unwrap_or("").to_owned();

		Self{
			id,name,
			command,ecode,
			stream_stdin,
			stream_stdout,
			stream_stderr,
		}
	}

	fn execute(&self) -> TestStatus {
		//let cmd = std::process::Command("sh")
		//	.arg(self.command)
		//	.stdin(std::process::Stdio::piped())
		//	.output()
		//	.expect("can't runt command");
		//cmd.status
		todo!("Implemend Test.execute!")
	}
}

fn mtbs(mtc: Option<regex::Match>) -> Option<&str> {
	mtc.map(|mtc|mtc.as_str())
}

fn mtos(mtc: Option<regex::Match>) -> Option<String> {
	mtc.map(|mtc|mtc.as_str().to_owned())
}

fn main() {
	//TODO: make name group optional
	const PATTERN: &str = r#"`(.*?)`\s*`(.*?)`\s*(\d{1,3})?\s*(?:in`((?:.|\n)*?)`)?\s*(?:out`((?:.|\n)*?)`)?\s*(?:err`((?:.|\n)*?)`)?"#;
	let re = regex::Regex::new(PATTERN).unwrap();

	let mut test_count = 0;
	let args = std::env::args();
	for file in args.skip(1) {
		let content = std::fs::read_to_string(file).expect("FUCK");
		let caps = re.captures_iter(&content);

		for cap in caps {
			test_count+=1;
			let test_name = mtos(cap.get(1));
			let test_command = cap.get(2).expect("must provide command to be ran").as_str().to_owned();
			let test_ecode: Option<u8> = cap.get(3).map(|mt|mt.as_str().parse::<u8>().expect("can't parse int"));
			let test_in  = mtbs(cap.get(4));
			let test_out = mtbs(cap.get(5));
			let test_err = mtbs(cap.get(6));
			let t = Test::new(test_count, test_name, test_command, test_ecode, test_in, test_out, test_err);
			println!("{t:?}");
		}
	}
}
