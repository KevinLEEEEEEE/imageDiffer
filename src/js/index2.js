// @ts-check

// const xorRes = [
//   1, 0, 0, 0, 0, 0, 0, 0,
//   0, 1, 0, 0, 0, 0, 0, 0,
//   0, 0, 1, 0, 0, 0, 0, 0,
//   0, 0, 0, 0, 1, 0, 0, 0
// ];

const xorTemplate = [
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
];
// const xorArray = []; // 320 * 320
// const width = 320;
// const height = xorArray.length / width;

// for(let i = 0; i < 512; i += 1) {
//   xorArray.push(...xorTemplate);
// }


const xorArray = xorTemplate;
const width = 20;
const height = xorArray.length / width;

const removeDuplicateAndZero = (arr) => {
  return arr.filter((value, index) => {
    return arr.indexOf(value) === index && value !== 0;
  })
}

const imagediff = () => {
  let prevClass = [];
  let currClass = [];
  let amount = 0;
  let connect = [];
  const border = {};

  const borderConnect = (k1 = 0, k2 = 0, k3 = 0, k4 = 0) => {
    const arr = removeDuplicateAndZero([k1, k2, k3, k4])

    if(arr.length > 1) connect.push(arr);
  }

  const getXorValue = (x, y) => {
    return (x >= width || y >= height) ? 0 : xorArray[x + width * y];
  }

  for (let y = 0; y < height + 1; y += 1) {
    for (let x = 0; x < width + 1; x += 1) {
      const xorVal = getXorValue(x, y);
      const classOfLeft = currClass[x - 1] || 0;
      const classOfTop = prevClass[x] || 0;

      if (xorVal === 0) {
        if (classOfLeft !== 0) {
          // left point is 【right】 border

          border[classOfLeft].right.push([x - 1, y]);

          borderConnect(currClass[x - 2], prevClass[x - 2], prevClass[x - 1], prevClass[x]);
        }

        if (classOfTop !== 0) {
          // top point is 【bottom】 border

          border[classOfTop].bottom.push([x, y - 1]);
        }

        currClass.push(0);
      } else {
        const classOf1st = prevClass[x - 1];
        const classOf2nd = prevClass[x];
        const classOf3rd = prevClass[x + 1];
        const classOfSurround = classOfLeft || classOf1st || classOf2nd || classOf3rd || 0;
        const classOfCurr = classOfSurround || ++amount;

        if (classOfLeft === 0) {
          // current point is 【left】 border

          if (classOfSurround === 0) {
            border[classOfCurr] = {
              merged: 0,
              left: [],
              right: [],
              top: [],
              bottom: [],
            }
          }

          border[classOfCurr].left.push([x, y]);

          borderConnect(classOfLeft, classOf1st, classOf2nd, classOf3rd);
        }

        if (classOfTop === 0) {
          // current point is 【top】 border

          border[classOfCurr].top.push([x, y]);

          borderConnect(classOfLeft, classOf1st, classOf2nd, classOf3rd);
        }

        currClass.push(classOfCurr);
      }
    }

    prevClass = currClass;
    currClass = [];
  }

  console.log('connect:');
  console.log(connect);

  console.log('border:');
  console.log(border);
}

imagediff();

// const time1 = new Date().getTime();

// for (let i = 0; i < 1000; i += 1) {
//   imagediff();
// }

// console.log((new Date().getTime()) - time1);