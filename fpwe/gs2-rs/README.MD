gs2-rs was made because executing $git status while libgit2 existed seemed inefficient
while I didn't benchmark this program, counting the amount of syscalls gives a rought ideia.

program     | sys calls
------------|-----------
bin/gs      | 758
$git status | 532
bin/gs2     | 496
