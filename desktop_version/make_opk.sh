#!/bin/sh

OPK_NAME=vvvvvv.opk

echo ${OPK_NAME}

# create default.gcw0.desktop
cat > default.gcw0.desktop <<EOF
[Desktop Entry]
Name=VVVVVV
Comment=2D puzzle platformer created by Terry Cavanagh
Exec=vvvvvv.x86
Terminal=false
Type=Application
StartupNotify=true
Icon=vvvvvv
Categories=games;
EOF

# create opk
FLIST="build/vvvvvv.x86"
FLIST="${FLIST} default.gcw0.desktop"
FLIST="${FLIST} vvvvvv.png"

rm -f ${OPK_NAME}
mksquashfs ${FLIST} ${OPK_NAME} -all-root -no-xattrs -noappend -no-exports

cat default.gcw0.desktop
rm -f default.gcw0.desktop
