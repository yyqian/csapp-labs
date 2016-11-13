#include <stdio.h>

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf(" %.2x", start[i]);
  }
  printf("\n");
}

void show_int(int x) {
  show_bytes((byte_pointer) &x, sizeof(int));
}

void show_float(float x) {
  show_bytes((byte_pointer) &x, sizeof(float));
}

void show_pointer(void *x) {
  show_bytes((byte_pointer) &x, sizeof(void *));
}

void test_show_bytes(int val) {
  show_int(val);
  show_float((float) val);
  show_pointer(&val);
}

void test_show_bytes_via_ptr(int* val) {
  show_int(*val);
  show_float((float) *val);
  show_pointer(val);
}

int main() {
  int ary[] = {1, 4096};
  printf("通过值传递数组：\n");
  test_show_bytes(ary[0]);
  test_show_bytes(*(ary + 1));
  printf("通过指针传递数组：\n");
  test_show_bytes_via_ptr(ary);
  test_show_bytes_via_ptr(ary + 1);
  printf("12345：\n");
  test_show_bytes(12345);
  printf("12345 >> 4：\n");
  test_show_bytes(12345 >> 4);
  printf("12345 << 4：\n");
  test_show_bytes(12345 << 4);
  printf("-12345：\n");
  test_show_bytes(-12345);
  printf("-12345 >> 4：\n");
  test_show_bytes(-12345 >> 4);
  printf("-12345 << 4：\n");
  test_show_bytes(-12345 << 4);
}
