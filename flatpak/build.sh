#!/bin/bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

flatpak-builder --repo=repo_dir --arch=x86_64 --install-deps-from=flathub --ccache --force-clean build_dir_x86_64 com.jpelczar.img_from_clip.json
flatpak-builder --repo=repo_dir_aarch64 --arch=aarch64 --install-deps-from=flathub --ccache --force-clean build_dir_aarch64 com.jpelczar.img_from_clip.json

flatpak build-bundle repo_dir ImgFromClip-x86_64.flatpak com.jpelczar.img_from_clip
flatpak build-bundle repo_dir_aarch64 ImgFromClip-aarch64.flatpak com.jpelczar.img_from_clip
