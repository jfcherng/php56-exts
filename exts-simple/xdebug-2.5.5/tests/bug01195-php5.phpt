--TEST--
Test for bug #1195: Segfault with code coverage and foreach (< PHP 7.0)
--SKIPIF--
<?php if (!version_compare(phpversion(), "7.0", '<')) echo "skip < PHP 7.0 needed\n"; ?>
--FILE--
<?php
include 'dump-branch-coverage.inc';

xdebug_start_code_coverage(XDEBUG_CC_UNUSED | XDEBUG_CC_DEAD_CODE | XDEBUG_CC_BRANCH_CHECK);

include 'bug01195.inc';

xdebug_stop_code_coverage(false);
$c = xdebug_get_code_coverage();
dump_branch_coverage($c);
?>
--EXPECTF--
foo
foo
foo
the end
fe
- branches
  - 00; OP: 00-03; line: 02-04 HIT; out1: 04 HIT; out2: 10  X 
  - 04; OP: 04-04; line: 04-04 HIT; out1: 05  X ; out2: 10 HIT
  - 05; OP: 05-09; line: 04-07  X ; out1: 04  X 
  - 10; OP: 10-14; line: 07-09 HIT; out1: EX  X 
- paths
  - 0 4 5 4 10:  X 
  - 0 4 10:  X 
  - 0 10:  X 

{main}
- branches
  - 00; OP: 00-11; line: 02-13 HIT; out1: EX  X 
- paths
  - 0: HIT
