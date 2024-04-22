# gs2-rs git status for your PS1

gs2-rs was made because executing $git status while libgit2 existed seemed inefficient
while I didn't benchmark this program, counting the amount of syscalls gives a rought ideia.

program     | sys calls
------------|-----------
bin/gs      | 758
$git status | 532
bin/gs2     | 496

## gs2-rs is still a work in progress.

it can't comunicate with the remote repo, therefore it can't know if you need
to push or pull The plan is to add a --fetch option to execute a fetch when
executing gs2 to know the state of the local repo in relation to the remote
