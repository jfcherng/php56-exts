#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
THREAD_CNT=$(getconf _NPROCESSORS_ONLN)


#----------------#
# configurations #
#----------------#

php_install_dir="/usr/local/php56"


#----------------------#
# environment checking #
#----------------------#

PHPIZE="${php_install_dir}/bin/phpize"
PHP_CONFIG="${php_install_dir}/bin/php-config"

test -f "${PHPIZE}"     || (echo "${PHPIZE} not found"     && exit)
test -f "${PHP_CONFIG}" || (echo "${PHP_CONFIG} not found" && exit)


#-------#
# begin #
#-------#

pushd "${SCRIPT_DIR}" || exit

for ext_dir in */; do
    pushd "${ext_dir}" || exit

    ${PHPIZE}
    ./configure --with-php-config="${PHP_CONFIG}"
    make -j"${THREAD_CNT}" && make install
    make clean

    popd || exit
done

popd || exit
