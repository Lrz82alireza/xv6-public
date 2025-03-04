#include <stdio.h>
#include <stdlib.h>

// Exclemation command
#define EXCLAMATION 6
#define KEYWORDS_CNT 7
const char *KEYWORDS[KEYWORDS_CNT] = {"void", "int", "char", "if", "for", "while", "return"};
const char *DELIMITER = " ";
const char EXCLAMATION_CHAR = '!';
const int INPUT_BUF = 128;
const int COLOR_LEN = 7;
const int RST_COLOR_LEN = 4;

const int MIN_KEYWORD_LEN = 2;
const int MAX_KEYWORD_LEN = 6;

const int OUTPUT_BUF = (COLOR_LEN + RST_COLOR_LEN) * (INPUT_BUF / MIN_KEYWORD_LEN) + INPUT_BUF;




// <string.h> standar functions
char* strchr(const char* str, int c);
char* strtok(char* str, const char* delimiters);
int strcmp(const char* str1, const char* str2);
size_t strlen(const char* str);
char* strcat(char* dest, const char* src);
char* strcpy(char* dest, const char* src);

// <string.h> standar functions
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    return NULL
  ;
  }
  
  char* strtok(char* str, const char* delimiters) {
    static char* last = NULL
  ;
    if (str == NULL
  ) str = last;
  
    if (str == NULL
  ) return NULL
  ;
  
    while (*str && strchr(delimiters, *str)) str++;
  
    if (*str == '\0') return NULL
  ;
  
    char* start = str;
  
    while (*str && !strchr(delimiters, *str)) str++;
  
    if (*str) {
        *str = '\0';
        last = str + 1;
    } else {
        last = NULL
      ;
    }
  
    return start;
  }
  

  int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
  }
  
  size_t strlen(const char* str) {
    size_t length = 0;
    while (*str++) {
        length++;
    }
    return length;
  }
  
  char* strcat(char* dest, const char* src) {
    char* orig_dest = dest;
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return orig_dest;
  }
  
  char* strcpy(char* dest, const char* src) {
    char* orig_dest = dest;
  
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
  
    *dest = '\0';
    return orig_dest;
  }
  /////////////////////////////////////////////////

  // ! processing
  int is_keyword(const char* word) {
    for (int i = 0; i < KEYWORDS_CNT; i++) {
        if (strcmp(word, KEYWORDS[i]) == 0) {
            return 1; //found
        }
    }
    return 0;//not found
  }

  void colorize_keywords(char *cmd) {
    char buf[OUTPUT_BUF];  
    char *token;
  
    token = strtok(cmd, DELIMITER);
    while (token != NULL) {
        
        if (is_keyword(token)) {
            strcat(buf, "\033[34m"); 
            strcat(buf, token);
            strcat(buf, "\033[0m"); 
        } else {
            strcat(buf, token);
        }
        strcat(buf, " ");
        token = strtok(NULL, DELIMITER);
    }
  
    strcpy(cmd, buf);
  }


  void remove_between_sharps(char *str) {
  int i, j = 0;
  int in_sharp = 0;

  for (i = 0; str[i] != '\0'; i++) {
      if (str[i] == '#') {
          in_sharp = !in_sharp; 
          continue; 
      }
      if (!in_sharp) {
          str[j++] = str[i];
      }
  }
  str[j] = '\0';
    }

  void exclamation_process(char *cmd)
    {
      remove_between_sharps(cmd);
      colorize_keywords(cmd);
    }
/////////////////////////////////////////////// 


int main() {
    char command[] = "if"; // متغیر محلی به جای اشاره‌گر به رشته ثابت
    char *token = strtok(command, DELIMITER);

    while (token != NULL) {
        printf("%s\n", token);
        if(strcmp(token , "if"))
            printf("IF\n");
        token = strtok(NULL, DELIMITER);
    }

    return 0;
}
