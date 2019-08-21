package pb2json

/*
#cgo CXXFLAGS: -std=c++11
#cgo LDFLAGS: -L. -lpb2json -lprotobuf
#include <stdlib.h>
#include "pb2json.h"
*/
import "C"
import (
	"fmt"
	"unsafe"
)

func Init(path, file string) (err error) {
	cpath := C.CString(path)
	cfile := C.CString(file)
	ret := C.init(cpath, cfile)
	if ret != 0 {
		err = fmt.Errorf("init failed. %d", ret)
	}
	C.free(unsafe.Pointer(cpath))
	C.free(unsafe.Pointer(cfile))
	return err
}

func Pb2Json(msgName string, data []byte, dataSize int) (json []byte, err error) {
	var output *C.char
	var size C.int
	cMsgName := C.CString(msgName)
	ret := C.dynamic_decode(cMsgName, (*C.char)(unsafe.Pointer(&data[0])), C.int(dataSize), &output, &size)
	if ret != 0 {
		C.free(unsafe.Pointer(cMsgName))
		err = fmt.Errorf("decode failed. %d", ret)
		fmt.Println("ret: ", ret, err)
		return
	}

	C.free(unsafe.Pointer(cMsgName))
	json = C.GoBytes(unsafe.Pointer(output), size)
	C.free(unsafe.Pointer(output))

	return
}

// for testing
func encode() (data []byte, err error) {
	msgName := "omg.new_app.GSessionItem"
	var res *C.char
	var size C.int
	cMsgName := C.CString(msgName)
	ret := C.dynamic_encode(cMsgName, &res, &size)
	if ret != 0 {
		C.free(unsafe.Pointer(cMsgName))
		err = fmt.Errorf("encode failed. %d", ret)
		return
	}
	C.free(unsafe.Pointer(cMsgName))
	data = C.GoBytes(unsafe.Pointer(res), size)
	C.free(unsafe.Pointer(res))

	return
}

/*
func main() {
	path := "/data/home/wenhuijia/project/src/RecSys/"
	file := "NewApp/Proto/Profile.proto"
	err := Init(path, file)
	if err != nil {
	}
	data, err := encode()
	if err != nil {
	}
	msgName := "omg.new_app.GSessionItem"
	json, err := Pb2Json(msgName, data)
	fmt.Println(json)
}
*/
