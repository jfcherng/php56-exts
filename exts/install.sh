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
    echo "[BEGIN][${ext_dir}] ..."

    if [ ! -f "config.m4" ]; then
        echo "[SKIP][${ext_dir}] Cannot find 'config.m4'..."

        continue
    fi

    ${PHPIZE}
    ./configure --with-php-config="${PHP_CONFIG}"
    make -j"${THREAD_CNT}" && make install

    "${phpize}" --clean
    make clean
    git clean -dfx
    git checkout -- .

    echo "[END][${ext_dir}] ..."
    popd || exit
done

pushd cphalcon*/build || exit
echo "[BEGIN][Phalcon] ..."

yum install -y re2c

./install --phpize "${PHPIZE}" --php-config "${PHP_CONFIG}"

echo "[END][Phalcon] ..."
popd || exit

popd || exit
