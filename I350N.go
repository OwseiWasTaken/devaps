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

type T350N_Reader = func(S *ReadStream, size int)(a *Taped, e error)
type T350N_Writer = func()(HType, HSize int, bytes []byte)

const (
	HeadIgnore = 0
	HeadString = 1
	HeadInt    = 2
	HeadMap_AnyAny = 3
	HeadMap_StringString = 5
	HeadMap_StringInt = 6
	HeadMax    = 7
	HeadCustom = 0
)

var HeadToName = map[int]string{
	HeadIgnore: "HeadIgnore",
	HeadString: "string",
	HeadInt: "int64",
	HeadMap_AnyAny: "map[any]any",
	HeadMap_StringString: "map[string]string",
	HeadMap_StringInt: "map[string]int",
}

var (
	T350N_Readers = [HeadMax]T350N_Reader{
		HeadIgnore: ReadIgnore,
		HeadString: ReadString,
		HeadInt: ReadInt,
		HeadMap_AnyAny: ReadMap_AnyAny,
		HeadMap_StringString: ReadMap_StringString,
		HeadMap_StringInt: ReadMap_StringInt,
	}
)

type Taped struct {
	Value interface{}
	HType int
}

func (T Taped) Comparable() (interface{}) {
	switch (T.HType) {
	default:
		panic(fmt.Errorf("Can't resolve [%d] into comparable", T.HType))
	case HeadString:
		return string(T.Value.([]byte))
	case HeadInt:
		return T.Value
	case HeadIgnore:
		return nil
	}
}

func (T Taped) String() (atr string) {
	switch (T.HType) {
	default:
		panic(fmt.Errorf("Can't resolve [%d] into string", T.HType))
	case HeadString:
		return string(T.Value.([]byte))
	case HeadInt:
		return fmt.Sprintf("%d", T.Value.(int64))
	case HeadMap_AnyAny:
		return fmt.Sprintf("%+v", T.Value)

	case HeadMap_StringString:
		for k,v:=range T.Value.(map[string]string){
			atr+=fmt.Sprintf("%q:%q", k, v)
		}
		return

	case HeadMap_StringInt:
		for k,v:=range T.Value.(map[string]int64){
			atr+=fmt.Sprintf("%q:%d", k, v)
		}
		return
	case HeadIgnore:
		return "<padding>"
	}
}

// [OptExpr]{bytecount}(dataname,datanames)
// {1}(HType,HSize) [HSize==0]{?}(SERO) {SERO+HSize}(DATA)

const (
	// SizeExtendedReadOverflowFlag
	SEROF byte = 0b10000000
	SEROF_Offset = 7
)

// split info head into type, size
func SplitIH(head byte) (HType, HSize int) {
	HType = int(head)>>4
	HSize = int(head)&0x0F
	return
}

type ReadStream struct {
	Content []byte
	Len int
	Readers [HeadMax]T350N_Reader
	Index int
}

func (S *ReadStream) Can(size int) bool {
	return S.Len>=(size+S.Index)
}

func (S *ReadStream) Single() (byte) {
	S.Index++
	return S.Content[S.Index-1]
}

func (S *ReadStream) Array(size int) (dst []byte) {
	dst = make([]byte, size)
	for i:=0;i<size;i++ {
		dst[i] = S.Content[S.Index+i]
	}
	S.Index+=size
	return
}

func main() {
	if (len(os.Args) != 2) {
		fmt.Fprintf(os.Stderr, "usage: %s <file.350N>\n", os.Args[0])
		os.Exit(1)
	}

	// open and read file
	var FileName = os.Args[1]
	file, err := os.Open(FileName)
	if (err != nil) { panic(err) }

	content, err := io.ReadAll(file)
	if (err != nil) { panic(err) }

	// make 350N reader with file content
	var FileStream = ReadStream{
		content, len(content),
		T350N_Readers,
		0,
	}

	var out = []*Taped{}
	for FileStream.Can(1) {
		c, e := FileStream.Next()
		if (e != nil) {
			panic(e)
		} else {
			out = append(out, c)
		}
	}
	for _, item:=range out {
		fmt.Printf("%s{ %s }\n", HeadToName[item.HType], item.String())
	}
}

