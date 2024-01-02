#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
   char str1[1001]={ 0, };
   char str2[1001][1001] = { 0, };
   char temp[1001] = { 0, };

   scanf("%s", str1);

   int len = strlen(str1);

   for (int i = 0; i < len; i++) {
      for (int j = 0; j < len; j++) {
         str2[i][j-i] = str1[j]; //index error 발생 예상 
      }
   }

   //if baekjoon    8
   for (int i = 0; i < len - 1; i++) {
      for (int j = i; j < len - 2 - i; j++) {
         if (strcmp(str2[j], str2[j + 1]) > 0) { 
        // 왜갑자기 1차원 배열로 바뀐것인가? str2는 2차원 배열이었는
            strcpy(temp, str2[j]);
            strcpy(str2[j], str2[j + 1]);
            strcpy(str2[j + 1], temp);
         }
      }
   }
   for (int i = 0; i < len; i++) {
      printf("%s\n", str2[i]);
   }

   return 0;
}