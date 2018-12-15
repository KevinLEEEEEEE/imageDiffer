// // @ts-check
// // import test from './imageXOR.js';

// // const image1 = new Uint8ClampedArray([
// //   120, 120, 120, 255, 120, 120, 120, 255,
// //   120, 120, 120, 255, 120, 120, 120, 255,
// //   120, 120, 120, 255, 120, 120, 120, 255,
// //   120, 120, 120, 255, 120, 120, 120, 255
// // ]);

// // const image2 = new Uint8ClampedArray([
// //   121, 120, 120, 255, 120, 120, 120, 255,
// //   120, 121, 120, 255, 120, 120, 120, 255,
// //   120, 120, 121, 255, 120, 120, 120, 255,
// //   120, 120, 120, 255, 121, 120, 120, 255
// // ]);

// // const width = 20;
// // const height = 10;

// // const xorRes = [
// //   1, 0, 0, 0, 0, 0, 0, 0,
// //   0, 1, 0, 0, 0, 0, 0, 0,
// //   0, 0, 1, 0, 0, 0, 0, 0,
// //   0, 0, 0, 0, 1, 0, 0, 0
// // ];


// // window['Module']().then((exported) => {
// // const heapType = 'HEAPU8';
// // const bytesPerElememmt = Uint8Array.BYTES_PER_ELEMENT;

// // console.log(exported);

// // const pointer = exported.ccall('pixelDataXOR', 'number', ['array', 'array', 'number', 'number'], [image1, image2, width, height]);

// // const res = [];

// // for (let i = 0; i < width * height * 4; i += 1) {
// //   res.push(exported[heapType][pointer / bytesPerElememmt + i]);
// // }

// // console.log('pixel data xor: ' + arrayDeepEqual(res, xorRes));
// // console.log(res);

// // const arr1 = new Uint8ClampedArray(res);

// // const pointer2 = exported.ccall('pixelDataToByte', 'number', ['array', 'number', 'number'], [arr1, width, height]);

// // const res2 = [];

// // for (let i = 0; i < width * height; i += 1) {
// //   res2.push(exported[heapType][pointer2 / bytesPerElememmt + i]);
// // }

// // console.log(pointer2, res2);


// const xorTemplate = [
//   0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
//   0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
//   0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
//   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
//   0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//   0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
//   1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
//   1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
// ];
// const xorArray = []; // 320 * 320
// const width = 320;
// const height = xorArray.length / width;

// for (let i = 0; i < 512; i += 1) {
//   xorArray.push(...xorTemplate);
// }

// // const xorArray = xorTemplate;
// // const width = 20;
// // const height = xorArray.length / width;

// const getValueAt = (arr, w, h, x, y) => {
//   if (x >= w || y >= h || x < 0 || y < 0) {
//     return 0;
//   }

//   return arr[x + w * y];
// }

// const uniqueArray = (arr) => {
//   return arr.filter((value, index) => {
//     return arr.indexOf(value) === index;
//   })
// }

// const func = () => {
//   const classifyArray = (new Array(width * height)).fill(0);
//   const combinedArray = [];
//   const border = {};
//   let amount = 0;

//   const borderDetect = (x, y) => {
//     const classOf4th = getValueAt(classifyArray, width, height, x - 1, y);
//     const classOf1st = getValueAt(classifyArray, width, height, x - 1, y - 1);
//     const classOf2nd = getValueAt(classifyArray, width, height, x, y - 1);
//     const classOf3rd = getValueAt(classifyArray, width, height, x + 1, y - 1);

//     // combined array
//     const combine = [classOf4th, classOf1st, classOf2nd, classOf3rd].filter(ele => ele > 0);
//     const shorted = uniqueArray(combine);

//     if (shorted.length > 1) {
//       combinedArray.push(shorted.sort());
//     }

//     return combine;
//   }

//   for (let y = 0; y < height; y += 1) {
//     let prev = 0; // save the prev point type, 0 or 1
//     let prevClass = 0;

//     for (let x = 0; x < width + 1; x += 1) {
//       const value = getValueAt(xorArray, width, height, x, y);

//       if (value === 0) {
//         if (prev === 0) { // [0, 0] 
//           // no difference here, skip

//           continue;
//         } else { // [1, 0]
//           // add prev point to the right border

//           const combine = borderDetect(x - 1, y);

//           const classOfCurrent = combine[0];

//           border[classOfCurrent].rightBorder.push([x - 1, y]);
//         }
//       } else {
//         if (prev === 0) { // [0, 1]
//           const combine = borderDetect(x, y);

//           const classOfCurrent = combine[0] || ++amount;

//           if (combine.length === 0) {
//             border[classOfCurrent] = {
//               mergedTo: 0,
//               leftBorder: [width + 1, height + 1],
//               rightBorder: [0, 0],
//               topBorder: [width + 1, height + 1],
//               bottomBorder: [0, 0],
//             };
//           }

//           border[classOfCurrent].leftBorder.push([x, y]);

//           classifyArray[x + y * width] = classOfCurrent;

//           prevClass = classOfCurrent;
//         } else { // [1, 1]
//           // class of current equals left

//           const classOfCurrent = prevClass;

//           classifyArray[x + y * width] = classOfCurrent;
//         }
//       }

//       prev = value; // update the prev value
//     }

//     prev = 0;
//   }

//   for (let x = 0; x < width; x += 1) {
//     let prev = 0;

//     for (let y = 0; y < height + 1; y += 1) {
//       const value = getValueAt(xorArray, width, height, x, y);

//       if (value === 0) {
//         if (prev === 0) { // [0, 0]
//           continue;
//         } else { // [1, 0]
//           // add prev point to bottom border

//           const classOfCurrent = getValueAt(classifyArray, width, height, x, y - 1);

//           border[classOfCurrent].bottomBorder.push([x, y - 1]);
//         }
//       } else {
//         if (prev === 0) { // [0, 1]
//           // add current point to top border

//           const classOfCurrent = getValueAt(classifyArray, width, height, x, y);

//           border[classOfCurrent].topBorder.push([x, y]);

//           borderDetect(x, y);
//         } else { // [1, 1]
//           continue;
//         }
//       }

//       prev = value;
//     }

//     prev = 0;
//   }

//   // console.log('combinedArray:');
//   // console.log(combinedArray);

//   // console.log('border:');
//   // console.log(border);
// }

// func();

// const time1 = new Date().getTime();

// for (let i = 0; i < 10000; i += 1) {
//   func();
// }

// console.log((new Date().getTime()) - time1);
// // });