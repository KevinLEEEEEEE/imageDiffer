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

typedef struct arrayItem
{
  int value[2];
  struct arrayitem *next;
} ArrayItem;

typedef ArrayItem *ArrayItemList;

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
  return (x >= width || y >= height) ? 0 : testBit(array, x + width * y);
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

void borderConnect(int k1, int k2, int k3, int k4)
{
}

EM_PORT_API(uint8_t *)
pixelDataXOR(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  const int pixelArraySize = width * height * SIZE_OF_RGBA;
  const int bitArraySize = ceil((width * height) / 8) * 8;
  uint8_t *const bitArray = (uint8_t *)malloc(bitArraySize);

  memset(bitArray, 0, bitArraySize);

  for (int i = 0; i < pixelArraySize; i += 4)
  {
    int isChanged = (pixelData1[i] ^ pixelData2[i]) ||
                    (pixelData1[i + 1] ^ pixelData2[i + 1]) ||
                    (pixelData1[i + 2] ^ pixelData2[i + 2]) ||
                    (pixelData1[i + 3] ^ pixelData2[i + 3]);

    if (isChanged != 0)
    {
      setBit(bitArray, i / 4);
    }
  }

  return bitArray;
}

EM_PORT_API(uint8_t *)
imagediff(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  const uint8_t *const xorArray = pixelDataXOR(pixelData1, pixelData2, width, height);
  int *prevClass = (int *)calloc(width + 2, sizeof(int));
  int *currClass = (int *)calloc(width + 2, sizeof(int));
  int amount = 0;
  ArrayItemList *connects = NULL;
  BorderList *borders = NULL;

  for (int y = 0; y < height + 1; y++)
  {
    for (int x = 1; x < width + 2; x++)
    {
      int realX = x - 1;
      int xorVal = getXorValue(xorArray, width, height, realX, y);
      int classOfLeft = currClass[x - 1];
      int classOfTop = prevClass[x];

      if (xorVal == 0)
      {
        if (classOfLeft != 0) // left point is 【right】 border
        {
          addPoint(currClass[x - 1], realX - 1, right, &borders);

          borderConnect();
        }

        if (classOfTop != 0) // top point is 【bottom】 border
        {
          addPoint(prevClass[x], y - 1, bottom, &borders);
        }

        currClass[x] = 0;
      }
      else
      {
        int classOfSurround = classOfLeft || prevClass[x - 1] || classOfTop || prevClass[x + 1];
        int classOfCurr = classOfSurround == 0 ? ++amount : classOfSurround;

        if (classOfLeft == 0) // current point is 【left】 border
        {
          if (classOfSurround == 0)
          {
            addBorder(classOfCurr, &borders);
          }

          addPoint(classOfCurr, realX, left, &borders);

          borderConnect();
        }

        if (classOfTop == 0) // current point is 【top】 border
        {
          addPoint(classOfCurr, y, top, &borders);

          if (classOfLeft != 0) // no border connect before
          {
            borderConnect();
          }
        }

        currClass[x] = classOfCurr;
      }
    }

    swap(&prevClass, &currClass);
  }

  // print values

  Border *scan = borders;

  printf("klass: %d\n", scan->klass);

  Item *scan2 = scan->border;

  printf("value: %d, type: %d\n", scan2->value, scan2->type);

  while (scan2->next != NULL)
  {
    scan2 = scan2->next;

    printf("value: %d, type: %d\n", scan2->value, scan2->type);
  }

  while (scan->next != NULL)
  {
    scan = scan->next;

    printf("klass: %d\n", scan->klass);

    Item *scan3 = scan->border;

    printf("value: %d, type: %d\n", scan3->value, scan3->type);

    while (scan3->next != NULL)
    {
      scan3 = scan3->next;

      printf("value: %d, type: %d\n", scan3->value, scan3->type);
    }
  }
}

void main()
{
  uint8_t arr1[32] = {
      1, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0};

  uint8_t arr2[32] = {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0};

  imagediff(arr1, arr2, 4, 2);

  system("pause");
}

// emcc color.c -O1 -s WASM=1 -s MODULARIZE=1 -s ASSERTIONS=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'getValue', 'setValue']" -o color.js