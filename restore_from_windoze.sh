#!/bin/bash

find -type f -exec chmod 644 {} ";"
find -type d -exec chmod 755 {} ";"

svn status | egrep '^!.*([[:space:]]|/)[A-Z]+[[:space:]]*$' \
	| while read A B; do b="`echo $B | tr "A-Z" "a-z"`"; mv $b $B; done

rm -rf src/release src/debug src/kboard.suo src/vc80.* \
   src/kboard.ncb src/kboard.sln src/kboard.vcproj*
