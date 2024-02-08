#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

class myPotato {
  public:
    int nhops;
    int count;
    int path[512];
    myPotato() : nhops(0), count(0) { memset(path, 0, 512); }
};

