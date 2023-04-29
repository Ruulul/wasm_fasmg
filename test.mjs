import { readFile } from 'fs/promises'
import { createInterface } from 'readline'
const read = createInterface(process.stdin, process.stdout)
const buffer = await readFile('test.wasm')
const valid_buffer = WebAssembly.validate(buffer)
console.log(`the compiled assembly is ${valid_buffer ? '' : 'not '}valid!`)
if (!valid_buffer) process.exit(0)
const { instance } = await WebAssembly.instantiate(buffer)
for (const [name, func] of Object.entries(instance.exports)) {
    let params
    if (func.length) 
        params = 
        await Promise.all(
            Array(func.length)
            .fill()
            .map((_, i)=>
                question(`insert the param n ${i} for ${name}: `).then(JSON.parse)
            )
        )
        
    console.log(`${name} returns ${func.apply(undefined, params && params)}`)
}
read.close()


async function question(prompt) {
    let res
    let promise = new Promise(_res => res = _res)
    read.question(prompt, res)
    return promise
}