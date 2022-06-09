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
		"✓",
		"+",
		"→",
		"↑",
		"↓",
		"🖉", // i monospace goes brrr
	}
	flags = make([]bool, FlagLen)
)

func main () {
	InitGu()
	var (
		GSOut string
		GD bool // git directory
  )

	Out, err := exec.Command("git", "status").Output()
	GSOut = string(Out)
	panic(err)

	for i:=0;i<FlagLen;i++ {
		// PS(FlagTexts[i])
		// PS(strings.Contains(GSOut, FlagTexts[i]))
		// print("\n")
		flags[i] = strings.Contains(GSOut, FlagTexts[i])
		GD = GD || flags[i]
	}

	if GD {
		printf("(")
		if flags[0] {
			printf(RGB(60, 255, 60))
		} else if flags[3] {
			printf(RGB(60, 60, 255))
		} else {
			printf(RGB(255, 60, 60))
		}
		for i:=0;i<FlagLen;i++ {
			if flags[i] {
				printf(FlagIcons[i])
			}
		}
		printf(RGB(255, 255, 255))
		printf(")")
	}

	exit(0)
}
