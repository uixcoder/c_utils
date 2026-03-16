#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Тип 1: Прямий витік
void direct_leak() {
  malloc(1024);  // Прямий витік - не зберігаємо вказівник
  printf("Створено прямий витік 1024 байт\n");
}
// Тип 2: Втрата вказівника через переприсвоєння
void pointer_reassignment_leak() {
  void* first_block = malloc(512);
  void* second_block = malloc(256);
  first_block = second_block;  // Втрачаємо перший блок
  free(first_block);
  printf("Створено витік через втрату вказівника: 512 байт\n");
}
// Тип 3: Умовний витік
void conditional_leak(int condition) {
  char* buffer = malloc(100);
  if (condition) {
    return;  // Вихід без звільнення пам'яті
  }
  free(buffer);
}
// Тип 4: Витік у циклі
void loop_leak(int iterations) {
  for (int i = 0; i < iterations; i++) {
    malloc(64);  // Витік на кожній ітерації
  }
  printf("Створено витік у циклі: %d × 64 байт\n", iterations);
}
// Тип 5: Витік у структурі даних
typedef struct TreeNode {
  int data;
  struct TreeNode* left;
  struct TreeNode* right;
} TreeNode;
void tree_leak() {
  TreeNode* root = malloc(sizeof(TreeNode));
  root->left = malloc(sizeof(TreeNode));
  root->right = malloc(sizeof(TreeNode));

  free(root);  // Забуваємо про left та right
  printf("Створено витік у структурі дерева\n");
}
// Тип 6: Витік через неправильне використання realloc
void realloc_leak() {
  int* array = malloc(5 * sizeof(int));
  for (int i = 0; i < 5; i++) {
    array[i] = i;
  }
  // Небезпечний виклик realloc
  int* new_array = realloc(array, 10 * sizeof(int));
  if (new_array == NULL) {
    // Якщо realloc не вдався, array все ще валідний,
    // але ми його не звільняємо
    return;
  }
  // Втрачаємо старий вказівник, якщо realloc перемістив блок
  array = new_array;

  free(array);
  printf("Продемонстровано проблеми з realloc\n");
}
// Тип 7: Витік у рекурсивній функції
void recursive_leak(int depth) {
  if (depth <= 0) return;

  int* data = malloc(128);
  recursive_leak(depth - 1);
  // Забуваємо free(data) - витік на кожному рівні рекурсії
}
int main() {
  printf("=== Демонстрація різних типів витоків пам'яті ===\n\n");
  direct_leak();
  pointer_reassignment_leak();
  conditional_leak(1);  // Викликаємо умовний витік
  loop_leak(5);
  tree_leak();
  realloc_leak();
  recursive_leak(3);
  printf("\n=== Тест завершено ===\n");
  return 0;
}
