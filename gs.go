package main

import (
	"fmt"
	"strings"
	"os/exec"
)

type Status struct {
	Text string
	Icon rune
	Color string
}

func (s Status) Format() string {
	return fmt.Sprintf("\x1b[%sm%c", s.Color, s.Icon)
}

func (s Status) Match(GSText string) bool {
	return strings.Contains(GSText, s.Text)
}

var (
	// could use map[string]rune
	// but would have random order of icons
	StatusList = []Status{
		{"working tree clean"           , 'ü', "0;32"},
		{"Untracked files"              , '+', "0;31"},
		{"Changes to be committed"      , '→', "0;31"},
		{"Your branch is ahead"         , '↑', "5;34"},
		{"behind"                       , '↓', "5;91"},
		{"Changes not staged for commit", '*', "0;31"},
	}
)

func main() {
	Out, err := exec.Command("git", "status").Output()
	if (err != nil) {panic(err)}
	GSText := string(Out)

	branchline := strings.Split(GSText, "\n")[0]
	branchname := strings.TrimPrefix(branchline, "On branch ")

	var icons string
	for _, status := range StatusList {
		if status.Match(GSText) {
			icons+=status.Format()
		}
	}

	fmt.Printf("\x1b[1;95m%s %s\x1b[0m", branchname, icons)
}
