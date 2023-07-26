/*
 * The point of this program is to crash in a multi-threaded app.
 * There are seven threads, doing the following things:
 * * Spinning
 * * Spinning inside a signal handler
 * * Spinning inside a signal handler executing on the altstack
 * * In a syscall
 * * In a syscall inside a signal handler
 * * In a syscall inside a signal handler executing on the altstack
 * * Finally, the main thread crashes in main, with no frills.
 *
 * These are the things threads in JRockit tend to be doing.  If gdb
 * can handle those things, both in core files and during live
 * debugging, that will help (at least) JRockit development.
 *
 * Let the program create a core file, then load the core file into
 * gdb.  Inside gdb, you should be able to do something like this:
 *
 * (gdb) t a a bt
 * 
 * Thread 7 (process 4352):
 * #0  0x001ba7dc in __nanosleep_nocancel () from /lib/tls/libc.so.6
 * #1  0x001ba5ff in sleep () from /lib/tls/libc.so.6
 * #2  0x080488a2 in makeSyscall (ignored=0x0) at threadcrash.c:118
 * #3  0x006aadec in start_thread () from /lib/tls/libpthread.so.0
 * #4  0x001ed19a in clone () from /lib/tls/libc.so.6
 * 
 * Thread 6 (process 4353):
 * #0  0x001ba7dc in __nanosleep_nocancel () from /lib/tls/libc.so.6
 * #1  0x001ba5ff in sleep () from /lib/tls/libc.so.6
 * #2  0x0804898f in syscallingSighandler (signo=10, info=0xb6be76f0, context=0xb6be7770)
 *     at threadcrash.c:168
 * #3  <signal handler called>
 * #4  0x006adf5e in pthread_kill () from /lib/tls/libpthread.so.0
 * #5  0x08048a51 in makeSyscallFromSighandler (ignored=0x0) at threadcrash.c:204
 * #6  0x006aadec in start_thread () from /lib/tls/libpthread.so.0
 * #7  0x001ed19a in clone () from /lib/tls/libc.so.6
 * 
 * Thread 5 (process 4354):
 * #0  0x001ba7dc in __nanosleep_nocancel () from /lib/tls/libc.so.6
 * #1  0x001ba5ff in sleep () from /lib/tls/libc.so.6
 * #2  0x08048936 in syscallingAltSighandler (signo=3, info=0x959cd70, context=0x959cdf0)
 *     at threadcrash.c:144
 * #3  <signal handler called>
 * #4  0x006adf5e in pthread_kill () from /lib/tls/libpthread.so.0
 * #5  0x080489e2 in makeSyscallFromAltSighandler (ignored=0x0) at threadcrash.c:190
 * #6  0x006aadec in start_thread () from /lib/tls/libpthread.so.0
 * #7  0x001ed19a in clone () from /lib/tls/libc.so.6
 * 
 * Thread 4 (process 4355):
 * #0  spin (ignored=0x0) at threadcrash.c:242
 * #1  0x006aadec in start_thread () from /lib/tls/libpthread.so.0
 * #2  0x001ed19a in clone () from /lib/tls/libc.so.6
 * 
 * Thread 3 (process 4356):
 * #0  spinningSighandler (signo=12, info=0xb4de46f0, context=0xb4de4770) at threadcrash.c:180
 * #1  <signal handler called>
 * #2  0x006adf5e in pthread_kill () from /lib/tls/libpthread.so.0
 * #3  0x08048b2f in spinFromSighandler (ignored=0x0) at threadcrash.c:232
 * #4  0x006aadec in start_thread () from /lib/tls/libpthread.so.0
 * #5  0x001ed19a in clone () from /lib/tls/libc.so.6
 * 
 * Thread 2 (process 4357):
 * #0  spinningAltSighandler (signo=14, info=0x959ee50, context=0x959eed0) at threadcrash.c:156
 * #1  <signal handler called>
 * #2  0x006adf5e in pthread_kill () from /lib/tls/libpthread.so.0
 * #3  0x08048ac0 in spinFromAltSighandler (ignored=0x0) at threadcrash.c:218
 * #4  0x006aadec in start_thread () from /lib/tls/libpthread.so.0
 * #5  0x001ed19a in clone () from /lib/tls/libc.so.6
 * 
 * Thread 1 (process 4351):
 * #0  0x08048cf3 in main (argc=1, argv=0xbfff9d74) at threadcrash.c:273
 * (gdb)
 */

#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIGSYSCALL_ALT SIGQUIT
#define SIGSYSCALL SIGUSR1
#define SIGSPIN_ALT SIGALRM
#define SIGSPIN SIGUSR2

typedef void (*sigaction_t)(int, siginfo_t *, void *);

static void installHandler(int signo, sigaction_t handler, int onAltstack) {
   struct sigaction action;
   sigset_t sigset;
   int result;
   stack_t altstack;
   stack_t oldaltstack;
   
   memset(&action, 0, sizeof(action));
   memset(&altstack, 0, sizeof(altstack));
   memset(&oldaltstack, 0, sizeof(oldaltstack));
   
   if (onAltstack) {
      altstack.ss_sp = malloc(SIGSTKSZ);
      assert(altstack.ss_sp != NULL);
      altstack.ss_size = SIGSTKSZ;
      altstack.ss_flags = 0;
      result = sigaltstack(&altstack, &oldaltstack);
      assert(result == 0);
      assert(oldaltstack.ss_flags == SS_DISABLE);
   }
   
   sigemptyset(&sigset);
   
   action.sa_handler = NULL;
   action.sa_sigaction = handler;
   action.sa_mask = sigset;
   action.sa_flags = SA_SIGINFO;
   if (onAltstack) {
      action.sa_flags |= SA_ONSTACK;
   }
   
   result = sigaction(signo, &action, NULL);
   assert(result == 0);
}

