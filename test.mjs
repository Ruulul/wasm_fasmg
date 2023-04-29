import { readFile } from 'fs/promises'
import { createInterface } from 'readline'
const read = createInterface(process.stdin, process.stdout)
const buffer = await readFile('test.wasm')
const { instance } = await WebAssembly.instantiate(buffer)
console.log(`the compiled assembly is ${WebAssembly.validate(buffer) ? '' : 'not '}valid!`)
for (const [name, func] of Object.entries(instance.exports)) {
    let params
    if (func.length) 
        params = 
        await Promise.all(
            Array(func.length)
            .fill()
            .map((_, i)=>
                question(`insert the param n ${i} for ${name}: `)
            )
        )
        
    console.log(`${name} runs as ${func.apply(undefined, params && params)}`)
}
read.close()


async function question(prompt) {
    let res
    let promise = new Promise(_res => res = _res)
    read.question(prompt, res)
    return promise
}