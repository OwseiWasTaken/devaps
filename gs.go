package main

include "gutil"

// flag index
const (
	Clean = iota
	Utracked = iota
	uncommited = iota
	unpushed = iota
	unpulled = iota
	restore = iota
	FlagLen = iota
)


// flag texts
var (
	FlagTexts = []string{
		"working tree clean",
		"Untracked files",
		"Changes to be committed",
		"Your branch is ahead",
		"behind",
		"Changes not staged for commit",
	}
	FlagIcons = []string{
		"ü",
		"+",
		"→",
		"↑",
		"↓",
		"*",
	}
	flags = make([]bool, FlagLen)
)

func main () {
	InitGu()
	var (
		GSOut string
		branch string
  )

	Out, err := exec.Command("git", "status").Output()

	// not git directory
	if err != nil{
		exit(0)
	}

	GSOut = string(Out)
	branch = strings.Split(GSOut, "\n")[0]
	branch = strings.Join(strings.Split(branch, " ")[2:], " ")

	for i:=0;i<FlagLen;i++ {
		flags[i] = strings.Contains(GSOut, FlagTexts[i])
	}


	if branch != "master" && branch != "main" {
		GSOut = "("+branch+" "
	} else {
		GSOut = "("
	}

	if flags[0] {
		GSOut += RGB(60, 255, 60)
	} else if flags[3] {
		GSOut += RGB(60, 60, 255)
	} else {
		GSOut += RGB(255, 60, 60)
	}

	for i:=0;i<FlagLen;i++ {
		if flags[i] {
			GSOut += FlagIcons[i]
		}
	}

	GSOut += RGB(255, 255, 255) + ")"
	printf(GSOut)

	exit(0)
}
