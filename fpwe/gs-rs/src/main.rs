//TODO: use libgit2 and these references to remake devaps/gs.go
//: without executing a new process ($git status) every time
//:: bonus print erros to stderr so that I can just to gs-rs 2>/dev/null in bashrc

//https://stackoverflow.com/questions/76949001/libgit2-obtain-list-of-uncommitted-changes
//https://libgit2.org/libgit2/ex/HEAD/status.html

fn main() {
    let repo = git2::Repository::discover(".").expect("not in git repo");
    let state = repo.state();
    println!("{:?}", state);
}
