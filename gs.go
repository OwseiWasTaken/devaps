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
		"🖉",// i monospace goes brrr
	}
	flags = make([]bool, FlagLen)
)

func main () {
	InitGu()
	var (
		GSOut string
	//	GD bool // git directory
  )

	Out, err := exec.Command("git", "status").Output()
	GSOut = string(Out)

	// not git directory
	if err != nil{
		exit(0)
	}

	for i:=0;i<FlagLen;i++ {
		// PS(FlagTexts[i])
		// PS(strings.Contains(GSOut, FlagTexts[i]))
		// print("\n")
		flags[i] = strings.Contains(GSOut, FlagTexts[i])
		//GD = GD || flags[i]
	}

	GSOut = ""

	//if GD {
		GSOut += "("
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
	//}
	GSOut = strings.Replace(GSOut, " ", "", -1)
	printf(GSOut)

	exit(0)
}
