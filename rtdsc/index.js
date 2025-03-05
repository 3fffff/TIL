import koffi from "koffi"

const kernel32 = koffi.load("kernel32.dll")
const msvcrt = koffi.load("msvcrt.dll")
const rtdsc = koffi.proto('long long rtdsc()');

const DWORD = koffi.alias('DWORD', 'uint32_t');
const HANDLE = koffi.pointer('HANDLE', koffi.opaque());
const HWND = koffi.alias('HWND', HANDLE);

const VirtualAlloc = kernel32.func('uintptr_t* VirtualAlloc(uintptr_t lpAddress, uintptr_t dwSize, uint32_t flAllocationType, uint32_t flProtect)');
const VirtualProtect = kernel32.func('int VirtualProtect(uintptr_t lpAddress,uintptr_t dwSize,uint32_t flAllocationType,_Out_ uint32_t* flProtect)')
const VirtualFree = kernel32.func('int VirtualFree(uintptr_t lpAddress, uintptr_t dwSize, uint32_t dwFreeType)');
const CreateFileA = kernel32.func('HWND CreateFileA(char* filename, DWORD access,DWORD sharing, uintptr_t SecurityAttributes, DWORD mode,DWORD options, HANDLE template)');
const DeviceIoControl = kernel32.func('int DeviceIoControl(HWND device, uint ctlcode,uintptr_t inbuffer, int inbuffersize, uintptr_t outbuffer, int outbufferSize,uintptr_t bytesreturned, uintptr_t overlapped)')
const CloseHandle = kernel32.func('void CloseHandle(HWND hdl)')
const WriteFile = kernel32.func('int WriteFile(HANDLE hFile,uintptr_t inbuffer, int inbuffersize, int lpNumberOfBytesWritten, uintptr_t overlapped)')
const ReadFile = kernel32.func('int ReadFile(HANDLE hFile,uintptr_t inbuffer, int inbuffersize, int lpNumberOfBytesWritten, uintptr_t overlapped)')
const SetupComm = kernel32.func('int SetupComm(HANDLE hFile,DWORD  dwInQueue,DWORD  dwOutQueue)')
const GetHandleInformation  = kernel32.func("int GetHandleInformation(HWND hObject,uintptr_t* lpdwFlags)")
const CopyMemory = msvcrt.func('void memcpy(void* Destination,const void* Source, uint32_t Length)')
const PAGE_EXECUTE_READWRITE = 0x40;
const MEM_COMMIT = 0x1000;
const MEM_RELEASE = 0x00008000;
const MEM_DECOMMIT = 0x00004000
const rdtscAsm = new Uint8Array([0x0F, 0x31, 0x48, 0xC1, 0xE2, 0x20, 0x48, 0x09, 0xD0, 0xC3]);

//openComPort()

let ptr = VirtualAlloc(0, rdtscAsm.buffer.byteLength, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
CopyMemory(ptr, rdtscAsm.buffer, rdtscAsm.buffer.byteLength)
const rtdsc_test = koffi.decode(ptr, rtdsc);
const average = array => array.reduce((a, b) => a + b) / array.length;
const avg = []
for (let i = 0; i < 100; i++) {
    const result = Math.abs(rtdsc_test() - rtdsc_test())
    avg.push(result)
    //console.log(rtdsc_test())
    await wait(20)
}
console.log(average(avg))

const free = VirtualFree(koffi.address(ptr), rdtscAsm.buffer.byteLength, MEM_RELEASE)
if (free == 0)
    console.log("memory freed")
function wait(ms) {
    return new Promise((resolve) => { setTimeout(resolve, ms) })
}

function openComPort() {
    const CTLCODE = 0xdaf52480;
    const maxUInt64 = 2n ** 64n - 1n;
    const FileAccess = { Read: 1, ReadWrite: 3, Write: 2 }
    const FileShare = { None: 0, Read: 1, Write: 2, ReadWrite: 3, Delete: 4, Inheritable: 16 }
    const FileMode = { CreateNew: 1, Create: 2, Open: 3, OpenOrCreate: 4, Truncate: 5 }
    const FileOptions = { None: 0, Encrypted: 16384, DeleteOnClose: 67108864, Asynchronous: 1073741824, RandomAccess: 268435456, SequentialScan: 134217728, WriteThrough: -2147483648 }
    const hdl = CreateFileA("\\\\.\\COM4", FileAccess.Read,
        FileShare.ReadWrite, 0, FileMode.Open,
        FileOptions.None, 0);
    const handle = koffi.as(hdl, HWND);
    const handleAddress = koffi.address(hdl)
    if (handleAddress ==  BigInt.asUintN(64, maxUInt64)) {
        console.log("handle error")
        return;
    }
    try {
        let drawer = 1;
        const ok = DeviceIoControl(hdl, CTLCODE, drawer, 1, 0,
            0, 0, 0);
        if (!ok) {
            console.log("IOCTL error")
            return;
        }
    }
    finally {
        CloseHandle(hdl);
    }
}