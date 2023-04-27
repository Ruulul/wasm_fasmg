format binary as 'wasm'
include 'macros.inc'

define sections_names custom, type, import, func, table, mem, global, export, start, elem, code, data
iteratelist name, sections_names
    section.name.id = % - 1
    section.name.count = 0 
    section.name.index = 0
end iterate
type?:
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
inst?:
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
        .add   = 0x6A
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
namespace export
    func    = 0x00
    table   = 0x01
    mem     = 0x02
    global  = 0x03
end namespace

define section? section

db 0, 'asm'
db 1, 3 dup 0
iteratelist name, sections_names
    virtual at 0
        name::
    end virtual
end iterate
postpone
    iteratelist name, sections_names
        if sizeof name > 0
            db section.name.id
            start_vector
                encode_u section.name.count
                load name#_section:sizeof name from name:0
                db name#_section
            end_vector
        end if
    end iterate
end postpone

calminstruction ?! line&
    local instr, namespace, signature
    check __file__ = __source__
    jno assemble_line

    match =func signature, line
    jyes parse_func
    
    match =export signature, line
    jyes parse_export

    match namespace.instr signature?, line
    jyes parse_namespaced_instr
    match instr signature?, line
    jyes parse_instr
    
    exit
    
    parse_func:
        local identifier, id, params, result, locals
        arrange identifier,
        arrange params,
        arrange result,
        arrange locals,
        match identifier? =param params? =result result? =local locals?, signature
        jyes parse_func_1 ;000

        exit
        
        parse_func_1:
            compute section.type.count, section.type.count + 1
            compute section.func.count, section.func.count + 1
            compute section.code.count, section.code.count + 1
            start_virtual type
                asm db type.func
                asm start_vector
                    match ,params
                    jyes parse_func_skip_params
                    arrange params, =db =type.params
                    assemble params
                    parse_func_skip_params:
                asm end_vector
                asm start_vector
                    match ,result
                    jyes parse_func_skip_result
                    arrange result, =db =type.result
                    assemble result
                    parse_func_skip_result:
                asm end_vector
            end_virtual
            start_virtual func
                compute tmp, section.func.index
                arrange tmp, =db tmp
                assemble tmp
                match ,identifier
                jno link_identifier
                parse_func_1_continue:
            end_virtual
            start_virtual code
                asm start_vector
                    asm start_vector
                        match ,locals
                        jyes parse_func_skip_locals
                        arrange locals, =db =type.locals
                        assemble locals
                        parse_func_skip_locals:
                    asm end_vector
            end_virtual
            compute section.func.index, section.func.index + 1
            exit
            link_identifier:
                publish identifier, section.func.index
                jump parse_func_1_continue
    parse_namespaced_instr:
        local opcode
        start_virtual code
            arrange opcode, =db =inst.namespace.instr
            assemble opcode
            match ,signature
            jyes parse_namespaced_instr__skip_signature
            call encode_u, signature
            parse_namespaced_instr__skip_signature:
        end_virtual
        exit
    parse_instr:
        local opcode
        match =end =func, line
        jyes close_func_body
        start_virtual code
            arrange opcode, =db =inst.instr
            assemble opcode
            match ,signature
            jyes parse_inst__skip_signature
            call encode_u, signature
            parse_inst__skip_signature:
        end_virtual
        exit
        close_func_body:
            start_virtual code
                    asm db inst.end
                asm end_vector
            end_virtual
            exit
    parse_export:
        local kind, identifier, name
        match kind identifier =as name, signature
        compute section.export.count, section.export.count + 1
        start_virtual export
            arrange name, =db name
            arrange kind, =db =export.kind
            arrange identifier, =db identifier
            asm start_vector
                assemble name
            asm end_vector
            assemble kind
            assemble identifier
        end_virtual
        exit
    assemble_line:
        assemble line
end calminstruction