use std::fs;
use std::path::{Path, PathBuf};

#[derive(Debug, Clone)]
pub enum Node<A, H> {
    Atom(A),
    List(H, Vec<Node<A, H>>),
}
pub type Entry = Node<PathBuf, PathBuf>;

impl<Ai, Hi> Node<Ai, Hi> {
    pub fn map<Ao, Ho, Ff, Fd>(self, mut dir_cb: Fd, mut file_cb: Ff) -> Node<Ao, Ho>
    where
        Ff: FnMut(Ai) -> Ao,
        Fd: FnMut(Hi, &[Node<Ai, Hi>]) -> Ho,
    {
        self.map_(&mut dir_cb, &mut file_cb)
    }
    
    fn map_<Ao, Ho, Ff, Fd>(self, dir_cb: &mut Fd, file_cb: &mut Ff) -> Node<Ao, Ho>
    where
        Ff: FnMut(Ai) -> Ao,
        Fd: FnMut(Hi, &[Node<Ai, Hi>]) -> Ho,
    {
        match self {
            Node::Atom(f)=>Node::Atom(file_cb(f)),
            Node::List(f, fs)=>{
                Node::List(
                    dir_cb(f, &fs),
                    fs.into_iter().map(|f|f.map_(dir_cb, file_cb)).collect()
                )
            },
        }
    }

    pub fn atom(&self) -> Option<&Ai> {
        match self {
            Node::Atom(a)=>Some(a),
            Node::List(..)=>None,
        }
    }
    pub fn list(&self) -> Option<(&Hi, &[Self])> {
        match self {
            Node::Atom(..)=>None,
            Node::List(h, xs)=>Some((h, &xs)),
        }
    }
}

pub fn read_dir(dir: &Path) -> Result<Entry, std::io::Error> {
    if !dir.is_dir() {
        return Ok(Entry::Atom(dir.into()));
    }
    let out: Result<Vec<_>, _> = fs::read_dir(dir)?
        .map(|f| read_dir(&f?.path()))
        .collect();
    Ok(Entry::List(dir.to_owned(), out?))
}

#[cfg(feature = "display")]
mod display {
    use super::*;
    use std::fmt;
    impl fmt::Display for Entry {
        fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
            display_entry(f, self, "", "")
        }
    }

    fn display_entry(
        f: &mut fmt::Formatter<'_>,
        e: &Entry,
        pad: &str,
        segment: &str,
    ) -> Result<(), fmt::Error> {
        f.write_str(pad)?;
        match e {
            Entry::Atom(name) => {
                let name = name.file_name().unwrap().to_str().unwrap();
                f.write_str(name)?;
                f.write_str("\n")?;
            }
            Entry::List(name, fs) => {
                let name = name
                    .file_name()
                    .and_then(|f|f.to_str())
                    .unwrap_or(".");
                f.write_str(name)?;
                f.write_str("\n")?;
                let nextpad = String::from(segment) + "├──";
                let lastpad = String::from(segment) + "└──";
                let nextseg = String::from(segment) + "│  ";
                let lastseg = String::from(segment) + "   ";
                for (idx, fl) in fs.iter().enumerate() {
                    if idx == fs.len() - 1 {
                        display_entry(f, fl, &lastpad, &lastseg)?;
                    } else {
                        display_entry(f, fl, &nextpad, &nextseg)?;
                    }
                }
            }
        };
        Ok(())
    }
}

