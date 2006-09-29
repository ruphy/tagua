@ECHO OFF

CD src
QMAKE -project -o kboard.pro
ECHO QT += network >>kboard.pro
ECHO CONFIG += qt debug >>kboard.pro
ECHO DEFINES += _CRT_SECURE_NO_DEPRECATE >>kboard.pro
if NOT "_%1_" == "__" (
	ECHO INCLUDEPATH += %1 >>kboard.pro)
CD ..
