include 'wasm.inc'

func main result i32
    i32.const 42
    return
end func
export func main as "main"