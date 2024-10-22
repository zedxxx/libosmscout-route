#!/bin/bash -ex

src=.libosmscout

root=libosmscout-importer-win64
target=$root/bin

rm -rf $root

mkdir $root
mkdir $target

cp ./../${src}/libosmscout/libosmscout.dll $target
cp ./../${src}/libosmscout-import/libosmscout_import.dll $target
cp ./../${src}/Import/Import.exe $target

dlls=(libosmscout.dll libosmscout_import.dll)

for dll in ${dlls[@]}; do
  python ./dllfetch/dllfetch.py "${target}/${dll}" --dir "/mingw64/bin/" --target-dir "${target}"
done

target=$root/stylesheets
mkdir $target

cp ./../$src/stylesheets/map.ost $target
cp ./../$src/packaging/import/windows/README.txt $root/Readme.txt
cp ./../$src/packaging/import/windows/import.cmd $root/Import.cmd

cp ImportRoute.cmd $root
cp stylesheets/route.ost $target

7z a "$root".zip "$root"/* 

