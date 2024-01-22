package main

import (
	"fmt"
)

type Iterator[Source, Out any] struct {
	arr []Source
	fn func(Source, int)(Out, bool)
}

func Iter[In, Out any](
	arr []In,
	fn func(In, int)(Out, bool),
) Iterator[In, Out] {
	return Iterator[In, Out]{
		arr, fn,
	}
}

func Apply[Source, In, Out any] (
	I Iterator[Source, In],
	fn func(In, int)(Out, bool),
) Iterator[Source, Out] {

	nfn := func(S Source, index int)(O Out, K bool) {
		v, K := I.fn(S, index)
		if (!K) {return}
		O, K = fn(v, index)
		return
	}

	return Iterator[Source, Out]{
		I.arr, nfn,
	}
}

func Collect[Source, Out any](I Iterator[Source, Out]) (oarr []Out) {
	oarr = []Out{}
	for index, item := range I.arr {
		var v, add = I.fn(item, index)
		if (add) {
			oarr = append(oarr, v)
		}
	}
	return
}

func INTNOP(S int, index int) (int, bool) {
	return S, true
}

func Double(v int, index int) (int, bool) {
	return v*2, true
}

func KeepEven(v int, _ int) (int, bool) {
	return v, v%2==0
}

func main() {
	a := Iter([]int{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, INTNOP)
	c := Apply(Apply(a, KeepEven), Double)
	fmt.Println(Collect(a))
	fmt.Println(Collect(c))
}

