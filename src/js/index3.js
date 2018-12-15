// // @ts-check

// console.log('3');

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

// const removeDuplicateAndZero = (arr) => {
//   return arr.filter((value, index) => {
//     return arr.indexOf(value) === index && value !== 0;
//   })
// }


// const func = () => {
//   const classArray = (new Array(width * height)).fill(0);
//   const connects = [];
//   const borders = {};
//   let amount = 0;

//   const borderConnect = (...val) => {
//     const arr = removeDuplicateAndZero(val)

//     if (arr.length > 1) connects.push(arr);
//   }

//   const getClassValue = (x, y) => {
//     return (x >= width || y >= height || x < 0 || y < 0) ? 0 : classArray[x + width * y];
//   }

//   const getXorValue = (x, y) => {
//     return (x >= width || y >= height || x < 0 || y < 0) ? 0 : xorArray[x + width * y];
//   }

//   const connectBoard = () => {
//     connects.forEach((connect) => {
//       const [target, merge] = connect;
//       const tar = borders[target];
//       const mer = borders[merge];

//       if (mer.isMerged === true) {
//         return;
//       }

//       tar.mergeWith.push(merge, ...mer.mergeWith);

//       mer.isMerged = true;
//     })
//   }

//   const compareBorder = (collection) => {
//     return collection.reduce(([prevLeft, prevRight, prevTop, prevBottom], curr) => {
//       const { left, right, top, bottom } = borders[curr];

//       left.forEach((val) => {
//         if (val < prevLeft) {
//           prevLeft = val;
//         }
//       })

//       right.forEach((val) => {
//         if (val > prevRight) {
//           prevRight = val;
//         }
//       })

//       top.forEach((val) => {
//         if (val < prevTop) {
//           prevTop = val;
//         }
//       })

//       bottom.forEach((val) => {
//         if (val > prevBottom) {
//           prevBottom = val;
//         }
//       })

//       return [prevLeft, prevRight, prevTop, prevBottom];
//     }, [width, 0, height, 0])
//   }

//   const compareAndSortBorder = () => {
//     const sortedBorderArray = [];

//     Object.keys(borders).forEach((key) => {
//       const { isMerged, mergeWith } = borders[key];

//       if (isMerged === true) {
//         return;
//       }

//       const [x1, x2, y3, y4] = compareBorder([key, ...mergeWith]);

//       sortedBorderArray.push([[x1, y3], [x2, y3], [x2, y4], [x1, y4]]);
//     })

//     return sortedBorderArray;
//   }

//   for (let y = 0; y < height + 1; y += 1) {
//     let prev = 0; // class of prev

//     for (let x = 0; x < width + 1; x += 1) {
//       const val = getXorValue(x, y);

//       if (val === 0) {
//         if (prev !== 0) {
//           // add prev point to the right border

//           borders[prev].right.push(x - 1);

//           borderConnect(
//             getClassValue(x - 2, y),
//             getClassValue(x - 2, y - 1),
//             getClassValue(x - 1, y - 1),
//             getClassValue(x, y - 1),
//           )

//           prev = 0;
//         }
//       } else {
//         if (prev === 0) { // [0, 1]
//           const classOf1st = getClassValue(x - 1, y - 1);
//           const classOf2nd = getClassValue(x, y - 1);
//           const classOf3rd = getClassValue(x + 1, y - 1);
//           const classOfSurround = classOf1st || classOf2nd || classOf3rd || 0;
//           const classOfCurr = classOfSurround || ++amount;

//           if (classOfSurround === 0) {
//             borders[classOfCurr] = {
//               isMerged: false,
//               mergeWith: [],
//               left: [],
//               right: [],
//               top: [],
//               bottom: [],
//             };
//           }

//           borders[classOfCurr].left.push(x);

//           classArray[x + y * width] = classOfCurr;

//           borderConnect(
//             prev,
//             getClassValue(x - 1, y - 1),
//             getClassValue(x, y - 1),
//             getClassValue(x + 1, y - 1),
//           )

//           prev = classOfCurr;
//         } else { // [1, 1]
//           // class of current equals left

//           classArray[x + y * width] = prev;
//         }
//       }
//     }

//     prev = 0;
//   }

//   for (let x = 0; x < width + 1; x += 1) {
//     let prev = 0;

//     for (let y = 0; y < height + 1; y += 1) {
//       const val = getXorValue(x, y);

//       if (val === 0) {
//         if (prev === 0) { // [0, 0]
//           continue;
//         } else { // [1, 0]
//           // add prev point to bottom border

//           const classOfCurr = getClassValue(x, y - 1);

//           borders[classOfCurr].bottom.push(y - 1);

//           prev = 0;
//         }
//       } else {
//         if (prev === 0) { // [0, 1]
//           // add current point to top border

//           const classOfCurr = getClassValue(x, y);

//           borders[classOfCurr].top.push(y);

//           borderConnect(
//             getClassValue(x - 1, y),
//             getClassValue(x - 1, y - 1),
//             getClassValue(x, y - 1),
//             getClassValue(x + 1, y - 1),
//           )

//           prev = classOfCurr;
//         } else { // [1, 1]
//           continue;
//         }
//       }
//     }

//     prev = 0;
//   }

//   connectBoard();

//   const sorted = compareAndSortBorder();

//   // console.log('combinedArray:');
//   // console.log(sorted);

//   // console.log('borders:');
//   // console.log(borders);
// }

// // func();

// const time1 = new Date().getTime();

// for (let i = 0; i < 100000; i += 1) {
//   func();
// }

// console.log((new Date().getTime()) - time1);

// // });