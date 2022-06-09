package main

include "gutil"
include "abspath"


type conv struct { place string
	replace string
	color string
}

func main() {
	var (
		file []string
		i int
		line []string
		stuff []conv
		color string
		pwd string
		cl []string
		flname string
		err error
		cfgfl AbsPath
	)
	out, err := exec.Command("pwd").Output()
	// btw, panic only "panics" if err != nil
	panic(err)
	pwd = string(out)
	cfgfl, err = HomeDir()
	panic(err)
	flname = cfgfl.String()

	if !exists(flname+"/.config/spwd.xmp") {
		if !exists(flname+"/.config") {
			dprint(stderr, "ERROR", "can't find %s/.config folder!", flname)
			exit(2)
		}
		WriteFile(flname+"/.config/spwd.xmp", "\n")
	}

	cfgfl, err = ExpandFrom(flname+"/.config/spwd.xmp")
	flname = cfgfl.String()

	file = strings.Split(ReadFile(flname), "\n") // not really a .xmp file

	for i=0;i<len(file);i++ {
		line = strings.Split(file[i], " ")
		line[0] = strings.Replace(line[0], "\t", "", -1)
		if len(line) == 1 {continue}
		if len(line) == 0 {continue}
		if (line[0][0] == '#'){continue}
		if len(line) == 2 {
			color = RGB(30, 30, 240)
		} else {
			color = line[2]
			cl = strings.Split(color, ",")
			color = RGB(cl[0], cl[1], cl[2])
		}
		stuff = append( stuff, conv{
			line[0],
			line[1],
			color,
		})
	}
	color = RGB(30, 30, 240)


	for i:=0;i<len(stuff);i++ {
		if (pwd != strings.Replace(pwd, stuff[i].place, stuff[i].replace, 1)){
			color = stuff[i].color
		}
		pwd = strings.Replace(pwd, stuff[i].place, stuff[i].replace, 1)
	}

	pwd = strings.Replace(pwd, "\n", "", -1)
	printf(color+pwd+RGB(255,255,255))

	exit(0)
}
