package main

// don't use this!
// it's not well made
// and it's not working

include "gutil"
include "pager"
include "lister"

var (
	fls []string
	fl string
	fli fs.FileInfo
	err error
)

func ShowDir (dirname string) () {
	ListDir( dirname )
}

func ShowFile (flname string) () {
	Pagefile( flname )
}

func main(){
	//InitGu()
	_clear = make(map[string]func()) //Initialize funcs map
	_clear["linux"] = func() {
		cmd := exec.Command("/usr/bin/clear")
		cmd.Stdout = os.Stdout
		cmd.Run()
	}
	_clear["windows"] = func() {
		cmd := exec.Command("cmd", "/c", "cls")
		cmd.Stdout = os.Stdout
		cmd.Run()
	}

	fls = get("").List
	if len(fls) == 0 {
		fls = []string{"."}
	}

	for i:=0;i<len(fls);i++ {
		fl = fls[i]
		fli, err = os.Stat(fl)

		if ( err != nil ) {
			printf("%s[cat error]%s: '%s' No such file\n", COLOR["red"], COLOR["nc"], fl)
		} else {
			if (fli.IsDir()) {
				ShowDir(fl)
			} else {
				ShowFile(fl)
			}
		}
	}

	exit(0)
}
