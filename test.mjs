import { readFile } from 'fs/promises'
const buffer = await readFile('test.wasm')
const { instance } = await WebAssembly.instantiate(buffer)
console.log(`the compiled assembly is ${WebAssembly.validate(buffer) ? '' : 'not '}valid!`)
console.log(instance.exports.main?.call() || "no exported function!")