import { readFile } from 'fs/promises'
const buffer = await readFile('test.wasm')
const { instance } = await WebAssembly.instantiate(buffer)
console.log(`the compiled assembly is ${WebAssembly.validate(buffer) ? '' : 'not '}valid!`)
for (const [name, func] of Object.entries(instance.exports))
    console.log(`${name} runs as ${func(2)}`)