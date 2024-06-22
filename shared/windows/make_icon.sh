#!/bin/bash

set -e
set -x

size_list=(16 24 32 48 64 128 256)
for size in "${size_list[@]}"; do
  inkscape ../../appicon512.png --export-filename $size.png -w $size -h $size
done

images=$(printf "%s.png " "${size_list[@]}")
convert $images img_from_clip.ico

rm -f $images

