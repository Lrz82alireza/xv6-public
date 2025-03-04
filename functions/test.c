#include <stdio.h>


const char* find_prefix_match() {
  for (int i = cmd_history.size -1 ; i >= 0; i--) {
      const char* candidate = cmd_history.buf[i];
      char* temp;
  
      strSplit(temp, input.buf, input.r, input.w - 1);
      
      // cprintf("candidate : %s\n", candidate);
      int j = 0;
      while (temp[j] != '\0' && candidate[j] != '\0' && temp[j] == candidate[j]) {
          j++;
      }
      if (temp[j] == '\0') {
          return candidate;
      }
  }
  return NULL;
}

const char* processTabCommand()
{
  const char* result = find_prefix_match();
  cprintf("result : %s\n", result);
  return result;
}

main()
{
  
}