#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "logger.h"
#include "proxy.h"
#include "thread_pool.h"

#define MAX_THREADS 10
#define MAX_TASKS 100

t_proxy *p_proxy = NULL;

void sigint_handler(int sig) {
  if (!p_proxy) {
    return;
  }

  p_proxy->running = 0;
  log_info("Proxy stopped by SIGINT", NULL);
}

int setup_signal_handlers() {
  struct sigaction s_sigaction;
  s_sigaction.sa_handler = sigint_handler;
  sigemptyset(&s_sigaction.sa_mask);
  s_sigaction.sa_flags = 0;

  int err = sigaction(SIGINT, &s_sigaction, NULL);
  if (err) {
    log_debug("Failed to install sigint handler", NULL);
    return -1;
  }

  s_sigaction.sa_handler = SIG_IGN;
  err = sigaction(SIGPIPE, &s_sigaction, NULL);
  if (err) {
    log_debug("Failed to install SIGPIPE handler", NULL);
    return -1;
  }

  log_debug("Signal handlers set up successfully", NULL);

  return 0;
}

int main() {
  int err = logger_create(debug);
  if (err < 0) {
    printf(ANSI_COLOR_BLUE "[INFO]: Failed to initialize logger" ANSI_COLOR_BLUE
                           "\n");
    return EXIT_FAILURE;
  }

  err = setup_signal_handlers();
  if (err < 0) {
    log_info("Failed to set up signal handlers", NULL);
    logger_destroy();
    return EXIT_FAILURE;
  }

  t_thread_pool *p_thread_pool = thread_pool_create(MAX_THREADS, MAX_TASKS);
  if (!p_thread_pool) {
    log_info("Failed to create thread pool", NULL);
    logger_destroy();
    return EXIT_FAILURE;
  }

  p_proxy = proxy_create(p_thread_pool);
  if (!p_proxy) {
    thread_pool_destroy(p_thread_pool);
    log_info("Failed to create proxy", NULL);
    logger_destroy();
    return EXIT_FAILURE;
  }

  err = proxy_start(p_proxy);
  if (err < 0) {
    proxy_destroy(p_proxy);
    thread_pool_destroy(p_thread_pool);
    log_info("Failed to start proxy", NULL);
    logger_destroy();
    return EXIT_FAILURE;
  }

  proxy_destroy(p_proxy);
  thread_pool_destroy(p_thread_pool);
  logger_destroy();
  return EXIT_SUCCESS;
}