static void installNormalHandler(int signo, sigaction_t handler) {
   installHandler(signo, handler, 0);
}

static void installAlthandler(int signo, sigaction_t handler) {
   installHandler(signo, handler, 1);
}

static void *makeSyscall(void *ignored) {
   (void)ignored;

   sleep(42);

   fprintf(stderr, "%s: returning\n", __FUNCTION__);
   return NULL;
}

/* Return true if we're currently executing on the altstack */
static int onAltstack(void) {
   stack_t stack;
   int result;
   
   result = sigaltstack(NULL, &stack);
   assert(result == 0);
   
   return stack.ss_flags & SS_ONSTACK;
}

static void syscallingAltSighandler(int signo, siginfo_t *info, void *context) {
   (void)signo;
   (void)info;
   (void)context;
   
   if (!onAltstack()) {
      printf("%s() not running on altstack!\n", __FUNCTION__);
   }
   
   sleep(42);
}

static void spinningAltSighandler(int signo, siginfo_t *info, void *context) {
   (void)signo;
   (void)info;
   (void)context;
   
   if (!onAltstack()) {
      printf("%s() not running on altstack!\n", __FUNCTION__);
   }
   
   while (1);
}

static void syscallingSighandler(int signo, siginfo_t *info, void *context) {
   (void)signo;
   (void)info;
   (void)context;
   
   if (onAltstack()) {
      printf("%s() running on altstack!\n", __FUNCTION__);
   }
   
   sleep(42);
}

static void spinningSighandler(int signo, siginfo_t *info, void *context) {
   (void)signo;
   (void)info;
   (void)context;
   
   if (onAltstack()) {
      printf("%s() running on altstack!\n", __FUNCTION__);
   }
   
   while (1);
}

static void *makeSyscallFromAltSighandler(void *ignored) {
   (void)ignored;
   
   int result;
   
   installAlthandler(SIGSYSCALL_ALT, syscallingAltSighandler);
   
   result = pthread_kill(pthread_self(), SIGSYSCALL_ALT);
   assert(result == 0);
   
   fprintf(stderr, "%s: returning\n", __FUNCTION__);
   return NULL;
}

static void *makeSyscallFromSighandler(void *ignored) {
   (void)ignored;
   
   int result;
   
   installNormalHandler(SIGSYSCALL, syscallingSighandler);
   
   result = pthread_kill(pthread_self(), SIGSYSCALL);
   assert(result == 0);
   
   fprintf(stderr, "%s: returning\n", __FUNCTION__);
   return NULL;
}

static void *spinFromAltSighandler(void *ignored) {
   (void)ignored;
   
   int result;
   
   installAlthandler(SIGSPIN_ALT, spinningAltSighandler);
   
   result = pthread_kill(pthread_self(), SIGSPIN_ALT);
   assert(result == 0);
   
   fprintf(stderr, "%s: returning\n", __FUNCTION__);
   return NULL;
}

static void *spinFromSighandler(void *ignored) {
   (void)ignored;
   
   int result;
   
   installNormalHandler(SIGSPIN, spinningSighandler);
   
   result = pthread_kill(pthread_self(), SIGSPIN);
   assert(result == 0);
   
   fprintf(stderr, "%s: returning\n", __FUNCTION__);
   return NULL;
}

static void *spin(void *ignored) {
   (void)ignored;
   
   while (1);

   fprintf(stderr, "%s: returning\n", __FUNCTION__);
   return NULL;
}

int main(int argc, char *argv[]) {
   int result;
   pthread_t thread;
   volatile int bad;
   
   result = pthread_create(&thread, NULL, makeSyscall, NULL);
   assert(result == 0);
   result = pthread_create(&thread, NULL, makeSyscallFromSighandler, NULL);
   assert(result == 0);
   result = pthread_create(&thread, NULL, makeSyscallFromAltSighandler, NULL);
   assert(result == 0);
   result = pthread_create(&thread, NULL, spin, NULL);
   assert(result == 0);
   result = pthread_create(&thread, NULL, spinFromSighandler, NULL);
   assert(result == 0);
   result = pthread_create(&thread, NULL, spinFromAltSighandler, NULL);
   assert(result == 0);
   
   // Give threads some time to get going
   sleep(3);
   
   // Crash
   bad = *(int*)7;

   /* Workaround: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=29628
      Simulate use to ensure `DW_AT_location' for them:
      readelf -a --debug threadcrash|grep -A5 -w argc
      --> DW_AT_location    : 2 byte block: 71 0     (DW_OP_breg1: 0)
      This case verified on: gcc-4.1.1-30.i386
      Keep it late to ensure persistency in the registers.  */
   bad = (int) argc;
   bad = (unsigned long) argv;
   
   return 0;
}
