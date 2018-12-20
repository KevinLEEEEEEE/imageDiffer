// @ts-check

const removeDuplicateAndZero = (arr) => {
  return arr.filter((value, index) => {
    return arr.indexOf(value) === index && value !== 0;
  });
}

const func = (xorArray, width, height) => {
  const classArray = (new Array((width + 2) * (height + 2))).fill(0);
  const connects = [];
  const borders = {};
  let amount = 0;

  const borderConnect = (...val) => {
    const arr = removeDuplicateAndZero(val)

    if (arr.length > 1) connects.push(arr);
  }

  const getClassValue = (x, y) => classArray[x + 1 + (width + 2) * (y + 1)];

  const setClassValue = (x, y, val) => {
    classArray[x + 1 + (width + 2) * (y + 1)] = val;
  }

  const getXorValue = (x, y) => {
    return (x >= width || y >= height || x < 0 || y < 0) ? 0 : xorArray[x + width * y];
  }

  const connectBoard = () => {
    connects.forEach((connect) => {
      let [target, merge] = connect;

      if (target < merge) {
        const tmp = target;

        target = merge;

        merge = tmp;
      }

      const tar = borders[target];
      const mer = borders[merge];

      if (mer.isMerged === true) {
        return;
      }

      tar.mergeWith.push(merge, ...mer.mergeWith);

      mer.isMerged = true;
    })
  }

  const compareBorder = (collection) => {
    return collection.reduce(([prevLeft, prevRight, prevTop, prevBottom], curr) => {
      const { left, right, top, bottom } = borders[curr];

      left.forEach((val) => {
        if (val < prevLeft) {
          prevLeft = val;
        }
      })

      right.forEach((val) => {
        if (val > prevRight) {
          prevRight = val;
        }
      })

      top.forEach((val) => {
        if (val < prevTop) {
          prevTop = val;
        }
      })

      bottom.forEach((val) => {
        if (val > prevBottom) {
          prevBottom = val;
        }
      })

      return [prevLeft, prevRight, prevTop, prevBottom];
    }, [width, 0, height, 0])
  }

  const compareAndSortBorder = () => {
    const sortedBorderArray = [];

    Object.keys(borders).forEach((key) => {
      const { isMerged, mergeWith } = borders[key];

      if (isMerged === true) {
        return;
      }

      const [x1, x2, y3, y4] = compareBorder([key, ...mergeWith]);

      sortedBorderArray.push([[x1, y3], [x2, y3], [x2, y4], [x1, y4]]);
    })

    return sortedBorderArray;
  }

  for (let y = 0; y < height + 1; y += 1) {
    let prev = 0; // class of prev

    for (let x = 0; x < width + 1; x += 1) {
      const val = getXorValue(x, y);
      const top = getClassValue(x, y - 1); // class of top

      if (val === 0) {
        if (prev !== 0) {
          // add prev point to the right border

          borders[prev].right.push(x - 1);

          borderConnect(
            getClassValue(x - 2, y),
            getClassValue(x - 2, y - 1),
            getClassValue(x - 1, y - 1),
            getClassValue(x, y - 1),
          )

          prev = 0;
        }

        if (top !== 0) {
          borders[top].bottom.push(y - 1);
        }
      } else {
        const classOf1st = getClassValue(x - 1, y - 1);
        const classOf3rd = getClassValue(x + 1, y - 1);
        const classOfSurround = prev || classOf1st || top || classOf3rd || 0;
        const classOfCurr = classOfSurround === 0 ? ++amount : classOfSurround;

        if (prev === 0) { // [0, 1]
          if (classOfSurround === 0) {
            borders[classOfCurr] = {
              isMerged: false,
              mergeWith: [],
              left: [],
              right: [],
              top: [],
              bottom: [],
            };
          }

          borders[classOfCurr].left.push(x);

          borderConnect(prev, classOf1st, top, classOf3rd);

          setClassValue(x, y, classOfCurr);

          prev = classOfCurr;
        } else { // [1, 1]
          // class of current equals left

          setClassValue(x, y, prev);
        }

        if (top === 0) {
          borders[classOfCurr].top.push(y);

          borderConnect(prev, classOf1st, top, classOf3rd);
        }
      }
    }

    prev = 0;
  }

  connectBoard();

  return compareAndSortBorder();
}

const imageXor = (imageData1, imageData2, w, h) => {
  const data1 = imageData1.data;
  const data2 = imageData2.data;
  const xor = (new Array(w * h)).fill(0);

  for (let i = 0; i < w * h * 4; i += 4) {
    const isChanged = (data1[i] ^ data2[i]) ||
      (data1[i + 1] ^ data2[i + 1]) ||
      (data1[i + 2] ^ data2[i + 2]) ||
      (data1[i + 3] ^ data2[i + 3]);

    if (isChanged !== 0) {
      xor[i / 4] = 1;
    }
  }

  return xor;
}

const imageDiff = (ctx1, ctx2, w, h) => {
  const imageData1 = ctx1.getImageData(0, 0, w, h);
  const imageData2 = ctx2.getImageData(0, 0, w, h);

  const xorArray = imageXor(imageData1, imageData2, w, h);

  const time1 = new Date().getTime();

  for (let i = 0; i < 1000; i += 1) {
    func(xorArray, w, h);
  }

  console.log((new Date().getTime()) - time1);

  const borders = func(xorArray, w, h);

  return borders;
}