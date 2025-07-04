import { JSB } from 'internal:constants';

declare const jsb: any;

export function createExternalArrayBuffer (byteLength: number): ArrayBuffer {
    if (JSB) {
        return jsb.createExternalArrayBuffer(byteLength) as ArrayBuffer;
    } else {
        return new ArrayBuffer(byteLength);
    }
}

export function createExternalFloat32Array (length: number): Float32Array {
    if (JSB) {
        return new Float32Array(jsb.createExternalArrayBuffer(length * Float32Array.BYTES_PER_ELEMENT) as ArrayBuffer);
    } else {
        return new Float32Array(length);
    }
}

export function createExternalUint16Array (length: number): Uint16Array {
    if (JSB) {
        return new Uint16Array(jsb.createExternalArrayBuffer(length * Uint16Array.BYTES_PER_ELEMENT) as ArrayBuffer);
    } else {
        return new Uint16Array(length);
    }
}
