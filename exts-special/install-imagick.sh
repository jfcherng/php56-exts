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

pushd imagick*/ || exit
echo "[BEGIN][Imagick] ..."

yum install -y ImageMagick ImageMagick-devel ImageMagick-perl

${PHPIZE}
./configure --with-php-config="${PHP_CONFIG}"
make -j"${THREAD_CNT}" && make install

"${PHPIZE}" --clean
make clean
git checkout -- .

echo "[END][Imagick] ..."
popd || exit

git clean -dfx

popd || exit
