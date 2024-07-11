use crate::*;
use std::fmt;

pub(crate) mod rot {
    use super::*;
    pub struct Export<'a>(pub(crate) &'a Graph);

    impl fmt::Display for Export<'_> {
        fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
            self.0
                .nodes
                .iter()
                .map(|n| self.display_node(f, n))
                .collect::<Result<_, _>>()?;
            self.0
                .links
                .iter()
                .map(|n| self.display_link(f, n))
                .collect::<Result<_, _>>()
        }
    }

    impl Export<'_> {
        fn display_link(&self, f: &mut fmt::Formatter<'_>, l: &Link) -> fmt::Result {
            let from = &self.0.get_node_by_id(l.from_node_id).unwrap().name;
            let to = &self.0.get_node_by_id(l.to_node_id).unwrap().name;
            write!(f, "{from}->{to}")?;
            self.display_props(f, &l.props)?;
            f.write_str("\n")
        }
        fn display_node(&self, f: &mut fmt::Formatter<'_>, n: &Node) -> fmt::Result {
            f.write_str(&n.name)?;
            self.display_props(f, &n.props)?;
            f.write_str("\n")
        }
        fn display_props(
            &self,
            f: &mut fmt::Formatter<'_>,
            p: &HashMap<String, String>,
        ) -> fmt::Result {
            f.debug_map().entries(p).finish()
        }
    }
}

pub(crate) mod dot {
    use super::*;
    #[allow(dead_code)]
    pub struct Export<'a>(pub(crate) &'a Graph);
}
pub(crate) mod svg {
    use super::*;
    #[allow(dead_code)]
    pub struct Export<'a>(pub(crate) &'a Graph);
}

pub mod to {
    use super::*;
    pub fn rot<'a>(g: &'a Graph) -> rot::Export<'a> {
        rot::Export(g)
    }
    pub fn dot<'a>(g: &'a Graph) -> dot::Export<'a> {
        dot::Export(g)
    }
    pub fn svg<'a>(g: &'a Graph) -> svg::Export<'a> {
        svg::Export(g)
    }
}
