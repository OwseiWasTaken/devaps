package main

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
	InitGu()

	fls = get("").List
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
