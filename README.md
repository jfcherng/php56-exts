# Build

In each extension's source directory, execute following commands.

```bash
php_install_dir=/usr/local/php56
proc_num=$(nproc)

"${php_install_dir}/bin/phpize"
./configure --with-php-config="${php_install_dir}/bin/php-config"
make -j"${proc_num}" && make install && make clean
```
