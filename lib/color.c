#ifndef EM_PORT_API
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#if defined(__cplusplus)
#define EM_PORT_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#else
#define EM_PORT_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#endif
#else
#if defined(__cplusplus)
#define EM_PORT_API(rettype) extern "C" rettype
#else
#define EM_PORT_API(rettype) rettype
#endif
#endif
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE_OF_RGBA 4;

enum borderType
{
  left,
  right,
  top,
  bottom
};

typedef struct mergeItem
{
  int value;
  enum borderType type;
  struct item *next;
} MergeItem;

typedef struct connectItem
{
  int target;
  int merge;
  struct doubleitem *next;
} ConnectItem;

typedef ConnectItem *ConnectList;

typedef struct border
{
  int klass;
  int isMerged;
  MergeItem *mergeWith;
  int left;
  int right;
  int top;
  int bottom;
  struct border *next;
} Border;

typedef Border *BorderList;

// void swap(int *p1, int *p2)
// {
//   int tmp = *p1;
//   *p1 = *p2;
//   *p2 = tmp;
// }

void addBorder(int klass, int width, int height, BorderList *plist)
{
  Border *pnew = (Border *)malloc(sizeof(Border));
  Border *scan = *plist;

  pnew->klass = klass;
  pnew->isMerged = 0;
  pnew->mergeWith = NULL;
  pnew->next = NULL;

  pnew->left = width;
  pnew->right = 0;
  pnew->top = height;
  pnew->bottom = 0;

  if (scan != NULL)
  {
    while (scan->next != NULL)
    {
      scan = scan->next;
    }

    scan->next = pnew;
  }
  else
  {
    *plist = pnew;
  }
}

// void addPoint(int klass, int val, enum borderType type, BorderList *plist)
// {
//   Border *scan = *plist;

//   do
//   {
//     if (scan->klass == klass)
//     {
//       switch (type)
//       {
//       case left:
//         if (scan->left > val)
//           scan->left = val;
//         break;
//       case right:
//         if (scan->right < val)
//           scan->right = val;
//         break;
//       case top:
//         if (scan->top > val)
//           scan->top = val;
//         break;
//       case bottom:
//         if (scan->bottom < val)
//           scan->bottom = val;
//         break;
//       default:
//       }

//       printf("add point: klass %d, val %d", klass, val);
//     }

//     scan = scan->next;
//   } while (scan != NULL);
// }

// void pushValueIntoConnects(int k1, int k2, DoubleItemList *plist)
// {
//   // printf("remove duplicate and sorted value: %d, %d\n", k1, k2);

//   DoubleItem *pnew = (DoubleItem *)malloc(sizeof(DoubleItem));

//   pnew->value1 = k1 < k2 ? k2 : k1;
//   pnew->value2 = k1 > k2 ? k2 : k1;
//   pnew->next = NULL;

//   DoubleItem *scan = *plist;

//   if (scan == NULL)
//   {
//     *plist = pnew;
//   }
//   else
//   {
//     while (scan->next != NULL)
//     {
//       scan = scan->next;
//     }

//     scan->next = pnew;
//   }
// }

// void borderConnect(int borderArray[], DoubleItemList *plist)
// {
//   // printf("border connect: %d, %d, %d, %d\n", borderArray[0], borderArray[1], borderArray[2], borderArray[3]);

//   int canBreak = 0;

//   for (int i = 0; i < 4; i++)
//   {
//     int currVal = borderArray[i];

//     if (currVal == 0)
//     {
//       continue;
//     }

//     for (int j = i + i; j < 4; j++)
//     {
//       int targetVal = borderArray[j];

//       if (currVal != targetVal && targetVal != 0)
//       {
//         pushValueIntoConnects(currVal, targetVal, plist);

//         canBreak = 1;

//         break;
//       }
//     }

//     if (canBreak == 1)
//     {
//       break;
//     }
//   }
// }

EM_PORT_API(uint8_t **)
pixelDataXOR(uint32_t pixelData1[], uint32_t pixelData2[], int width, int height)
{
  uint8_t **xorArray = (uint8_t **)malloc((height + 1) * sizeof(uint8_t *));

  for (int y = 0; y < height; y++)
  {
    xorArray[y] = (uint8_t *)calloc(width + 1, sizeof(uint8_t));

    for (int x = 0; x < width; x++)
    {
      if (pixelData1[y * width + x] ^ pixelData2[y * width + x])
      {
        xorArray[y][x] = 1;
      }
    }
  }

  xorArray[height] = (uint8_t *)calloc(width + 1, sizeof(uint8_t));

  return xorArray;
}

