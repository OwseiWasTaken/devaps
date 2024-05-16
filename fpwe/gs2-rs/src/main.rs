use gs2::*;

fn main() -> Result<(), git2::Error> {
    let repo = match git2::Repository::discover(".") {
        Ok(repo) => repo,
        Err(_) => return Ok(()),
    };

    // get branch name, remote
    let mut status_reporter = StatusReport::new(&repo)?;
    // get diffs
    status_reporter.update_statuses()?;
    // get remote graph
    status_reporter.update_graph()?;
    print!("{status_reporter}");

    Ok(())
}

//refs/remotes/origin/master
//52cd28549250197520c0859639cd66426b985b7b
//refs/heads/master
//52cd28549250197520c0859639cd66426b985b7b

