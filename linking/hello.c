#include "d_lib.h"
#include "s_lib.h"
#include <dlfcn.h>
#include <stdio.h>

void wrapper() {
  void *handle;
  const char *lib_name = "./libdl.so";
  handle = dlopen(lib_name, RTLD_LAZY);
  if (!handle) {
    printf("Failed to open dynamic library \"%s\": %s\n", lib_name, dlerror());
    return;
  }
  void (*func)() = dlsym(handle, "hello_from_dlopen");
  if (func) {
    func();
  } else {
    printf("Failed to get function pointer\n");
  }
  dlclose(handle);
}

int main(int argc, char *argv[]) {

  printf("hello, world\n");

  hello_from_static_lib();

  hello_from_dynamic_lib();

  wrapper();

  return 0;
}
