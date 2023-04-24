format binary as 'wasm'

namespace section
    custom.id  = 0
    type.id    = 1
    import.id  = 2
    func.id    = 3
    table.id   = 4
    mem.id     = 5
    global.id  = 6
    export.id  = 7
    start.id   = 8
    elem.id    = 9
    code.id    = 10
    data.id    = 11
end namespace
namespace type
    i32         = 0x7F
    i64         = 0x7E
    f32         = 0x7D
    f64         = 0x7C
    v128        = 0x7B
    funcref     = 0x70
    externref   = 0x6F
    func        = 0x60
    has_max     = 0x01
    no__max     = 0x00
    const       = 0x00
    var         = 0x01
end namespace
namespace inst
    i32:
        .const = 0x41
        .eqz   = 0x45
        .eq    = 0x46
        .ne    = 0x47
        .lt_s  = 0x48
        .lt_u  = 0x49
        .gt_s  = 0x4A
        .gt_u  = 0x4B
        .le_s  = 0x4C
        .le_u  = 0x4D
        .ge_s  = 0x4E
        .ge_u  = 0x4F
    i64:
        .const = 0x42
        .eqz   = 0x50
        .eq    = 0x51
        .ne    = 0x52
        .lt_s  = 0x53
        .lt_u  = 0x54
        .gt_s  = 0x55
        .gt_u  = 0x56
        .le_s  = 0x57
        .le_u  = 0x58
        .ge_s  = 0x59
        .ge_u  = 0x5A
    f32:
        .const = 0x43
        .eq    = 0x5B
        .ne    = 0x5C
        .lt    = 0x5D
        .gt    = 0x5E
        .le    = 0x5F
        .ge    = 0x60
    f64:
        .const = 0x44
        .eq    = 0x61
        .ne    = 0x62
        .lt    = 0x63
        .gt    = 0x64
        .le    = 0x65
        .ge    = 0x66
    ?local:
        .get = 0x20
        .set = 0x21
        .tee = 0x22
    global:
        .get = 0x23
        .set = 0x24
    ref:
        .null    = 0xD0
        .is_null = 0xD1
        .func    = 0xD2
    table:
        .get  = 0x25
        .set  = 0x26
        .prefix = 0xFC
        .init = 12
        .drop = 13
        .copy = 14
        .grow = 15
        .size = 16
        .fill = 17
    empty_block     = 0x40
    unreachable     = 0x00
    nop             = 0x01
    block           = 0x02
    loop            = 0x03
    ?if              = 0x04
    ?else            = 0x05
    
    ?end             = 0x0B
    br              = 0x0C
    br_if           = 0x0D
    br_table        = 0x0E
    return          = 0x0F
    call            = 0x10
    call_indirect   = 0x11

    drop            = 0x1A
    select          = 0x1B
    select_t        = 0x1C
end namespace

db 0, 'asm'
db 1, 3 dup 0

define sections_names custom, type, import, func, table, mem, global, export, start, elem, code, data
iteratelist name, sections_names
    if name#_section <> 0
        db name#_section
    end if
end iterate
iteratelist name, sections_names
    virtual at 0
        name::
    end virtual
end iterate
    
postpone
    iteratelist name, sections_names
        if sizeof name > 0
            ; TODO: need to write section id and size of section here
            load name#_section:sizeof name from name:0
        else
            name#_section = 0
        end if
    end iterate
end postpone

calminstruction iteratelist?! param, var
    transform var
    arrange param, =iterate param, var
    assemble param
end calminstruction

calminstruction encode_u number*
    local byte
    loop:
        compute byte, number and 0x7F
        compute number, number shr 7
        check number > 0
        jno emit_byte
        compute byte, byte or 1 shl 7
    emit_byte:
        arrange byte, =db byte
        assemble byte
        jyes loop
end calminstruction

calminstruction encode_s number*
    compute number, (not number) + 1
    call encode_u, number
end calminstruction

macro calminstruction?.debug? value*, string
    stringify value
    display string
    display value
    display 10
end macro

calminstruction calminstruction?.initsym? variable*,value&
    publish variable, value
end calminstruction
calminstruction calminstruction?.asm? line&
        local   name, i
        initsym name, name.0
        match   name.i, name
        compute i, i+1
        arrange name, name.i
        publish name:, line
        arrange line, =assemble name
        assemble line
end calminstruction   

macro calminstruction?.start_virtual? section
    asm virtual section
end macro
macro calminstruction?.end_virtual?
    asm end virtual
end macro
calminstruction ?! line&
    local instr, namespace, opcode, section, signature
    check __file__ = __source__
    jno assemble_line

    match =func signature, line
    jyes parse_func
    
    match =export signature, line
    jyes parse_export

    match namespace.instr signature, line
    jyes parse_namespaced_instr
    match instr signature, line
    jyes parse_instr
    match instr, line
    jyes parse_simple_instr
    
    debug line, 'non parsed line: '

    exit
    
    parse_func:
        debug signature, 'func sign: '
        exit
    parse_namespaced_instr:
        start_virtual code
        arrange opcode, =db =inst.namespace.instr
        assemble opcode
        call encode_u, signature
        end_virtual
        exit
    parse_instr:
        start_virtual code
        arrange opcode, =db =inst.instr
        assemble opcode
        call encode_u, signature
        end_virtual
        exit
    parse_simple_instr:
        start_virtual code
        arrange opcode, =db =inst.instr
        assemble opcode
        end_virtual
        exit
    parse_export:
        match kind identifier =as name, signature
        transform kind
        transform identifier
        start_virtual export
        arrange name, =db name
        arrange kind, =db 0
        arrange identifier, =db 0
        assemble name
        assemble kind
        assemble identifier
        end_virtual
        exit
    assemble_line:
        assemble line
end calminstruction