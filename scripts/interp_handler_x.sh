#!/bin/bash

[ -z "$PHP_CFI" ] && echo "This script requires variable \$PHP_CFI. Exiting now." && exit

out="$PHP_CFI/opmon/interp_handler_x.h"

function gencode() {
  echo "#include \"interp_handler.h\""
  echo ""
  echo "static opcode_handler_t original_handler[2000] = { 0 };"
  echo ""
  for i in {0..1999}
  do
    echo "static int interp_handler_$i(ZEND_OPCODE_HANDLER_ARGS)"
    echo "{"
    echo "  return execute_opcode(execute_data, original_handler[$i] TSRMLS_DC);"
    echo "}"
  done

  echo "static opcode_handler_t interp_handlers[] = {"
  for i in {0..1999}
  do
    echo "  interp_handler_$i,"
  done
  echo "};"
}

gencode > $out;
