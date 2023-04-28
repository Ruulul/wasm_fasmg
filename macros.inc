
vector_count? = 0
vector_id? =: 0
vector_start? =: 0
calminstruction start_vector    
    local vector, tmp
    compute tmp, vector_count
    take vector_id, tmp
    compute vector_count, vector_count + 1
    arrange vector, =vector_#vector_id
    call encode_u, vector
    compute tmp, $
    take vector_start, tmp
end calminstruction
calminstruction end_vector
    local vector, length
    arrange vector, =vector_#vector_id
    compute length, $ - vector_start
    publish vector:, length
    take ,vector_start
    take ,vector_id
end calminstruction

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
macro calminstruction?.debug? var*, format
    local tmp
    arrange tmp, var
    stringify tmp
    display format
    display "'"
    display tmp
    display "'"
    display 10
end macro
macro calminstruction?.checksym? name*
    publish name:, name
    publish name:, name
end macro
macro calminstruction?.start_virtual? section
    asm virtual section
end macro
calminstruction calminstruction?.end_virtual?
    asm asm end virtual
end calminstruction