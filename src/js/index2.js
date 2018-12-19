// // @ts-check

// console.log('2');

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
// const height =320;

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

// const imagediff = () => {
//   let prevClass = [];
//   let currClass = [];
//   let amount = 0;
//   const connects = [];
//   const borders = {};

//   const borderConnect = (k1 = 0, k2 = 0, k3 = 0, k4 = 0) => {
//     const arr = removeDuplicateAndZero([k1, k2, k3, k4])

//     if (arr.length > 1) connects.push(arr);
//   }

//   const getXorValue = (x, y) => {
//     return (x >= width || y >= height) ? 0 : xorArray[x + width * y];
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
//     for (let x = 0; x < width + 1; x += 1) {
//       const xorVal = getXorValue(x, y);
//       const classOfLeft = currClass[x - 1] || 0;
//       const classOfTop = prevClass[x] || 0;

//       if (xorVal === 0) {
//         if (classOfLeft !== 0) {
//           // left point is 【right】 border

//           borders[classOfLeft].right.push(x - 1);

//           borderConnect(currClass[x - 2], prevClass[x - 2], prevClass[x - 1], prevClass[x]);
//         }

//         if (classOfTop !== 0) {
//           // top point is 【bottom】 border

//           borders[classOfTop].bottom.push(y - 1);
//         }

//         currClass.push(0);
//       } else {
//         const classOf1st = prevClass[x - 1];
//         const classOf2nd = prevClass[x];
//         const classOf3rd = prevClass[x + 1];
//         const classOfSurround = classOfLeft || classOf1st || classOf2nd || classOf3rd || 0;
//         const classOfCurr = classOfSurround || ++amount;

//         if (classOfLeft === 0) {
//           // current point is 【left】 border

//           if (classOfSurround === 0) {
//             borders[classOfCurr] = {
//               isMerged: false,
//               mergeWith: [],
//               left: [],
//               right: [],
//               top: [],
//               bottom: [],
//             }
//           }

//           borders[classOfCurr].left.push(x);

//           borderConnect(classOfLeft, classOf1st, classOf2nd, classOf3rd);
//         }

//         if (classOfTop === 0) {
//           // current point is 【top】 border

//           borders[classOfCurr].top.push(y);

//           borderConnect(classOfLeft, classOf1st, classOf2nd, classOf3rd);
//         }

//         currClass.push(classOfCurr);
//       }
//     }

//     prevClass = currClass;
//     currClass = [];
//   }

//   connectBoard();

//   const sorted = compareAndSortBorder();

//   // console.log('connects:');
//   // console.log(connects);

//   // console.log('borders:');
//   // console.log(borders);

//   // console.log('sorted border:')
//   // console.log(sorted)
// }

// // imagediff();

// const time1 = new Date().getTime();

// for (let i = 0; i < 100; i += 1) {
//   imagediff();
// }

// console.log((new Date().getTime()) - time1);