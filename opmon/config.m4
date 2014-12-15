dnl config.m4 for extension opmon

dnl ##########################################################################
dnl Initialize the extension
PHP_ARG_ENABLE(opcode-monitor, whether to enable opcode monitoring,
[  --enable-opcode-monitor     Enable opcode monitoring])

if test "$PHP_OPCODE_MONITOR" != "no"; then
  PHP_SUBST(OPCODE_MONITOR_SHARED_LIBADD)
  PHP_REQUIRE_CXX()
  PHP_NEW_EXTENSION(opmon, opcode_monitor.c interp_context.c compile_context.c ../lib/script_cfi_hashtable.c, $ext_shared)
fi
