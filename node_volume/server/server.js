import express from 'express'
import koffi from "koffi"
import { dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { parseArgs } from 'node:util';
const __filename = dirname(fileURLToPath(import.meta.url));
const server = express();
const volume = koffi.load("volume.dll")
const setVolume = volume.func('int setVolume(double newVolume)');
//const EventLogger = require('node-windows').EventLogger;

const SERVER_PORT = 12345;
//const logger = new EventLogger('New windows-event logger');

const messageCodes = {
    0: "Ok",
    1: "Volume must be a fractional number ranging from 0 to 1",
    2: "No endpoints found.",
    3: "Volume is not specified",
    5: "NodeJS AudioManager server start at port " + SERVER_PORT,
    42: "Something went wrong when tried using extern DLL (unknown error)",
};

// Add headers before the routes are defined
server.use(function (req, res, next) {
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST');
    res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With, content-type');
    next();
});

server.use(express.json({ limit: '1mb' }));
server.use(express.static(__filename + '/../public'));

server.post('/api/setvolume', function (request, response) {
    const input = request.body;
    if (!input.hasOwnProperty('volume')) {
        response.status(422);
        response.send(messageCodes[3]);
        return;
    }
    const newVolume = parseFloat(input['volume']);
    if (isNaN(newVolume) || newVolume < 0 || newVolume > 1) {
        response.status(422);
        response.send(messageCodes[1]);
        return;
    }

    try {
        const res = setVolume(newVolume)
        if (res != 0) {
            console.error(messageCodes[res]);
            response.send(messageCodes[externalResult]);
        }

    } catch (err) {
        //  logger.error('Something went wrong when tried using extern DLL: ' + err.name + ' ' + err.message, 42);
        console.error(err);
        response.send(err.message);
        return;
    }
});

server.listen(SERVER_PORT, () => {
    //logger.info(messageCodes[5], 5); // Second param is event code, use your own
    console.log(messageCodes[5]);
    console.log('Open http://127.0.0.1:' + SERVER_PORT + ' in your browser');
});