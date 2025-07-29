#
# This file is the ioapp recipe.
#

SUMMARY = "Simple ioapp application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://ioapp.c \
		file://Makefile \
        file://wsserver.py \
		file://ioapp_init.sh"

S = "${WORKDIR}"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

inherit update-rc.d

INITSCRIPT_NAME = "ioapp"
INITSCRIPT_PARAMS = "start 99 S ."

do_compile() {
	     oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/ioapp ${D}${bindir}/ioapp
    install -m 0755 ${S}/wsserver.py ${D}${bindir}/wsserver.py

    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/ioapp_init.sh ${D}${sysconfdir}/init.d/ioapp
}

FILES:${PN} += "${sysconfdir}/*"