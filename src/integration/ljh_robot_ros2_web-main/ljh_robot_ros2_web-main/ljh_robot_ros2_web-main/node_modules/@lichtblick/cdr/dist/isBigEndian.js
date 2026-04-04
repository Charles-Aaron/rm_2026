"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.isBigEndian = void 0;
const endianTestArray = new Uint8Array(4);
const endianTestView = new Uint32Array(endianTestArray.buffer);
endianTestView[0] = 1;
/**
 * Test if the current running system is Big Endian architecture or Little Endian.
 * @returns true on Big Endian architecture systems
 */
function isBigEndian() {
    return endianTestArray[3] === 1;
}
exports.isBigEndian = isBigEndian;
//# sourceMappingURL=isBigEndian.js.map