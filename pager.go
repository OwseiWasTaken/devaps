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
	txs string
)

func PFS ( f []string, off int ) () {
	//clear()
	pos(0, 0)
	printf("%s───────┬", grey) // ,8
	print(txs+"\n") // ,0
	printf(  " File: │ %s%s\n", white, __fl)
	printf("%s───────┼", grey) // ,8
	print(txs+"\n") // ,0

	printf("\x1b[%d;0H", 3+1+1)
	for i:=0;i<len(f);i++{
		if running && i+1 == len(f){
			break
		}

		// clear
		stdout.WriteString("\x1b[K")
		stdout.WriteString(
		spf("%s%s%d%s │%s %s%s",
			strings.Repeat(" ", 6-(len(spf("%d", i+1+off)))),
			lgrey,
			i+1+off,
			grey,
			white,
			f[i],
			spf("\x1b[%d;0H", i+3+1+1),
		))
		//printf("%s%s%d%s │%s %s\n",
		//	strings.Repeat(" ", 6-(len(spf("%d", i+1+off)))),
		//	lgrey,
		//	i+1+off,
		//	grey,
		//	white,
		//	f[i],
		//)
	}
	stdout.WriteString("\n")
	stdout.Flush()
	stdout.WriteString(grey+"───────┴") // ,8
	stdout.WriteString(txs+"\n") // ,0
	stdout.WriteString(white)
	stdout.Flush()

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


	txs = strings.Repeat("─", termx - 8)
	printf("%s───────┬", grey) // ,8
	print(txs+"\n") // ,0
	printf("%s		 │%s File: %s\n", grey, white, __fl)
	printf("%s───────┼", grey) // ,8
	print(txs+"\n") // ,0
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

