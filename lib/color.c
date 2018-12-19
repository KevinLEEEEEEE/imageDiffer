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

typedef struct item
{
  int value;
  enum borderType type;
  struct item *next;
} Item;

typedef struct doubleItem
{
  int value1;
  int value2;
  struct doubleitem *next;
} DoubleItem;

typedef DoubleItem *DoubleItemList;

typedef struct border
{
  int klass;
  int isMerged;
  Item *mergeWith;
  Item *border;
  Item *borderHead;
  struct border *next;
} Border;

typedef Border *BorderList;

void setBit(uint8_t array[], int k)
{
  int index = k / 8;
  int offset = k % 8;

  unsigned int flag = 1;

  flag <<= offset;

  array[index] |= flag;
}

void clearBit(uint8_t array[], int k)
{
  int index = k / 8;
  int offset = k % 8;

  unsigned int flag = 1;

  flag <<= offset;

  array[index] &= ~flag;
}

int testBit(uint8_t array[], int k)
{
  int index = k / 8;
  int offset = k % 8;

  unsigned int flag = 1;

  flag <<= offset;

  return (array[index] & flag) != 0;
}

int getXorValue(uint8_t array[], int width, int height, int x, int y)
{
  // return (x >= width || y >= height) ? 0 : testBit(array, x + width * y);

  return (x >= width || y >= height) ? 0 : array[y * width + x];
}

