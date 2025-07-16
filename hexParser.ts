const START_CODE_OFFSET = 0;
const START_CODE_LENGTH = 1;
const BYTE_COUNT_OFFSET = START_CODE_OFFSET + START_CODE_LENGTH;
const BYTE_COUNT_LENGTH = 2;
const ADDRESS_OFFSET = BYTE_COUNT_OFFSET + BYTE_COUNT_LENGTH;
const ADDRESS_LENGTH = 4;
const RECORD_TYPE_OFFSET = ADDRESS_OFFSET + ADDRESS_LENGTH;
const RECORD_TYPE_LENGTH = 2;
const DATA_OFFSET = RECORD_TYPE_OFFSET + RECORD_TYPE_LENGTH;
const CHECKSUM_LENGTH = 2;
const minimumLineLength = START_CODE_LENGTH + BYTE_COUNT_LENGTH + ADDRESS_LENGTH + RECORD_TYPE_LENGTH + CHECKSUM_LENGTH;

export function hexToBin(data: string) {
  let result: number[] = []

  data.trim().split(/\r?\n/).forEach(line => {
    if (line.length < minimumLineLength)
      throw new Error("line length")
    if (line[0] !== ":")
      throw new Error("line not start with :")

    const DataLength = Number("0x" + line.substring(BYTE_COUNT_OFFSET, BYTE_COUNT_OFFSET + BYTE_COUNT_LENGTH));
    const Address = Number("0x" + line.substring(ADDRESS_OFFSET, ADDRESS_OFFSET + ADDRESS_LENGTH));
    const Type = Number("0x" + line.substring(RECORD_TYPE_OFFSET, RECORD_TYPE_OFFSET + RECORD_TYPE_LENGTH));
    const checkSumLine = Number("0x" + line.substring(DATA_OFFSET + DataLength * 2, DATA_OFFSET + DataLength * 2 + CHECKSUM_LENGTH));

    if (Type == 4 || Type == 5) return;
    if (Type != 0 && Type != 1)
      throw new Error("UNSUPPORTED_RECORD_TYPE");
    const lineBytes = new Uint8Array(DataLength)
    for (let i = 0; i < DataLength; i++)
      lineBytes[i] = Number(("0x" + line.substring(DATA_OFFSET + 2 * i, DATA_OFFSET + 2 * i + 2)).toString());

    let checksum = DataLength + Type + Address + ((Address & 0xFF00) >> 8);

    for (let i = 0; i < DataLength; i++)
      checksum = (checksum & 0xFF) + lineBytes[i];

    checksum = ((~checksum + 1) & 0xFF);
    if (checkSumLine != checksum)
      throw new Error("checksum incorrect")

    result.push(...lineBytes)
  })
  return new Uint8Array(result)
}
