include "gutil"

func Make_FS ( f fs.FileInfo ) ( _fs ) {
	m := uint32(f.Mode())
	if f.IsDir() {
		m -= 2147483648 // (1<<32)/2
	}
	x := _fs{
		f.Name(),
		f.IsDir(),

		// user
		m & 0b001000000 != 0,
		m & 0b010000000 != 0,
		m & 0b100000000 != 0,

		// group
		m & 0b000001000 != 0,
		m & 0b000010000 != 0,
		m & 0b000100000 != 0,

		// other
		m & 0b000000001 != 0,
		m & 0b000000010 != 0,
		m & 0b000000100 != 0,

		f.Size(),
	}
	return x
}

type _fs struct {
	name string
	dir bool

	exec bool
	write bool
	read bool

	GroupExec bool
	GroupWrite bool
	GroupRead bool

	OtherExec bool
	OtherWrite bool
	OtherRead bool

	size int64
}

func PrintFs ( f _fs ) ( int ) {
	var color string
	color = RGB(55, 55, 55)
	if (f.dir) {
		color = RGB(0, 0, 255)
	} else if (f.write && f.read && !f.exec) {
		color = RGB(255, 255, 255)
	} else if (f.write && f.read && f.exec) {
		color = RGB(0, 255, 0)
	}
	printf("%s%s ", color, f.name)
	return len(f.name)+1
}

func ListDir ( dirname string ) () {
	s:=_ls(dirname)
	fls := make([]_fs, len(s))
	_, termx := GetTerminalSize()

	i:=0
	for ;i<len(s);i++ {
		fls[i] = Make_FS(s[i])
	}

	i=0
	l:=0
	for ;i<len(s);i++ {
		if len(fls[i].name)+l > termx {
			print("\n")
			l = 0
		}
		l+=PrintFs(fls[i])
	}
	PS(l)
}

