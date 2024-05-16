use git2::Error as GErr;
use git2::*;
type GitResult<T> = Result<T, GErr>;

fn get_remote(repo: &Repository) -> GitResult<Remote> {
    let remote_name = repo
        .remotes()?
        .into_iter()
        .flatten()
        .next()
        .unwrap()
        .to_owned();
    repo.find_remote(&remote_name)
}

pub struct StatusReport<'r> {
    repo: &'r Repository,
    staged: bool,
    new: bool,
    modified: bool,
    deleted: bool,
    renamed: bool,
    conflict: bool,
    branch: String,
    remote: Remote<'r>,
    print_graph_count: bool,
    ahead: Option<usize>,
    behind: Option<usize>,
}

fn get_branch(repo: &Repository) -> GitResult<String> {
    Reference::normalize_name(
        repo.head()?.shorthand().unwrap_or("<unknown branch>"),
        ReferenceFormat::ALLOW_ONELEVEL | ReferenceFormat::REFSPEC_SHORTHAND,
    )
}

impl<'r> StatusReport<'r> {
    pub fn new(repo: &'r Repository, print_graph_count: bool) -> GitResult<Self> {
        let branch = get_branch(repo)?;
        let remote = get_remote(repo)?;
        Ok(StatusReport {
            repo,
            print_graph_count,
            staged: false,   // →
            new: false,      // +
            modified: false, // *
            deleted: false,  // -
            renamed: false,  // ←
            conflict: false, // ↨
            branch,
            remote,
            ahead: None,
            behind: None,
        })
    }

    pub fn update_graph(&mut self) -> Result<(), GErr> {
        let (ahead, behind) = self.get_graph()?;
        self.ahead = Some(ahead);
        self.behind = Some(behind);
        Ok(())
    }

    pub fn get_graph(&mut self) -> Result<(usize, usize), GErr> {
        self.remote.connect(Direction::Fetch)?;
        let local = self.repo.head()?.target().unwrap();
        let upstream = self.remote.list()?.first().unwrap().oid();
        self.remote.disconnect()?;
        self.repo.graph_ahead_behind(local, upstream)
    }

    pub fn get_statuses(&self) -> Result<Vec<Status>, GErr> {
        Ok(self
            .repo
            .statuses(None)?
            .iter()
            .map(|a| a.status())
            .filter(|status| !status.is_empty() && !status.is_ignored())
            .collect())
    }

    pub fn update_statuses(&mut self) -> Result<(), GErr> {
        self.get_statuses()?
            .into_iter()
            .for_each(|stat| self.update(stat));
        Ok(())
    }
    fn update(&mut self, status: Status) {
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
        self.staged |= is_index_new
            | is_index_modified
            | is_index_deleted
            | is_index_renamed
            | is_index_typechange;
        self.new |= is_wt_new;
        self.modified |= is_wt_modified;
        self.deleted |= is_wt_deleted;
        self.renamed |= is_wt_renamed;
        self.conflict |= is_conflicted;
    }
}

impl<'r> std::fmt::Display for StatusReport<'r> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> {
        let clean = !(self.staged
            | self.new
            | self.modified
            | self.deleted
            | self.renamed
            | self.behind.is_some()
            | self.conflict);
        write!(f, " \x1b[1;4;35m{}\x1b[0m ", self.branch)?;
        if clean {
            write!(f, "\x1b[32mÜ")?
        };
        if self.new {
            write!(f, "\x1b[31m+")?
        };
        if self.staged {
            write!(f, "\x1b[31m→")?
        };
        if self.conflict {
            write!(f, "\x1b[5;31mx")?
        };
        if self.deleted {
            write!(f, "\x1b[31m-")?
        };
        if self.renamed {
            write!(f, "\x1b[34m←")?
        };
        if self.modified {
            write!(f, "\x1b[31m*")?
        };
        if let Some(pull) = self.behind {
            if pull != 0 {
                write!(f, "\x1b[7;5;31m↓")?;
                if self.print_graph_count {
                    write!(f, "\x1b[0m{pull}")?;
                }
            }
        };
        if let Some(push) = self.ahead {
            if push != 0 {
                write!(f, "\x1b[5;34m↑\x1b[0m")?;
                if self.print_graph_count {
                    write!(f, "\x1b[0m{push}")?;
                }
            }
        };
        write!(f, "\x1b[0m")
    }
}
