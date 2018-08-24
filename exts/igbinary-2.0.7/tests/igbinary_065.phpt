--TEST--
Don't emit zval has unknown type 0 (IS_UNDEF)
--SKIPIF--
<?php
if (PHP_MAJOR_VERSION < 7) {
	exit('skip a separate test case tests php5');
}
?>
--FILE--
<?php

class MyClass {
    public $kept = 2;
    public $x;
    protected $y;
    private $z;
    protected $set = 2;
    private $priv = 2;
    private $omitted = 'myVal';

    public function __sleep() {
        unset($this->x);
        unset($this->y);
        unset($this->z);
        $this->set = 'setVal';
        $this->priv = null;
        $this->omitted = 'otherVal';

        return ['kept', 'x', 'y', 'z', 'set', 'priv'];
    }
}
error_reporting(E_ALL);
// TODO: emit 'Notice: igbinary_serialize(): "x" returned as member variable from __sleep() but does not exist' instead.
$serialized = igbinary_serialize(new MyClass());
echo bin2hex($serialized) . "\n";
var_export(igbinary_unserialize($serialized));
echo "\n";
?>
--EXPECTF--
Notice: igbinary_serialize(): "x" returned as member variable from __sleep() but does not exist in %s on line 25

Notice: igbinary_serialize(): "y" returned as member variable from __sleep() but does not exist in %s on line 25

Notice: igbinary_serialize(): "z" returned as member variable from __sleep() but does not exist in %s on line 25
0000000217074d79436c617373140611046b6570740602110178001104002a007900110a004d79436c617373007a001106002a00736574110673657456616c110d004d79436c617373007072697600
MyClass::__set_state(array(
   'kept' => 2,
   'x' => NULL,
   'y' => NULL,
   'z' => NULL,
   'set' => 'setVal',
   'priv' => NULL,
   'omitted' => 'myVal',
))
