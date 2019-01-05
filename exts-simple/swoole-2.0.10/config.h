/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Whether to build swoole as dynamic module */
#define COMPILE_DL_SWOOLE 1

/* have accept4 */
#define HAVE_ACCEPT4 1

/* have clock_gettime */
#define HAVE_CLOCK_GETTIME 1

/* cpu affinity? */
#define HAVE_CPU_AFFINITY 1

/* have daemon */
#define HAVE_DAEMON 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* have epoll */
#define HAVE_EPOLL 1

/* have eventfd */
#define HAVE_EVENTFD 1

/* have execinfo */
#define HAVE_EXECINFO 1

/* have FUTEX? */
#define HAVE_FUTEX 1

/* have hiredis */
/* #undef HAVE_HIREDIS */

/* have inotify */
#define HAVE_INOTIFY 1

/* have inotify_init1 */
#define HAVE_INOTIFY_INIT1 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* have kqueue */
/* #undef HAVE_KQUEUE */

/* have malloc_trim */
#define HAVE_MALLOC_TRIM 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* have mkostemp */
#define HAVE_MKOSTEMP 1

/* have pthread_mutex_timedlock */
#define HAVE_MUTEX_TIMEDLOCK 1

/* have nghttp2 */
/* #undef HAVE_NGHTTP2 */

/* have openssl */
/* #undef HAVE_OPENSSL */

/* have pcre */
#define HAVE_PCRE 1

/* have poll */
#define HAVE_POLL 1

/* have pthread_barrier_init */
#define HAVE_PTHREAD_BARRIER 1

/* have SO_REUSEPORT? */
#define HAVE_REUSEPORT 1

/* have pthread_rwlock_init */
#define HAVE_RWLOCK 1

/* have sendfile */
#define HAVE_SENDFILE 1

/* have signalfd */
#define HAVE_SIGNALFD 1

/* have pthread_spin_lock */
#define HAVE_SPINLOCK 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* have timerfd */
#define HAVE_TIMERFD 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* enable ability of coroutine */
#define SW_COROUTINE 1

/* do we enable swoole debug */
/* #undef SW_DEBUG */

/* have zlib */
#define SW_HAVE_ZLIB 1

/* enable sockets support */
/* #undef SW_SOCKETS */

/* enable http2.0 support */
/* #undef SW_USE_HTTP2 */

/* enable hugepage support */
/* #undef SW_USE_HUGEPAGE */

/* use jemalloc */
/* #undef SW_USE_JEMALLOC */

/* use mysqlnd */
/* #undef SW_USE_MYSQLND */

/* enable openssl support */
/* #undef SW_USE_OPENSSL */

/* enable picohttpparser support */
/* #undef SW_USE_PICOHTTPPARSER */

/* enable async-redis support */
/* #undef SW_USE_REDIS */

/* enable thread support */
/* #undef SW_USE_THREAD */

/* enable timewheel support */
/* #undef SW_USE_TIMEWHEEL */
