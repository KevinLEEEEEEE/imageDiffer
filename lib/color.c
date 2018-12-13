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

uint8_t *getBitArray(int size)
{
  return (uint8_t *)malloc(ceil(size / 8));
}

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

EM_PORT_API(uint8_t *)
pixelDataXOR(uint8_t pixelData1[], uint8_t pixelData2[], int width, int height)
{
  const int pixelArraySize = width * height * SIZE_OF_RGBA;
  uint8_t *array = (uint8_t *)malloc(pixelArraySize * sizeof(uint8_t));

  for (int i = 0; i < pixelArraySize; i += 4)
  {
    array[i] = pixelData1[i] ^ pixelData2[i];
    array[i + 1] = pixelData1[i + 1] ^ pixelData2[i + 1];
    array[i + 2] = pixelData1[i + 2] ^ pixelData2[i + 2];
    array[i + 3] = pixelData1[i + 3] ^ pixelData2[i + 3];
  }

  return array;
}

EM_PORT_API(uint8_t *)
pixelDataToBit(uint8_t pixelData[], int width, int height)
{
  const int arraySize = width * height;
  const int pixelArraySize = arraySize * SIZE_OF_RGBA;
  uint8_t *array = getBitArray(arraySize);

  memset(array, 0, arraySize);

  for (int i = 0; i < pixelArraySize; i += 4)
  {
    if (pixelData[i] != 0 || pixelData[i + 1] != 0 || pixelData[i + 2] != 0 || pixelData[i + 3] != 0)
    {
      setBit(array, i / 4);
    }
  }

  return array;
}

EM_PORT_API(uint8_t *)
drawBorderFromPixelData(uint8_t pixelData[], int width, int height)
{
  const uint8_t bitArray = pixelDataToBit(pixelData, width, height);
  const uint8_t borderArray = (uint8_t *)malloc(width * height * sizeof(uint8_t));

  for (int i = 0; i < width * height; i++)
  {
  }

  return borderArray;
}

void main()
{
  printf("hello\n");

  uint8_t arr1[32] = {
      1, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0};

  uint8_t *res = pixelDataToByte(arr1, 2, 4);

  printf("%d\n", testBit(res, 0));
  printf("%d\n", testBit(res, 1));
  printf("%d\n", testBit(res, 2));
  printf("%d\n", testBit(res, 3));
  printf("%d\n", testBit(res, 4));
  printf("%d\n", testBit(res, 5));
  printf("%d\n", testBit(res, 6));
  printf("%d\n", testBit(res, 7));

  system("pause");
}

// emcc color.c -O1 -s WASM=1 -s MODULARIZE=1 -s ASSERTIONS=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'getValue', 'setValue']" -o color.js