EM_PORT_API(void)
imagediff(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  uint8_t **xorArray = pixelDataXOR((uint32_t)pixelData1, (uint32_t)pixelData2, width, height);
  int *prevClasses = (int *)calloc(width + 2, sizeof(int));
  int *currClasses = (int *)calloc(width + 2, sizeof(int));
  int amount = 0;
  ConnectList *connects = NULL;
  BorderList *borders = NULL;

  for (int y = 0; y < height + 1; y++)
  {
    for (int x = 0; x < width + 1; x++)
    {
      int classX = x + 1;
      int xorVal = xorArray[y][x];
      int classOfLeft = currClasses[classX - 1];
      int classOfTop = prevClasses[classX];

      if (xorVal == 0)
      {
        if (classOfLeft != 0) // left point is 【right】 border
        {
          // addPoint(currClasses[classX - 1], x - 1, right, &borders);

          // int borderArray[] = {currClasses[classX - 2], prevClasses[classX - 2], prevClasses[classX - 1], prevClasses[classX]};

          // borderConnect(borderArray, &connects);
        }

        if (classOfTop != 0) // top point is 【bottom】 border
        {
          // addPoint(prevClasses[classX], y - 1, bottom, &borders);
        }

        currClasses[classX] = 0;
      }
      else
      {
        int classOfSurround = classOfLeft || prevClasses[classX - 1] || classOfTop || prevClasses[classX + 1];
        int classOfCurr = classOfSurround == 0 ? ++amount : classOfSurround;

        if (classOfLeft == 0) // current point is 【left】 border
        {
          if (classOfSurround == 0)
          {
            addBorder(classOfCurr, width, height, &borders);
          }

          // addPoint(classOfCurr, x, left, &borders);

          // int borderArray[] = {currClasses[x - 1], prevClasses[x - 1], prevClasses[x], prevClasses[x + 1]};

          // borderConnect(borderArray, &connects);
        }

        if (classOfTop == 0) // current point is 【top】 border
        {
          // addPoint(classOfCurr, y, top, &borders);

          if (classOfLeft != 0) // remove duplicate connection
          {
            // int borderArray[] = {currClasses[x - 1], prevClasses[x - 1], prevClasses[x], prevClasses[x + 1]};

            // borderConnect(borderArray, &connects);
          }
        }

        currClasses[classX] = classOfCurr;
      }
    }

    // swap(&prevClasses, &currClasses);
  }

  Border *scan = borders;

  printf("klass: %d\n", scan->klass);

  scan = scan->next;

  printf("klass: %d\n", scan->klass);
  // free(prevClasses);
  // free(currClasses);
  // free(connects);
  // free(borders);
  // free(xorArray);
}

EM_PORT_API(void)
imageDiffTest(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  // for (int i = 0; i < 1000; i++)
  // {

  imagediff(pixelData1, pixelData2, width, height);

  // uint8_t ** xor = pixelDataXOR((uint32_t)pixelData1, (uint32_t)pixelData2, width, height);

  // for (int x = 0; x < width; x++)
  // {
  //   for (int y = 0; y < height; y++)
  //   {
  //     printf("x: %d, y: %d, val: %d\n", x, y, xor[x][y]);
  //   }
  // }
}

void main()
{
  // const uint8_t template[] = {
  //     0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  //     0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
  //     0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  //     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  //     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
  //     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
  //     0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  //     0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  //     1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  //     1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // uint8_t arr3[102400 * 4];

  // for (int a = 0; a < 102400 * 4; a++)
  // {
  //   arr3[a] = template[a % 200];
  //   arr3[a + 1] = template[a % 200 + 1];
  //   arr3[a + 2] = template[a % 200 + 2];
  //   arr3[a + 3] = template[a % 200 + 3];
  // }

  // const uint8_t arr4[102400 * 4] = {0};

  uint8_t arr1[64] = {
      1, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 1, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0,
      1, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 1, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0};

  uint8_t arr2[64] = {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0};

  // time_t starttime = clock();

  imageDiffTest(arr1, arr2, 4, 4);

  // time_t endtime = clock();

  // printf("time: %f", difftime(endtime, starttime));

  system("pause");
}

// emcc color.c -O1 -s WASM=1 -s MODULARIZE=1 -s ASSERTIONS=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" -o color.js