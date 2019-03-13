#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
THREAD_CNT=$(getconf _NPROCESSORS_ONLN)


#-------#
# begin #
#-------#

pushd "${SCRIPT_DIR}" || exit

# keep fresh state
git clean -dfx
git checkout -- .

for script in exts-simple/*.sh; do
    bash "${script}"
done

for script in exts-special/*.sh; do
    bash "${script}"
done

# keep fresh state
git clean -dfx
git checkout -- .

popd || exit
