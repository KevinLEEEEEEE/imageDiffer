// @ts-check
import test from './imageXOR.js';

const image1 = new Uint8ClampedArray([
  120, 120, 120, 255, 120, 120, 120, 255,
  120, 120, 120, 255, 120, 120, 120, 255,
  120, 120, 120, 255, 120, 120, 120, 255,
  120, 120, 120, 255, 120, 120, 120, 255
]);

const image2 = new Uint8ClampedArray([
  121, 120, 120, 255, 120, 120, 120, 255,
  120, 121, 120, 255, 120, 120, 120, 255,
  120, 120, 121, 255, 120, 120, 120, 255,
  120, 120, 120, 255, 121, 120, 120, 255
]);

const width = 2;
const height = 4;

const xorRes = [
  1, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0
];

const arrayDeepEqual = (arr1, arr2) => {
  if (arr1.length !== arr2.length) {
    return false;
  }

  for (let i = 0; i < arr1.length; i += 1) {
    if (arr1[i] !== arr2[i]) {
      return false;
    }
  }

  return true;
};

window['Module']().then((exported) => {
  const heapType = 'HEAPU8';
  const bytesPerElememmt = Uint8Array.BYTES_PER_ELEMENT;

  console.log(exported);

  const pointer = exported.ccall('pixelDataXOR', 'number', ['array', 'array', 'number', 'number'], [image1, image2, width, height]);

  const res = [];

  for (let i = 0; i < width * height * 4; i += 1) {
    res.push(exported[heapType][pointer / bytesPerElememmt + i]);
  }

  console.log('pixel data xor: ' + arrayDeepEqual(res, xorRes));

  const arr1 = new Uint8ClampedArray(res);

  const pointer2 = exported.ccall('pixelDataToByte', 'number', ['array', 'number', 'number'], [arr1, width, height]);

  const res2 = [];

  for (let i = 0; i < width * height; i += 1) {
    res2.push(exported[heapType][pointer2 / bytesPerElememmt + i]);
  }

  console.log(pointer2, res2);
});