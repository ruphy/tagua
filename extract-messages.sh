#!/bin/sh
set -e

WDIR=`pwd`
BASEDIR=$1
[ $BASEDIR ] || BASEDIR=$WDIR   # root of translatable sources
SRCDIR=$BASEDIR/src
PROJECT="tagua"  # project name
BUGADDR="http://trac.tagua-project.org"
 
echo "Preparing rc files"
cd ${SRCDIR}
# we use simple sorting to make sure the lines do not jump around too much from system to system
find . -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/extractedrc.cpp
# additional string for KAboutData
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/extractedrc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/extractedrc.cpp
cd ${WDIR}
echo "Done preparing rc files"
 
 
echo "Extracting messages"
cd ${SRCDIR}
# see above on sorting
find . -name '*.cpp' -o -name '*.h' -o -name '*.c' | sort > ${WDIR}/infiles.list
echo "extractedrc.cpp" >> ${WDIR}/infiles.list
cd ${WDIR}
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
    -kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
   --msgid-bugs-address="${BUGADDR}" \
     --files-from=infiles.list -D ${SRCDIR} -D ${WDIR} -o ${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }
echo "Done extracting messages"
 
 
echo "Merging translations"
catalogs=`find translations -name '*.po'`
for cat in $catalogs; do
  echo $cat
  msgmerge -o $cat.new $cat ${PROJECT}.pot
  mv $cat.new $cat
done
echo "Done merging translations"
 
 
echo "Cleaning up"
cd ${WDIR}
rm rcfiles.list
rm infiles.list
rm extractedrc.cpp
echo "Done"
