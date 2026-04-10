#!/bin/sh

OPK_NAME=vvvvvv_gkd350h.opk

echo ${OPK_NAME}

# create default.gcw0.desktop
cat > default.gcw0.desktop <<EOF
[Desktop Entry]
Name=VVVVVV
Comment=2D puzzle platformer created by Terry Cavanagh
Exec=vvvvvv
Icon=vvvvvv
Categories=games
EOF

# create opk
FLIST="build/vvvvvv"
FLIST="${FLIST} default.gcw0.desktop"
FLIST="${FLIST} vvvvvv.png"

rm -f ${OPK_NAME}
/opt/gcw0-toolchain-gcc11.1.0/bin/mipsel-linux-strip build/vvvvvv
mksquashfs ${FLIST} ${OPK_NAME} -all-root -no-xattrs -noappend -no-exports

cat default.gcw0.desktop
rm -f default.gcw0.desktop
