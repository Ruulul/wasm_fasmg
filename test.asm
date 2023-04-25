include 'wasm.inc'

func $main param result i32 local
    i32.const 42
    return
end func
export func $main as "main"