#!/bin/bash -ex

src=.libosmscout

cd ./../

if [ ! -d "${src}/" ] ; then
    git clone https://github.com/framstag/libosmscout "${src}"
fi

cd "${src}/"
git fetch --all --verbose
git clean -d -x --force
git reset --hard origin/master    

cmake . -G 'MSYS Makefiles' \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE=Release \
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
    -DOSMSCOUT_BUILD_WEBPAGE=OFF \
    -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF \
    -DOSMSCOUT_BUILD_TOOL_STYLEEDITOR=OFF \
    -DGPERFTOOLS_USAGE=OFF \
    -DOSMSCOUT_BUILD_TOOL_DUMPDATA=OFF \
    -DOSMSCOUT_BUILD_CLIENT_QT=OFF \
    -DOSMSCOUT_BUILD_GPX=OFF \
    -DOSMSCOUT_BUILD_TOOL_IMPORT=ON \
    -DOSMSCOUT_BUILD_IMPORT=ON \
    -DOSMSCOUT_BUILD_TOOL_PUBLICTRANSPORTMAP=OFF \
    -DOSMSCOUT_BUILD_WITH_OPENMP=OFF \
    -DOSMSCOUT_BUILD_IMPORT_TOOL_FOR_DISTRIBUTION=OFF
    
make -j2

cd ./../Importer/

root=libosmscout-importer-win64
target=$root/bin

rm -rf $root

mkdir $root
mkdir $target

cp ./../${src}/libosmscout/libosmscout.dll $target
cp ./../${src}/libosmscout-import/libosmscout_import.dll $target
cp ./../${src}/Import/Import.exe $target

dlls=(libwinpthread-1.dll libgcc_s_seh-1.dll libstdc++-6.dll libiconv-2.dll libprotobuf.dll zlib1.dll libgomp-1.dll liblzma-5.dll libxml2-2.dll)

for dll in ${dlls[@]}; do
  cp /mingw64/bin/$dll $target
done

target=$root/stylesheets
mkdir $target

cp ./../$src/stylesheets/map.ost $target
cp ./../$src/packaging/import/windows/README.txt $root/Readme.txt
cp ./../$src/packaging/import/windows/import.cmd $root/Import.cmd

cp ImportRoute.cmd $root
cp stylesheets/route.ost $target

7z a "$root".zip "$root"/* 

rm -rf $root
