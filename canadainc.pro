TEMPLATE = lib
TARGET = canadainc

CONFIG += qt warn_on debug_and_release cascades staticlib

INCLUDEPATH += $$quote($$BASEDIR/src)
SOURCES += ../src/*.cpp
HEADERS += ../src/*.hpp ../src/*.h

lupdate_inclusion {
    SOURCES = ../assets/*.qml
}

device {
	CONFIG(release, debug|release) {
		DESTDIR = o.le-v7
	}
	CONFIG(debug, debug|release) {
		DESTDIR = o.le-v7-g
	}
}

simulator {
	CONFIG(release, debug|release) {
		DESTDIR = o
	}
	CONFIG(debug, debug|release) {
		DESTDIR = o-g
	}
}

OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui

suredelete.target = sureclean
suredelete.commands = $(DEL_FILE) $${MOC_DIR}/*; $(DEL_FILE) $${RCC_DIR}/*; $(DEL_FILE) $${UI_DIR}/*
suredelete.depends = distclean

QMAKE_EXTRA_TARGETS += suredelete

TRANSLATIONS = $$quote($${TARGET}_ar.ts) \
	$$quote($${TARGET}_de.ts) \
	$$quote($${TARGET}_es.ts) \
    $$quote($${TARGET}_fr.ts) \
    $$quote($${TARGET}_id.ts) \
    $$quote($${TARGET}_hi.ts) \
    $$quote($${TARGET}_ru.ts) \
    $$quote($${TARGET}_th.ts) \
    $$quote($${TARGET}_tr.ts) \
    $$quote($${TARGET}.ts)