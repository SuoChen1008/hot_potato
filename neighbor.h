#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

class neighborData {
  public:
    char ip[100];
    int port;
    neighborData() : port(-1) {
      memset(ip, 0, 100);
    }
};
