include "gutil"
include "control"

func read() ([]byte) {
	return GetChByte()
}

func Compress( x []byte ) ( string ) {
	buff := ""
	for i:=0;i!=6;i++{
		if (x[i] == 0) { break }
		buff+=spf("%.3d,", x[i])
		lk = append(lk, x[i])
	}
	return buff
}


func wgtk ( ) ( string ) {
	x:=read()
	lk = []byte{}
	e, ok := Control[Compress(x)]
	if (!ok) {
		return "NULL"
	}
	return e
}

var (
	lk []byte
	k string
	y int
	PgFl []string
	running bool = false
	termy int
	termx int
	grey = RGB(50, 50, 50)
	lgrey = RGB(180, 180, 180)
	white = RGB(255, 255, 255)
	__fl string
)

func PFS ( f []string, off int ) () {
	clear()
	printf("%s───────┬", grey) // ,8
	print(strings.Repeat("─", termx - 8)+"\n") // ,0
	printf("%s       │%s File: %s\n", grey, white, __fl)
	printf("%s───────┼", grey) // ,8
	print(strings.Repeat("─", termx - 8)+"\n") // ,0

	for i:=0;i<len(f);i++{
		if running && i+1 == len(f){
			break
		}
		printf("%s%s%d%s │%s %s\n",
			strings.Repeat(" ", 6-(len(spf("%d", i+1+off)))),
			lgrey,
			i+1+off,
			grey,
			white,
			f[i],
		)
	}
	printf("%s───────┴", grey) // ,8
	print(strings.Repeat("─", termx - 8)+"\n") // ,0
	print(white)
}

func PrintFile ( f []string, offset int ) () {
	if len(f) < termy {
		PFS(f, 0)
	} else {
		PFS(f[offset:termy+offset-4], offset)
	}
}

func Pagefile ( flname string ) () {
	__fl = flname

	termy, termx = GetTerminalSize()

	PgFl = strings.Split(ReadFile(flname), "\n")
	if len(PgFl) > termy {
		running = true
		InitGetCh()
	}

	PrintFile(PgFl, 0)

	for ;running;{

		printf(":")
		k = wgtk()
		switch (k){
		case "q":
			clear()
			running = false
		case "up", "k":
			if (y!=0){
				y--
			}
		case "down", "j", "enter":
			if (y-4+termy!=len(PgFl)){ // -4 = HUD
				y++
			}
		}

		if y > 0 {
			PrintFile(PgFl, y)
		} else {
			PrintFile(PgFl, 0)
		}
	}

}

