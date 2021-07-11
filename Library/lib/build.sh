#!/usr/bin/bash -ex

src=".libosmscout"

cd "./../../"

if [ ! -d "${src}/" ]; then
    git clone https://github.com/framstag/libosmscout "${src}"
fi

cd "${src}/"
git fetch --all --verbose
git clean -d -x --force
git reset --hard origin/master

cmake . -G "MSYS Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DOSMSCOUT_BUILD_MAP=OFF \
    -DOSMSCOUT_BUILD_MAP_AGG=OFF \
    -DOSMSCOUT_BUILD_MAP_CAIRO=OFF \
    -DOSMSCOUT_BUILD_MAP_OPENGL=OFF \
    -DOSMSCOUT_BUILD_MAP_DIRECTX=OFF \
    -DOSMSCOUT_BUILD_MAP_GDI=OFF \
    -DOSMSCOUT_BUILD_MAP_QT=OFF \
    -DOSMSCOUT_BUILD_MAP_SVG=OFF \
    -DOSMSCOUT_BUILD_MAP_IOSX=OFF \
    -DOSMSCOUT_BUILD_TESTS=OFF \
    -DOSMSCOUT_BUILD_DEMOS=OFF \
    -DOSMSCOUT_BUILD_BINDING_JAVA=OFF \
    -DOSMSCOUT_BUILD_BINDING_CSHARP=OFF \
    -DOSMSCOUT_BUILD_DOC_API=OFF \
    -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF \
    -DOSMSCOUT_BUILD_TOOL_STYLEEDITOR=OFF \
    -DGPERFTOOLS_USAGE=OFF \
    -DOSMSCOUT_BUILD_TOOL_DUMPDATA=OFF \
    -DOSMSCOUT_BUILD_CLIENT_QT=OFF \
    -DOSMSCOUT_BUILD_GPX=OFF \
    -DOSMSCOUT_BUILD_TOOL_IMPORT=OFF \
    -DOSMSCOUT_BUILD_IMPORT=OFF \
    -DOSMSCOUT_BUILD_TOOL_PUBLICTRANSPORTMAP=OFF \
    -DOSMSCOUT_BUILD_WITH_OPENMP=OFF \
    -DOSMSCOUT_BUILD_IMPORT_TOOL_FOR_DISTRIBUTION=OFF

make -j2

cp -v ./libosmscout/libosmscout.a ./../Library/lib/libosmscout.a

cp -r -v ./libosmscout/include ./../Library/
