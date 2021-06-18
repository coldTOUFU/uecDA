void swap_num(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void quicksort(int a[], int left, int right) {
  int pivot, l, r;

  if (left >= right) { return; }

  pivot = a[left];
  l = left; r = right;

  while(1) {
    while (a[l] < pivot) { l++; }

    while (a[r] > pivot) { r--; }

    if (l >= r) { break; }

    swap_num(&a[l], &a[r]);

    l++; r--;
  }

  quicksort(a, left, l-1);
  quicksort(a, r+1, right);
}
