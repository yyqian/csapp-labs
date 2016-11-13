#include <stdio.h>

void inplace_swap(int *x, int *y) {
  if (x == y) {
    return;
  }
  *y = *x ^ *y;
  *x = *x ^ *y;
  *y = *x ^ *y;
}

void reverse_array(int a[], int cnt) {
  int first, last;
  for (first = 0, last = cnt - 1; first < last; first++, last--) {
    inplace_swap(a + first, a + last);
  }
}

void print_array(int a[], int cnt) {
  printf("{%d", a[0]);
  for (int i = 1; i < cnt; i++) {
    printf(", %d", a[i]);
  }
  printf("}\n");
}

int main() {
  int x = 10;
  int y = 20;
  printf("Original: x=%d, y=%d\n", x, y);
  inplace_swap(&x, &y);
  printf("swap x and y: x=%d, y=%d\n", x, y);
  inplace_swap(&x, &x);
  printf("swap x and x: x=%d, y=%d\n", x, x);
  int ary[] = {1,2,3,4,5};
  printf("Original array: ");
  print_array(ary, 5);
  reverse_array(ary, 5);
  printf("Reversed array: ");
  print_array(ary, 5);
}