func (S *ReadStream) Next() (T *Taped, e error) {
	if (!S.Can(1)) {
		return nil, fmt.Errorf("S.Next: Can't read")
	}

	HType, HSize := SplitIH(S.Single())
	Reader := S.Readers[HType]
	if (Reader == nil) {
		return nil, fmt.Errorf("Can't find S.Reader for HType %d", HType)
	}

	// SERO
	if (HSize == SizeArray) {
		HSize, e = ReadSERO(S)
		if (e != nil) {return nil, e}
	}
	return Reader(S, HSize)
}

func ReadSERO(S *ReadStream) (HSize int, e error) {
	var fin = SEROF
	for {
		if (!S.Can(1)) {
			return HSize, fmt.Errorf("Can't read SEROF")
		}
		fin = S.Single()
		if (fin&SEROF==0) { // end SERO
			HSize+=int(fin)
			break
		} else {
			HSize+=int(fin-SEROF)
		}
	}
	return HSize, nil
}

func ReadString(S *ReadStream, size int) (*Taped, error) {
	if (!S.Can(size)) {
		return nil, fmt.Errorf("S.Readers[ReadString] can't read specified size of %d", size)
	}

	return &Taped{S.Array(size), HeadString}, nil
}

func ReadIgnore(S *ReadStream, size int) (*Taped, error) {
	return &Taped{nil, HeadIgnore}, nil
}

func ReadInt(S *ReadStream, size int) (*Taped, error) {
	if (!S.Can(size)) {
		return nil, fmt.Errorf("S.Readers[ReadInt] can't read specified size of %d", size)
	}
	if (size > 8) {
		return nil, fmt.Errorf("S.Readers[ReadInt] can't read more than 8 bytes into int, trying to read %d", size)
	}

	var sum int64
	for i:=0;i<size;i++ {
		sum=(sum<<8)+int64(S.Single())
	}
	return &Taped{sum, HeadInt}, nil
}

func ReadMap_AnyAny(S *ReadStream, entries int) (Map *Taped, E error) {
	var m = make(map[interface{}]interface{})
	var Key, Value *Taped
	for i:=0;i<entries;i++ {
		Key, E = S.Next()
		if (E != nil){ return }
		Value, E = S.Next()
		if (E != nil){ return }
		m[Key.Comparable()] = Value.Value
	}
	return &Taped{m, HeadMap_AnyAny}, nil
}

func ReadMap_StringString(S *ReadStream, entries int) (Map *Taped, E error) {
	var m = make(map[string]string)
	var Key, Value *Taped
	var KSize, VSize int
	for i:=0;i<entries;i++ {
		KSize, E = ReadSERO(S)
		if (E != nil){ return }
		Key, E = ReadString(S, KSize)
		if (E != nil){ return }

		VSize, E = ReadSERO(S)
		if (E != nil){ return }
		Value, E = ReadString(S, VSize)
		if (E != nil){ return }

		m[Key.String()] = Value.String()
	}
	return &Taped{m, HeadMap_StringString}, nil
}

func ReadMap_StringInt(S *ReadStream, entries int) (Map *Taped, E error) {
	var m = make(map[string]int64)
	var Key, Value *Taped
	var KSize, VSize int
	for i:=0;i<entries;i++ {
		KSize, E = ReadSERO(S)
		if (E != nil){ return }
		Key, E = ReadString(S, KSize)
		if (E != nil){ return }

		VSize, E = ReadSERO(S)
		if (E != nil){ return }
		Value, E = ReadInt(S, VSize)
		if (E != nil){ return }

		m[Key.String()] = Value.Value.(int64)
	}
	return &Taped{m, HeadMap_StringInt}, nil
}

