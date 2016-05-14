dnl config.m4 for extension opmon

dnl ##########################################################################
dnl Initialize the extension
PHP_ARG_ENABLE(opcode-monitor, whether to enable opcode monitoring,
[  --enable-opcode-monitor     Enable opcode monitoring])

if test "$PHP_OPCODE_MONITOR" != "no"; then

  MYSQL_INCLINE=`mysql_config --cflags | sed -e "s/'//g"`
  MYSQL_LIBLINE=`mysql_config --libs | sed -e "s/'//g"`

  echo "$MYSQL_LIBLINE"

  PHP_SUBST(OPCODE_MONITOR_SHARED_LIBADD)
  PHP_REQUIRE_CXX()
  PHP_EVAL_INCLINE("-I/usr/include/apache2")
  PHP_EVAL_INCLINE("-I/usr/include/apr-1.0")
  PHP_EVAL_INCLINE($MYSQL_INCLINE)
  PHP_EVAL_LIBLINE($MYSQL_LIBLINE, OPMON_SHARED_LIBADD)
  opmon_sources="opcode_monitor.c \
                 event_handler.c \
                 interp_context.c \
                 compile_context.c \
                 operand_resolver.c \
                 dataset.c \
                 metadata_handler.c \
                 cfg.c \
                 cfg_handler.c \
		 dataflow.c \
		 taint.c \
                 lib/script_cfi_utils.c \
                 lib/script_cfi_array.c \
                 lib/script_cfi_queue.c \
                 lib/script_cfi_hashtable.c"
  PHP_NEW_EXTENSION(opmon, $opmon_sources, $ext_shared)
  PHP_SUBST(OPMON_SHARED_LIBADD)
fi
