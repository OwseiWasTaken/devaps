package main

include "gutil"

func CompareBytes(ba1, ba2 []byte) (bool) {
	for i:=0;i<len(ba1);i++ {
		if ba1[i] != ba2[i] {return false}
	}
	return true
}

func IATC(IA []int) (string) {
	return RGB(IA[0], IA[1], IA[2])
}

func main(){

	var (
		CharBuff []byte
		ClearChar = "\033[2K"
		e error
		HadError bool
		ri int
		gi int
		bi int
		IdToColorS = map[int]string{
			0:"red",
			1:"green",
			2:"blue",
		}
		cla = []uint8{255, 255, 255}
		ColorId = 0 // red
		IncId = func() () {
			ColorId = (ColorId+1)%3
		}
		DecId = func() () {
			ColorId = (ColorId+2)%3
		}
		IncCol = func() () { cla[ColorId] = ( cla[ColorId]+1 ) }
		DecCol = func() () { cla[ColorId] = (cla[ColorId]+255) }
		SIncCol = func() () { cla[ColorId] = ( cla[ColorId]+10 ) }
		SDecCol = func() () { cla[ColorId] = (cla[ColorId]+246) }
		oz []int
		TestText = "Lorem Ipsum"
	)

	InitGu()
	InitGetCh()
	clear()
	var (
		up	  = []byte{27, 91, 65}
		down  = []byte{27, 91, 66}
		right = []byte{27, 91, 67}
		left  = []byte{27, 91, 68}
		enter = []byte{10}
		dcol  = []byte{58}
		t     = []byte{116}
		sup   = []byte{27, 91, 49, 59, 50, 65}
		sdown = []byte{27, 91, 49, 59, 50, 66}
		//TODO cup/cdown = all++
		//TODO scup/scdown = all+10
	)

	HideCursor()
	oz = []int{0, 0, 0}
	for {
		pos(0, 0)

		print(ClearChar)
		for i:=0;i<3;i++{
			oz[i] = int(cla[i])
			if ( i == ColorId ) {
				printf("%s\x1b[48;2;255;255;255m%s\x1b[48;2;0;0;0m%s:%d ",
					IATC(oz), IdToColorS[i], COLOR["nc"], cla[i],
				)
			} else {
				printf("%s%s%s:%d ",
					IATC(oz), IdToColorS[i], COLOR["nc"], cla[i],
				)
			}
			oz[i] = 0
		}

		printf("\n%s%s█████ █████\n%s%s%s\n",
			ClearChar, RGB(cla[0], cla[1], cla[2]), ClearChar, TestText, COLOR["nc"],
		)
		stdout.Flush()
		CharBuff = make([]byte, 5)
		CharBuff = GetChByte(stdin)

		pos(8, 0)
		printf("%s%v", ClearChar, CharBuff)

		if (CompareBytes(sup, CharBuff)) {SIncCol()} else
		if (CompareBytes(sdown, CharBuff)) {SDecCol()} else
		if (CompareBytes(up, CharBuff)) {IncCol()} else
		if (CompareBytes(down, CharBuff)) {DecCol()} else
		if (CompareBytes(right, CharBuff)) {IncId()} else
		if (CompareBytes(left, CharBuff)) {DecId()} else
		if (CompareBytes(enter, CharBuff)) {break}
		if (CompareBytes(dcol, CharBuff)) {
			pos(2, 0)
			ShowCursor()
			print("\nR:")
			r:=oldinput()
			print("\nG:")
			g:=oldinput()
			print("\nB:")
			b:=oldinput()

			ri, e = strconv.Atoi(r)
			HadError = (e!=nil)
			gi, e = strconv.Atoi(g)
			HadError = HadError||(e!=nil)
			bi, e = strconv.Atoi(b)
			HadError = HadError||(e!=nil)
			if !HadError {
				if ((ri < 255) && (gi < 255) && (bi < 255)) {
					cla[0] = uint8(ri)
					cla[1] = uint8(gi)
					cla[2] = uint8(bi)
				}
			}
			pos(2, 0)
			print("\n"+ClearChar)
			print("\n"+ClearChar)
			print("\n"+ClearChar)

			HideCursor()
		}
		if (CompareBytes(t, CharBuff)) {
			pos(2, 0)
			print(ClearChar+RGB(cla[0], cla[1], cla[2]))
			ShowCursor()
			TestText = oldinput()
			print(COLOR["nc"])
			HideCursor()
		}
	}
	pos(0,0)
	ShowCursor()
	printf("%sgcall: RGB(%d, %d, %d)\n", ClearChar, cla[0], cla[1], cla[2])
	printf("%sansii: \"\\x1b[38;2;%d;%d;%dm\"\n", ClearChar, cla[0], cla[1], cla[2])
	exit(0)
}
