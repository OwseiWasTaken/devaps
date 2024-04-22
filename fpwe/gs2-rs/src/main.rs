fn get_remote(repo: &git2::Repository) -> git2::Remote {
    let remote_name = repo
        .remotes()
        .unwrap()
        .into_iter()
        .flatten()
        .next()
        .unwrap()
        .to_owned();
    repo.find_remote(&remote_name).unwrap()
}

fn main() {
    let repo = git2::Repository::discover(".").unwrap();

    let /*mut*/ push = false; // ↑
                              // only if --fetch flag
    if false {
        //TODO maybe should use more precise API
        // to connect and download from remote
        // to know if repo is outdated
        let mut remote = get_remote(&repo);
        let branches: Vec<_> = repo
            .branches(None)
            .unwrap()
            .map(Result::unwrap)
            .map(|(branch, _)| branch)
            .collect();
        let branches: Vec<_> = branches
            .iter()
            .map(git2::Branch::name)
            .map(Result::unwrap)
            .map(Option::unwrap)
            .filter(|a| !a.starts_with("origin")) // TODO use remote.name instead
            .collect();
        remote.fetch(&branches, None, None).unwrap()
    }

    let statuses = repo.statuses(None).unwrap();
    let statuses_iter = statuses
        .iter()
        .map(|a| a.status())
        .filter(|status| !status.is_empty() && !status.is_ignored());

    let mut staged = false; // →
    let mut new = false; // +
    let mut modified = false; // *
    let mut deleted = false; // -
    let mut renamed = false; // ←
    let /*mut*/ pull = false; // ↓
    let mut conflict = false; // ↨
    for status in statuses_iter {
        let is_index_new = status.is_index_new();
        let is_index_modified = status.is_index_modified();
        let is_index_deleted = status.is_index_deleted();
        let is_index_renamed = status.is_index_renamed();
        let is_index_typechange = status.is_index_typechange();
        let is_wt_new = status.is_wt_new();
        let is_wt_modified = status.is_wt_modified();
        let is_wt_deleted = status.is_wt_deleted();
        //let is_wt_typechange = status.is_wt_typechange();
        let is_wt_renamed = status.is_wt_renamed();
        let is_conflicted = status.is_conflicted();
        staged |= is_index_new
            | is_index_modified
            | is_index_deleted
            | is_index_renamed
            | is_index_typechange;
        new |= is_wt_new;
        modified |= is_wt_modified;
        deleted |= is_wt_deleted;
        renamed |= is_wt_renamed;
        conflict |= is_conflicted;
    }
    let clean = !(staged | new | modified | deleted | renamed | push | pull | conflict);

    let branch = git2::Reference::normalize_name(
        repo.head().unwrap().shorthand().unwrap(),
        git2::ReferenceFormat::ALLOW_ONELEVEL | git2::ReferenceFormat::REFSPEC_SHORTHAND,
    )
    .unwrap();

    print!("\x1b[4;35m{branch}\x1b[0m ");
    if clean    { print!("\x1b[32mÜ") };
    if new      { print!("\x1b[31m+") };
    if staged   { print!("\x1b[31m→") };
    if push     { print!("\x1b[5;34m↑") };
    if pull     { print!("\x1b[7;5;31m↓") };
    if conflict { print!("\x1b[5;31mx") };
    if deleted  { print!("\x1b[31m-") };
    if renamed  { print!("\x1b[34m←") };
    if modified { print!("\x1b[31m*") };
    print!("\x1b[0m");
}
