import { Worker, isMainThread, parentPort, workerData } from 'worker_threads'
//import request from 'request'
import { dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { parseArgs } from 'node:util';
const __filename = dirname(fileURLToPath(import.meta.url));

if (isMainThread) {
    console.log("main thread")
    let w = new Worker(__filename, { execArgv: ["--inspect-brk"],workerData: null });
    setTimeout(() =>
        w.postMessage({ cancel: true }), 5000)

    w.postMessage({ data: 500000 });
    w.on("message", (msg) => {
        console.log(msg)
    })
    w.on("error", console.error)
    w.on("exit", (code) => {
        console.log("worker stopped" + code)
    })
} else {
    let cancelWorker = false;

    const yields = () => new Promise((resolve) => setTimeout(() => resolve, 0))
    async function isPrime(n) {
        for (let i = 2; i < n / 2; i++) {
             await yields()
            if (cancelWorker) {
                cancelWorker = false;
                throw new Error();
            }
            if (n % i == 0)
                return false
        }
        return true
    }
    parentPort.on('message', async(message) => {
        if (message.cancel == true)
            cancelWorker = true
        else if (message.data != null) {
            const res = []
            for (let i = 4; i < message.data; i++) {
                const result = await isPrime(i)
                if (result)
                    res.push(i)
            }
            parentPort.postMessage({ data: res })
        }
    });
    // parentPort.postMessage({ val: sorter.firstValue, timeDiff: Date.now() - start });
}