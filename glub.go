package main

import (
	"fmt"
	"io"
	"os"
)

const (
	SizeArray = 0
	// 1..15 = defined size
	SizeMax = 15
)

const (
	HeadCustom = 0
	HeadString = 1
	HeadInt    = 2
	HeadMax    = 3
)

// 1(HTYPE,HSIZE)[SERO]size(DATA)

const (
	// SizeExtendedReadOverflowFlag
	SEROF = 0b10000000
	SEROF_Offset = 7
)

// split info head into type, size
func SplitIH(head byte) (HType, HSize int) {
	HType = int(head)>>4
	HSize = int(head)&0x0F
	return
}

type Stream struct {
	Content []byte
	Index int
	Len int
	Readers [HeadMax]GlubReader
}

type GlubReader = func(S *Stream, size int)(a interface{}, e error)

func (S *Stream) Can(size int) bool {
	return S.Len>=(size+S.Index)
}

func (S *Stream) Single() (byte) {
	S.Index++
	return S.Content[S.Index-1]
}

func (S *Stream) Array(size int) (dst []byte) {
	dst = make([]byte, size)
	for i:=0;i<size;i++ {
		dst[i] = S.Content[S.Index+i]
	}
	S.Index+=size
	return
}

func main() {
	if (len(os.Args) != 2) {
		fmt.Fprintf(os.Stderr, "\n")
		os.Exit(1)
	}
	var FileName = os.Args[1]
	file, err := os.Open(FileName)
	if (err != nil) { panic(err) }

	content, err := io.ReadAll(file)
	if (err != nil) { panic(err) }

	var FileStream = Stream{ content, 0, len(content), [HeadMax]GlubReader{}}
	FileStream.Readers[HeadString] = ReadString

	fmt.Printf("%+v\n", FileStream)
	for FileStream.Can(1) {
		c, e := FileStream.Next()
		if (e != nil) {
			fmt.Println(e)
		} else {
			fmt.Printf("%+v\n", c)
		}
	}
}

func (S *Stream) Next() (any, error) {
	if (!S.Can(1)) {
		return nil, fmt.Errorf("S.Next: Can't read")
	}

	HType, HSize := SplitIH(S.Single())
	Reader := S.Readers[HType]
	if (Reader == nil) {
		return nil, fmt.Errorf("Can't find S.Reader for HType %d", HType)
	}
	return Reader(S, HSize)
}

func ReadString(S *Stream, size int) (interface{}, error) {
	if (size > SizeMax) {
		var fin = SEROF
		for fin&SEROF!=0{
			if (!S.Can(1)) {
				return nil, fmt.Errorf("Can't read SEROF")
			}
			fin := S.Single()
			size+=int(fin)^SEROF
		}
	}

	if (!S.Can(size)) {
		return nil, fmt.Errorf("S.Readers[ReadStringLDT] can't read specified size of %d", size)
	}

	return S.Array(size), nil
}

