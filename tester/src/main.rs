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

// always in order (expected, got)
#[derive(Debug)]
enum TestStatus {
	StreamOut(String, String),
	StreamErr(String, String),
	Code(u8, u8),
	Passed,
}

impl std::fmt::Display for TestStatus {
	fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
		match self {
			TestStatus::StreamOut(expected, got)=>{
				write!(f, "\x1b[91mFAILED stdout\x1b[0m expected: {expected}, got: {got}")
			},
			TestStatus::StreamErr(expected, got)=>{
				write!(f, "\x1b[91mFAILED stderr\x1b[0m expected: {expected}, got: {got}")
			},
			TestStatus::Code(expected, got)=>{
				write!(f, "\x1b[91mFAILED $?\x1b[0m expected: {expected}, got: {got}")
			},
			TestStatus::Passed=>write!(f, "\x1b[92mPassed!\x1b[0m"),
		}
	}
}


impl Test {
	fn new(
		id: i32,
		name: Option<&str>,
		command: String,
		ecode: Option<u8>,
		stream_stdin:  Option<&str>,
		stream_stdout: Option<&str>,
		stream_stderr: Option<&str>,
	) -> Self {
		let name = name.unwrap_or("Unnamed test").to_owned();
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

	fn run(self) -> (TestStatus, i32, String) {
		let status = self.execute();
		( status, self.id, self.name )
	}

	fn execute(&self) -> TestStatus {
		use std::process::{Stdio};
		use std::io::{Write};
		let child = std::process::Command::new("/bin/sh")
			.arg("-c")
			.arg(&self.command)
			.stdin(Stdio::piped())
			.stdout(Stdio::piped())
			.stderr(Stdio::piped())
			.spawn()
			.expect("Can't execute command");

		// write input
		if !self.stream_stdin.is_empty() {
			let mut child_stdin = child.stdin.as_ref().expect("Can't write to stdin");
			child_stdin.write_all(self.stream_stdin.as_ref()).unwrap();
		}

		// gather status, stdout, stderr
		let (status, stdout, stderr) = {
			let output = child.wait_with_output().unwrap();
			(
				output.status.code().unwrap_or(0),
				//TODO allow for non-UTF8 sequences
				String::from_utf8(output.stdout).expect("Invalid UTF-8 sequence in stdout"),
				String::from_utf8(output.stderr).expect("Invalid UTF-8 sequence in stderr"),
			)
		};

		if status != self.ecode.into() {
			TestStatus::Code(self.ecode, status as u8)
		} else if stdout != self.stream_stdout {
			TestStatus::StreamOut(self.stream_stdout.clone(), stdout)
		} else if stderr != self.stream_stderr {
			TestStatus::StreamErr(self.stream_stderr.clone(), stderr)
		} else {
			TestStatus::Passed
		}
	}
}

// Match -To-> &str
fn mtbs(mtc: Option<regex::Match>) -> Option<&str> {
	mtc.map(|mtc|mtc.as_str())
}

//TODO allow for non-UTF8 sequences
fn main() {
	/* pattern found at: https://regexr.com/7shjt */
	const PATTERN: &str = r#"(?:`(.*?)`)?\s*`(.*?)`\s*(\d{1,3})?\s*(?:in`((?:.|\n)*?)`)?\s*(?:out`((?:.|\n)*?)`)?\s*(?:err`((?:.|\n)*?)`)?"#;
	let re = regex::Regex::new(PATTERN).unwrap();

	let mut test_count = 0;
	let args = std::env::args();
	let mut tests: Vec<Test> = Vec::new();
	let files: Vec<String> = args.skip(1).collect();
	let files = {
		if files.len() == 0 {
			glob::glob("*.test").unwrap().map(|a|{
				a.unwrap()
					.into_os_string()
					.into_string().unwrap()
			}).collect::<Vec<String>>().into_iter()
		} else {
			files.into_iter()
		}
	};

	for file in files {
		let content = std::fs::read_to_string(file).expect("can't read file");
		let caps = re.captures_iter(&content);

		for cap in caps {
			test_count+=1;
			let name = mtbs(cap.get(1));
			let command = cap.get(2).expect("must provide command to be ran").as_str().to_owned();
			let ecode: Option<u8> = cap.get(3).map(|mt|mt.as_str().parse::<u8>().expect("can't parse int"));
			let stdin  = mtbs(cap.get(4));
			let stdout = mtbs(cap.get(5));
			let stderr = mtbs(cap.get(6));
			tests.push(
				Test::new(
					test_count, name,
					command, ecode,
					stdin, stdout, stderr
				)
			);
		}
	}

	// TODO: multi-thread OR spawn all, await each, print all
	for test in tests {
		let (status, id, name) = test.run();
		println!("{name}#{id}: {status}");
	}
}

