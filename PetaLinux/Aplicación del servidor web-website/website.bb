#
# This file is the website recipe.
#

SUMMARY = "Simple website application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://index.html \
           file://smoothie.js \
           file://EPN.png \
           file://conf_0.png \
           file://conf_1.png \
           file://conf_2.png \
           file://cgi-bin/inputs.sh"

FILES:${PN} += "/srv/www"

S = "${WORKDIR}"

do_install() {
    install -d ${D}/srv/www
    install -d ${D}/srv/www/cgi-bin

    install -m 0644 ${S}/index.html ${D}/srv/www
    install -m 0644 ${S}/smoothie.js ${D}/srv/www
    install -m 0644 ${S}/EPN.png ${D}/srv/www
    install -m 0644 ${S}/conf_0.png ${D}/srv/www
    install -m 0644 ${S}/conf_1.png ${D}/srv/www
    install -m 0644 ${S}/conf_2.png ${D}/srv/www
    install -m 0755 ${S}/cgi-bin/inputs.sh ${D}/srv/www/cgi-bin
}