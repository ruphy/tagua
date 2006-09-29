#!/bin/bash
lang=$1

if [ $lang ]
then
	echo "TRANSLATIONS += kboard_$lang.ts" >> src/kboard-l.pro
	cd src
	lupdate kboard-l.pro
	cd ..
	make lang_prep
	cd src
	echo "run"
	echo "      linguist kboard_$lang.ts"
	echo "to begin translation"
fi
