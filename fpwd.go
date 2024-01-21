package main

import (
	"fmt"
	"os"
	"io"
	"strings"
)

type Out struct {
	Path string
	Color string
}

func (O Out)String() string {
	var col = strings.Replace(O.Color, "\\e", "\x1b", -1)
	return RESET+col+O.Path+RESET
}

type Dir struct {
	Path string
	Sub string
	Color string
}

func (Dir Dir) Mut(out Out) (Out) {
	if strings.HasPrefix(out.Path, Dir.Path) {
		out.Path = Dir.Sub+strings.TrimPrefix(out.Path, Dir.Path)
		if (Dir.Color != "") {
			out.Color = Dir.Color
		}
	}
	return out
}

const RESET = "\x1b[0m"

func main() {
	cwd, err := os.Getwd()
	if (err != nil) {panic(err)}
	var HOME = os.Getenv("HOME")
	var dedits = []Dir{Dir{HOME, "~", ""}}

	config, err := os.Open(HOME+"/.config/fpwd")
	if (err == nil) {
		dtxt, err := io.ReadAll(config)
		if (err != nil) {panic(err)}
		dlines := strings.Split( string(dtxt), "\n" )
		for _, line := range dlines {
			prts := strings.SplitN(line, " ", 3)
			if len(prts) == 2 {
				dedits = append(dedits, Dir{prts[0], prts[1], ""})
			} else if len(prts) == 3 {
				dedits = append(dedits, Dir{prts[0], prts[1], prts[2]})
			}
		}
	}

	var out = Out{cwd, ""}
	for _, edit := range dedits {
		out = edit.Mut(out)
	}

	fmt.Print(out.String())
}