void swap(int *p1, int *p2)
{
  int tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

void addBorder(int klass, BorderList *plist)
{
  Border *pnew = (Border *)malloc(sizeof(Border));
  Border *scan = *plist;

  memset(pnew, 0, sizeof(Border));

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

  pnew->klass = klass;
}

void addPointToBorder(int val, enum borderType type, Border *border)
{
  Item *newItem = (Item *)malloc(sizeof(Item));

  newItem->value = val;
  newItem->type = type;
  newItem->next = NULL;

  if (border->borderHead != NULL)
  {
    border->borderHead->next = newItem;

    border->borderHead = newItem;
  }
  else
  {
    border->borderHead = newItem;

    border->border = newItem;
  }
}

void addPoint(int klass, int val, enum borderType type, BorderList *plist)
{
  Border *scan = *plist;

  do
  {
    if (scan->klass == klass)
    {
      addPointToBorder(val, type, scan);
    }

    scan = scan->next;
  } while (scan != NULL);
}

void pushValueIntoConnects(int k1, int k2, DoubleItemList *plist)
{
  // printf("remove duplicate and sorted value: %d, %d\n", k1, k2);

  DoubleItem *pnew = (DoubleItem *)malloc(sizeof(DoubleItem));

  pnew->value1 = k1 < k2 ? k2 : k1;
  pnew->value2 = k1 > k2 ? k2 : k1;
  pnew->next = NULL;

  DoubleItem *scan = *plist;

  if (scan == NULL)
  {
    *plist = pnew;
  }
  else
  {
    while (scan->next != NULL)
    {
      scan = scan->next;
    }

    scan->next = pnew;
  }
}

void borderConnect(int borderArray[], DoubleItemList *plist)
{
  // printf("border connect: %d, %d, %d, %d\n", borderArray[0], borderArray[1], borderArray[2], borderArray[3]);

  int canBreak = 0;

  for (int i = 0; i < 4; i++)
  {
    int currVal = borderArray[i];

    if (currVal == 0)
    {
      continue;
    }

    for (int j = i + i; j < 4; j++)
    {
      int targetVal = borderArray[j];

      if (currVal != targetVal && targetVal != 0)
      {
        pushValueIntoConnects(currVal, targetVal, plist);

        canBreak = 1;

        break;
      }
    }

    if (canBreak == 1)
    {
      break;
    }
  }
}

EM_PORT_API(uint8_t *)
pixelDataXOR(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  const int pixelArraySize = width * height * SIZE_OF_RGBA;
  // const int bitArraySize = ceil((width * height) / 8) * 8;
  // uint8_t *const bitArray = (uint8_t *)malloc(bitArraySize);

  uint8_t *const bitArray = calloc(width * height, sizeof(uint8_t));

  // memset(bitArray, 0, bitArraySize);

  for (int i = 0; i < pixelArraySize; i += 4)
  {
    int isChanged = (pixelData1[i] ^ pixelData2[i]) ||
                    (pixelData1[i + 1] ^ pixelData2[i + 1]) ||
                    (pixelData1[i + 2] ^ pixelData2[i + 2]) ||
                    (pixelData1[i + 3] ^ pixelData2[i + 3]);

    if (isChanged != 0)
    {
      // setBit(bitArray, i / 4);

      bitArray[i / 4] = 1;
    }
  }

  return bitArray;
}

EM_PORT_API(int)
imagediff(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  const uint8_t *const xorArray = pixelDataXOR(pixelData1, pixelData2, width, height);
  int *prevClasses = (int *)calloc(width + 2, sizeof(int));
  int *currClasses = (int *)calloc(width + 2, sizeof(int));
  int amount = 0;
  DoubleItemList *connects = NULL;
  BorderList *borders = NULL;

  for (int y = 0; y < height + 1; y++)
  {
    for (int x = 1; x < width + 2; x++)
    {
      int realX = x - 1;
      int xorVal = getXorValue(xorArray, width, height, realX, y);
      int classOfLeft = currClasses[x - 1];
      int classOfTop = prevClasses[x];

      if (xorVal == 0)
      {
        if (classOfLeft != 0) // left point is 【right】 border
        {
          addPoint(currClasses[x - 1], realX - 1, right, &borders);

          int borderArray[] = {currClasses[x - 2], prevClasses[x - 2], prevClasses[x - 1], prevClasses[x]};

          borderConnect(borderArray, &connects);
        }

        if (classOfTop != 0) // top point is 【bottom】 border
        {
          addPoint(prevClasses[x], y - 1, bottom, &borders);
        }

        currClasses[x] = 0;
      }
      else
      {
        int classOfSurround = classOfLeft || prevClasses[x - 1] || classOfTop || prevClasses[x + 1];
        int classOfCurr = classOfSurround == 0 ? ++amount : classOfSurround;

        if (classOfLeft == 0) // current point is 【left】 border
        {
          if (classOfSurround == 0)
          {
            addBorder(classOfCurr, &borders);
          }

          addPoint(classOfCurr, realX, left, &borders);

          int borderArray[] = {currClasses[x - 1], prevClasses[x - 1], prevClasses[x], prevClasses[x + 1]};

          borderConnect(borderArray, &connects);
        }

        if (classOfTop == 0) // current point is 【top】 border
        {
          addPoint(classOfCurr, y, top, &borders);

          if (classOfLeft != 0) // remove duplicate connection
          {
            int borderArray[] = {currClasses[x - 1], prevClasses[x - 1], prevClasses[x], prevClasses[x + 1]};

            borderConnect(borderArray, &connects);
          }
        }

        currClasses[x] = classOfCurr;
      }
    }

    swap(&prevClasses, &currClasses);
  }

  free(prevClasses);
  free(currClasses);
  free(connects);
  free(borders);
  free(xorArray);

  return amount;
}

EM_PORT_API(int)
imageDiffTest(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  time_t starttime = time(NULL);

  for (int i = 0; i < 100; i++)
  {
    imagediff(pixelData1, pixelData2, width, height);
  }

  time_t endtime = time(NULL);

  return (int)difftime(endtime, starttime);
}

// void main()
// {
//   const uint8_t template[] = {
//       0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
//       0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
//       0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
//       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
//       0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//       0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
//       1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
//       1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//   uint8_t arr3[102400 * 4];

//   for (int a = 0; a < 102400 * 4; a++)
//   {
//     arr3[a] = template[a % 200];
//     arr3[a + 1] = template[a % 200 + 1];
//     arr3[a + 2] = template[a % 200 + 2];
//     arr3[a + 3] = template[a % 200 + 3];
//   }

//   const uint8_t arr4[102400 * 4] = {0};

//   // uint8_t arr1[64] = {
//   //     1, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 1, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 1, 0, 1, 0, 0, 0,
//   //     0, 0, 0, 0, 1, 0, 0, 0,
//   //     1, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 1, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 1, 0, 1, 0, 0, 0,
//   //     0, 0, 0, 0, 1, 0, 0, 0};

//   // uint8_t arr2[64] = {
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0,
//   //     0, 0, 0, 0, 0, 0, 0, 0};

//   time_t starttime = clock();

//   for (int a = 0; a < 1000; a++)
//   {
//     imagediff(arr4, arr3, 320, 320);
//   }

//   time_t endtime = clock();

//   double durationtime = (double)(endtime - starttime);

//   printf("time: %f", difftime(endtime, starttime));

//   system("pause");
// }

// emcc color.c -O1 -s WASM=1 -s MODULARIZE=1 -s ASSERTIONS=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'getValue', 'setValue']" -o color.js