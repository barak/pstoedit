#!/bin/sh
echo running aclocal
if (aclocal-1.4 --version)  < /dev/null > /dev/null 2>&1; then
    aclocal-1.4 ${ACLOCAL_FLAGS}
else
    aclocal ${ACLOCAL_FLAGS}
fi

echo running automake
if (automake-1.4 --version) < /dev/null > /dev/null 2>&1; then
    automake-1.4 -a
else
    automake -a
fi

echo running autoconf
autoconf
# CONFIG_DIR=config
# echo aclocal -I /usr/local/share/aclocal/ --output=${CONFIG_DIR}/aclocal.m4
# aclocal -I /usr/local/share/aclocal --output=${CONFIG_DIR}/aclocal.m4

# echo automake -a
# automake -a --include-deps

# echo autoconf  --localdir=${CONFIG_DIR}
# autoconf  --localdir=${CONFIG_DIR}

