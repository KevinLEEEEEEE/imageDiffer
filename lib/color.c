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

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_OF_RGBA 4;

enum borderType
{
  left,
  right,
  top,
  bottom
};

typedef struct connect
{
  int merge;
  int to;
  struct connect *next;
} Connect;

typedef struct border
{
  int klass;
  int isMerged;
  int left;
  int right;
  int top;
  int bottom;
  struct border *next;
} Border;

void swap(int **p1, int **p2)
{
  int *tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

void addBorder(int klass, int width, int height, Border *pointer)
{
  Border *pnew = (Border *)malloc(sizeof(Border));
  Border *scan = pointer;

  pnew->klass = klass;
  pnew->isMerged = 0;
  pnew->next = NULL;

  pnew->left = width;
  pnew->right = 0;
  pnew->top = height;
  pnew->bottom = 0;

  while (scan->next != NULL)
  {
    scan = scan->next;
  }

  scan->next = pnew;
}

void addPoint(int klass, int val, enum borderType type, Border *pointer)
{
  Border *scan = pointer;

  do
  {
    scan = scan->next;

    if (scan->klass == klass)
    {
      switch (type)
      {
      case left:
        if (scan->left > val)
          scan->left = val;
        break;
      case right:
        if (scan->right < val)
          scan->right = val;
        break;
      case top:
        if (scan->top > val)
          scan->top = val;
        break;
      case bottom:
        if (scan->bottom < val)
          scan->bottom = val;
        break;
      default:
        break;
      }

      break;
    }
  } while (scan->next != NULL);
}

int removeDuplicateAndZero(int borderArray[], Connect *pnew)
{
  for (int i = 0; i < 4; i++)
  {
    int currVal = borderArray[i];

    if (currVal == 0)
    {
      continue;
    }

    for (int j = i + 1; j < 4; j++)
    {
      int targetVal = borderArray[j];

      if (currVal != targetVal && targetVal != 0)
      {
        pnew->merge = currVal < targetVal ? currVal : targetVal; // small number
        pnew->to = currVal > targetVal ? currVal : targetVal;    // large number
        pnew->next = NULL;

        return 1;
      }
    }
  }

  return 0;
}

void addConnect(int borderArray[], Connect *pointer)
{
  Connect *pnew = (Connect *)malloc(sizeof(Connect));
  int state = removeDuplicateAndZero(borderArray, pnew);

  if (state != 0)
  {
    Connect *scan = pointer;

    while (scan->next != NULL)
    {
      scan = scan->next;

      if (scan->merge == pnew->merge && scan->to == pnew->to) // remove duplicate connection
      {
        return;
      }
    }

    scan->next = pnew;
  }
}

void connectBorderBetween(int merge, int to, Border *pointer)
{
  Border *scan = pointer;
  Border *pMerge = NULL, *pTo = NULL;

  while (scan->next != NULL)
  {
    scan = scan->next;

    if (scan->klass == merge)
    {
      pMerge = scan;
    }
    else if (scan->klass == to)
    {
      pTo = scan;
    }
  }

  if (pMerge->isMerged == 1)
  {
    return;
  }

  pMerge->isMerged = 1;

  if (pMerge->left < pTo->left)
  {
    pTo->left = pMerge->left;
  }

  if (pMerge->right > pTo->right)
  {
    pTo->right = pMerge->right;
  }

  if (pMerge->top < pTo->top)
  {
    pTo->top = pMerge->top;
  }

  if (pMerge->bottom > pTo->bottom)
  {
    pTo->bottom = pMerge->bottom;
  }
}

void connectBorders(Border *pBorder, Connect *pConnect)
{
  Connect *scan = pConnect;

  while (scan->next != NULL)
  {
    scan = scan->next;

    connectBorderBetween(scan->merge, scan->to, pBorder);
  }
}

Border *initBorderList()
{
  Border *first = (Border *)malloc(sizeof(Border));

  first->klass = 0;
  first->next = NULL;

  return first;
}

Connect *initConnectList()
{
  Connect *first = (Connect *)malloc(sizeof(Connect));

  first->next = NULL;
  first->merge = 0;
  first->to = 0;

  return first;
}

int *initClasses(int size)
{
  return (int *)calloc(size, sizeof(int));
}

uint8_t **pixelDataXOR(uint32_t pixelData1[], uint32_t pixelData2[], int width, int height)
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

void printBorders(Border *pointer)
{
  Border *scan = pointer;

  while (scan->next != NULL)
  {
    scan = scan->next;

    printf("klass: %d, isMerged: %d, left: %d, right: %d, top: %d, bottom: %d\n", scan->klass, scan->isMerged, scan->left, scan->right, scan->top, scan->bottom);
  }
}

EM_PORT_API(void)
imagediff(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  uint8_t **xorArray = pixelDataXOR((uint32_t)pixelData1, (uint32_t)pixelData2, width, height);
  int *prevClasses = initClasses(width + 2);
  int *currClasses = initClasses(width + 2);
  int amount = 0;
  Connect *connects = initConnectList();
  Border *borders = initBorderList();

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
          addPoint(currClasses[classX - 1], x - 1, right, borders);

          int borderArray[4] = {currClasses[classX - 2], prevClasses[classX - 2], prevClasses[classX - 1], prevClasses[classX]};

          addConnect(borderArray, connects);
        }

        if (classOfTop != 0) // top point is 【bottom】 border
        {
          addPoint(prevClasses[classX], y - 1, bottom, borders);
        }

        currClasses[classX] = 0;
      }
      else
      {
        int classOfSurround = classOfLeft != 0 ? classOfLeft : prevClasses[classX - 1] != 0 ? prevClasses[classX - 1] : classOfTop != 0 ? classOfTop : prevClasses[classX + 1];
        int classOfCurr = classOfSurround == 0 ? ++amount : classOfSurround;

        if (classOfLeft == 0) // current point is 【left】 border
        {
          if (classOfSurround == 0)
          {
            addBorder(classOfCurr, width, height, borders);
          }

          addPoint(classOfCurr, x, left, borders);

          int borderArray[4] = {classOfLeft, prevClasses[classX - 1], classOfTop, prevClasses[classX + 1]};

          addConnect(borderArray, connects);
        }

        if (classOfTop == 0) // current point is 【top】 border
        {
          addPoint(classOfCurr, y, top, borders);

          if (classOfLeft != 0) // remove duplicate connect
          {
            int borderArray[4] = {classOfLeft, prevClasses[classX - 1], classOfTop, prevClasses[classX + 1]};

            addConnect(borderArray, connects);
          }
        }

        currClasses[classX] = classOfCurr;
      }
    }

    swap(&prevClasses, &currClasses);
  }

  connectBorders(borders, connects);

  printBorders(borders);

  free(xorArray);
  free(prevClasses);
  free(currClasses);
  free(borders);
  free(connects);
}

void main()
{
  const uint8_t template[] = {
      0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
      0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
      0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
      1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
      1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint8_t arr3[102400 * 4];
  uint8_t arr4[102400 * 4] = {0};

  for (int a = 0; a < 102400 * 4; a++)
  {
    arr3[a] = template[a % 200];
    arr3[a + 1] = template[a % 200 + 1];
    arr3[a + 2] = template[a % 200 + 2];
    arr3[a + 3] = template[a % 200 + 3];
  }

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

  time_t starttime = clock();

  imagediff(arr1, arr2, 4, 4);

  // imagediff(arr3, arr4, 320, 320);

  time_t endtime = clock();

  printf("time: %f", difftime(endtime, starttime));

  system("pause");
}

// emcc color.c -O1 -s WASM=1 -s MODULARIZE=1 -s ASSERTIONS=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" -o color.js