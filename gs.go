package main

include "gutil"

// flag index
const (
	Clean = iota
	Utracked
	uncommited
	restore
	FlagLen
)


// flag texts
var (
	FlagTexts = []string{
		"working tree clean",
		"Untracked files",
		"Changes to be committed",
		"git restore",
		"FLAG LEN",
	}
	FlagIcons = []string{
		"✓",
		"+",
		"↑",
		"",//TODO find monospace pencil
		"!",
	}
	flags = make([]bool, FlagLen)
)

func main () {
	InitGu()
	var (
		GSOut string
		flags []bool
  )

	GSOut, err := exec.Command("git", "status").Output()
	panic(err)

	for (i:=0;i<FlagLen;i++) {
		flags[i] = strings.Contains(GSOut, FlagTexts[i])
	}
	//TODO print flags' icons
	exit(0)
}
