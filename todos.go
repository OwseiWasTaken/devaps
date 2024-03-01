package main

import (
	"os"
	"io"
	"fmt"
	"strings"
	"regexp"
)

type Finder struct {
	canPrefix, canSuffix bool
	ext string
	finder *regexp.Regexp
}

func (F Finder) Match(line string) (ColOff, ColSpan int, Found bool) {
	loc := F.finder.FindStringIndex(line)
	if (loc == nil) {return 0, 0, false}
	return loc[0], loc[1], true
}

func (F Finder) Should(filename string) bool {
	if (F.canPrefix && F.canSuffix) {
		return strings.Contains(filename, F.ext)
	}
	if (F.canPrefix) {
		return strings.HasSuffix(filename, F.ext)
	}
	if (F.canSuffix) {
		return strings.HasPrefix(filename, F.ext)
	}
	return filename==F.ext
}

const (
	LINE_HL = "\x1b[91m"
	RESET = "\x1b[0m"
	POS_HL = "\x1b[92m"
	FILE_HL = "\x1b[95m"
)

func HLLine(s string, off, span int) string {
	return strings.TrimSpace(s[:off]+LINE_HL+s[off:span]+RESET+s[span:])
}

func ReadAll(filename string) (string, error) {
	file, err := os.Open(filename)
	if (err != nil) {return "", err}
	defer file.Close()
	cont, err := io.ReadAll(file)
	if (err != nil) {return "", err}
	return string(cont), nil
}
// find TODO finders
var finderFinder = regexp.MustCompile(`(\*)?(\S+?)(\*)?\t+(.*)`)

func main() {
	var HOME = os.Getenv("HOME")
	config, err := ReadAll(HOME+"/.config/todos")
	if (err != nil) {
		fmt.Fprintf(os.Stderr, "No such file $HOME/.config/todos; finders must be placed there\n")
		os.Exit(1)
	}

	var finders = []Finder{}

	lines := strings.Split(config, "\n")
	for _, line := range lines {
		var info = finderFinder.FindStringSubmatch(line)
		if len(info) == 5 {
			rgx, err := regexp.Compile(info[4])
			if (err!=nil) {
				fmt.Fprintf(os.Stderr, "Could not compile regex %q\n", info[4])
				fmt.Fprintf(os.Stderr, "Proceeding normally\n")
			} else {
				finders = append(finders, Finder{
					info[1]=="*",
					info[3]=="*",
					info[2],
					rgx,
				})
			}
		} //TODO else and len() != 0
	}

	files := os.Args[1:]

	for _, filename := range files {
		cont, e := ReadAll(filename)
		if (e!=nil) {panic(e)}
		lines := strings.Split(cont, "\n")
		thisfinders := []Finder{}
		for _, finder := range finders {
			if (finder.Should(filename)) {
				thisfinders = append(thisfinders, finder)
			}
		}

		for LineOff, line := range lines {
			for _, finder := range thisfinders {
				colof, colspan, found := finder.Match(line)
				if (!found) {continue}
				fmt.Printf(FILE_HL+"%s"+RESET+":"+POS_HL+"%d:%d"+RESET+": \"%s\"\n", filename, LineOff+1, colof, HLLine(line, colof, colspan))
			}
		}
	}
}

