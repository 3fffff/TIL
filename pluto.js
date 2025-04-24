const PlutoSDR_USB_VID = 0x0456;
const PlutoSDR_USB_PID = 0xb673;
const IIO_USD_CMD_RESET_PIPES = 0;
const IIO_USD_CMD_OPEN_PIPE = 1;
const IIO_USD_CMD_CLOSE_PIPE = 2;
const IIO_USB_INTERFACE = 5;
const OUT_EP = 0x04
const IN_EP = 0x06
const maxBandwidth = 52000000
const maxSampleRate = 61000000
const TIMEOUT = 2500
const pipe = 1

export class PlutoSDR {
  #serialNumber // serial number
  #xmlMeta      // xml metadata
  #rxRunning = null
  #bandwidth = 20000000
  #maxBytesRead = 2000000
  #centerFrequency = 205e6

  /* device property getters/setters */
  get serialNumber() { return this.#serialNumber; }
  get xmlMeta() { return this.#xmlMeta; }

  /* center frequency getters/setters */
  async getCenterFreq() {
    let rx = parseInt(await this.getAttribute("altvoltage0", "OUTPUT", "frequency"));
    let tx = parseInt(await this.getAttribute("altvoltage1", "OUTPUT", "frequency"));
    if (rx !== tx) throw `RX and TX center frequencies don't match. (${rx} !== ${tx})`;
    return rx;
  }
  async setCenterFreq(freq) {
    let fs = `${freq}`;
    await this.setAttribute("altvoltage0", "OUTPUT", "frequency", fs);
    await this.setAttribute("altvoltage1", "OUTPUT", "frequency", fs);
  }

  /* constructor */
  constructor() {
    this.pluto = null;
    this.te = new TextEncoder();
    this.td = new TextDecoder();
  }

  async #executeVendorCommand(command, value = 0) {
    let result = await this.pluto.controlTransferOut({
      requestType: "vendor",
      recipient: "interface",
      request: command,
      value: value,
      index: IIO_USB_INTERFACE,
      data: null
    });
    if (result.status !== "ok") { throw "Vendor command " + command + " failed!"; }
  }

  async sendCommands(commands, out_ep = OUT_EP, in_ep = IN_EP) {
    for (let i = 0; i < commands.length; i++) {
      let result = await this.pluto.transferOut(out_ep, this.te.encode(commands[i]));
      if (result.status !== "ok") throw "command '" + command + "'failed";
    }

    let res = await this.pluto.transferIn(in_ep, 10);
    if (res.status !== "ok") throw "sendCommands(): reading response code failed";
    var val = parseInt(this.td.decode(res.data.buffer));
    return val;
  }

  async setAttribute(channel, direction, register, value) {
    let ntval = `${value}\x00`;
    let writeCommand = `WRITE iio:device1 ${direction} ${channel} ${register} ${ntval.length}\r\n`;
    var val = await this.sendCommands([writeCommand, ntval]);
    if (val !== ntval.length) throw "setAttribute() failed, bad length";
  }

  async getAttribute(channel, direction, register) {
    /* send the READ command to the device */
    let readCommand = `READ iio:device1 ${direction} ${channel} ${register}\r\n`;
    var len = await this.sendCommands([readCommand]);
    if (len < 0) throw `getAttribute() failed with error code ${len}`;

    /* read in the response */
    let result = await this.pluto.transferIn(IN_EP, 1024);
    console.log(result);
    if (result.status !== "ok") throw "READ response read failed";
    let data = this.td.decode(result.data.buffer);

    /* return the data as an ArrayBuffer */
    return data;
  }

  async getXmlMeta() {
    /* send the PRINT command, which returns the XML length */
    let dataLen = await this.sendCommands(["PRINT\r\n"]);

    /* read in the XML */
    let result = await this.pluto.transferIn(IN_EP, dataLen);
    if (result.status !== "ok") { throw "PRINT response read failed"; }
    let data = this.td.decode(result.data.buffer);

    /* read in the terminating newline */
    result = await this.pluto.transferIn(IN_EP, 10);
    if (result.status !== "ok") { throw "PRINT failed reading terminating newline"; }

    /* parse the XML */
    сщтые parser = new DOMParser();
    this.#xmlMeta = parser.parseFromString(data, "text/xml");
  }

  async openDevice(pipe, dev, sampleCount, channelMask, timeout) {
    /* open the pipe */
    await this.#executeVendorCommand(IIO_USD_CMD_OPEN_PIPE, pipe);

    /* configure the channel */
    let command = `OPEN ${dev} ${sampleCount} ${channelMask}`;
    let ret = await this.sendCommands([`${command}\r\n`], OUT_EP + pipe, IN_EP + pipe);
    if (ret !== 0) throw `${command} failed with code ${ret}`;

    /* set the I/O timeout */
    command = `TIMEOUT ${timeout}`;
    ret = await this.sendCommands([`${command}\r\n`], OUT_EP + pipe, IN_EP + pipe);
    if (ret !== 0) throw `${command} failed with code ${ret}`;
  }

  async closeDevice(dev = "iio:device4", pipe = 1) {
    let command = `CLOSE ${dev}\r\n`;
    let ret = await this.sendCommands([`${command}\r\n`], OUT_EP + pipe, IN_EP + pipe);
    if (ret !== 0) throw `${command} failed with code ${ret}`;
    await this.#executeVendorCommand(IIO_USD_CMD_CLOSE_PIPE, pipe);
    await this.#executeVendorCommand(IIO_USD_CMD_CLOSE_PIPE, 0);
  }

  async init() {
    // enumerate connected device we have permission to access
    let devices = await navigator.usb.getDevices();
    devices.forEach(device => {
      // select the first PlutoSDR
      if (this.pluto === null) {
        if (device.vendorId == PlutoSDR_USB_VID &&
          device.productId == PlutoSDR_USB_PID) {
          this.pluto = device;
        }
      }
    });

    // if we couldn't find an authorized PlutoSDR, request access
    if (this.pluto === null) {

      // request device by USB VID/PID
      let device;
      try {
        device = await navigator.usb.requestDevice({
          filters: [{
            vendorId: PlutoSDR_USB_VID,
            productId: PlutoSDR_USB_PID
          }]
        });
      } catch (err) {
        console.log(err);
      }

      // select the device
      if (device !== undefined) {
        this.pluto = device;
      }
    }

    // report failure if we couldn't find a PlutoSDR
    if (this.pluto === null) {
      console.log("No PlutoSDR found!");
      return;
    }

    /* update the serial number */
    this.#serialNumber = this.pluto.serialNumber;

    /* setup the USB device */
    await this.pluto.open();
    await this.pluto.selectConfiguration(1);
    await this.pluto.claimInterface(5);
  }

  set Bandwidth(bandwidth) {
    if (bandwidth <= maxBandwidth)
      this.#bandwidth = bandwidth
  }

  async setAttackGain(slow) {
    if (!slow)
      await this.setAttribute("voltage0", "INPUT", "gain_control_mode", "fast_attack")
    else await this.setAttribute("voltage0", "INPUT", "gain_control_mode", "slow_attack")
  }

  async setManualGain(gain) {
    if (gain < 0 || gain > 70)
      return;
    await this.setAttribute("voltage0", "INPUT", "gain_control_mode", "manual")
    await this.setAttribute("voltage0", "INPUT", "hardwaregain", gain.toString());
  }

  async startRx(callback) {
    if (this.#rxRunning) {
      console.log("already started");
      return;
    }

    await this.#executeVendorCommand(IIO_USD_CMD_RESET_PIPES, 0);
    await this.#executeVendorCommand(IIO_USD_CMD_OPEN_PIPE, 0);
    await this.setAttribute("voltage0", "INPUT", "rf_port_select", "A_BALANCED");
    await this.setAttribute("voltage0", "OUTPUT", "rf_port_select", "A");
    await this.setAttribute("voltage0", "INPUT", "rf_bandwidth", this.#bandwidth.toString());
    await this.setAttribute("voltage0", "OUTPUT", "rf_bandwidth", this.#bandwidth.toString());
    await this.setAttribute("voltage0", "INPUT", "sampling_frequency", this.#bandwidth.toString());
    await this.setAttribute("voltage0", "OUTPUT", "sampling_frequency", this.#bandwidth.toString());
    await this.setCenterFreq(this.#centerFrequency);
    const prescaler = ~~(this.#bandwidth.toString() / this.#maxBytesRead)
    const bytes = ~~(this.#bandwidth.toString() / prescaler)
    let samples = bytes / 4;
    /* configure the RX channel */
    await this.openDevice(1, "iio:device4", samples, "00000003", TIMEOUT);

    const transfer = async (resolve) => {
      let ret = await this.sendCommands([`READBUF iio:device4 ${bytes}\r\n`], OUT_EP + pipe, IN_EP + pipe);
      if (ret !== bytes) throw `READBUF iio:device4 ${bytes} failed with code ${ret}`;
      let res = await this.pluto.transferIn(IN_EP + pipe, 10);
      if (res.status !== "ok") throw "IQ RX reading response code failed";
      let val = parseInt(this.td.decode(res.data.buffer));
      if (val != 3) throw `read got ${val} expected 3`

      /* read in the IQ */
      const result = await this.pluto.transferIn(IN_EP + pipe, bytes);
      if (result.status !== 'ok')
        throw 'failed to get transfer';
      callback(result.data.buffer);
      if (this.#rxRunning)
        transfer(resolve);
      else resolve();
    }
    this.#rxRunning = new Promise(resolve => transfer(resolve))
  }

  async stopRx() {
    if (this.#rxRunning) {
      const promise = this.#rxRunning;
      this.#rxRunning = null;
      await Promise.resolve(promise);
      await this.closeDevice()
    }
  }

  async close() {
    this.pluto.close()
  }
}
