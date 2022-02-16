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
		red = RGB(255, 0, 0)
		green = RGB(0, 255, 0)
		blue = RGB(0, 0, 255)
		IdToColor = map[int]string{
			0:red,
			1:green,
			2:blue,
		}
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
	)

	InitGu()
	InitGetCh()
	clear()
	var (
		up	  = []byte{27, 91, 65, 0, 0}
		down  = []byte{27, 91, 66, 0, 0}
		right = []byte{27, 91, 67, 0, 0}
		left  = []byte{27, 91, 68, 0, 0}
		enter = []byte{10, 0, 0, 0, 0}
	)

	pos(0, 0)
	for i:=0;i<3;i++{
		if (i == ColorId) {
			printf("%s%s%s:%d ",
				IdToColor[i], IdToColorS[i], COLOR["nc"], cla[i],
			)
		} else {
			printf("%s:%d ",
				IdToColorS[i], cla[i],
			)
		}
	}
	printf("\n%s█████ █████\nLorem Ipsum%s\n",
		RGB(cla[0], cla[1], cla[2]), COLOR["nc"],
	)
	HideCursor()
	for {
		CharBuff = make([]byte, 5)
		CharBuff = GetChByte()
		clear()
		if (CompareBytes(up, CharBuff)) {IncCol()} else
		if (CompareBytes(down, CharBuff)) {DecCol()} else
		if (CompareBytes(right, CharBuff)) {IncId()} else
		if (CompareBytes(left, CharBuff)) {DecId()} else
		if (CompareBytes(enter, CharBuff)) {break}
		pos(0, 0)

		for i:=0;i<3;i++{
			if (i == ColorId) {
				printf("%s%s%s:%d ",
					IdToColor[i], IdToColorS[i], COLOR["nc"], cla[i],
				)
			} else {
				printf("%s:%d ",
					IdToColorS[i], cla[i],
				)
			}
		}

		printf("\n%s█████ █████\nLorem Ipsum%s\n",
			RGB(cla[0], cla[1], cla[2]), COLOR["nc"],
		)
	}
	ShowCursor()
	printf("gcall: RGB(%d, %d, %d)\n", cla[0], cla[1], cla[2])
	printf("ansii: \"\\x1b[38;2;%d;%d;%dm\"\n", cla[0], cla[1], cla[2])
	exit(0)
}
