include 'wasm.inc'

func (i32) main()
start func
    i32.const 42
    return
end func

func (i32) double(i32)
start func
    local.get 0
    local.get 0
    i32.add
    return
end func

export func main as "main"
export func double as "2x"