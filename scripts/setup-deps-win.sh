#!/usr/bin/env bash
# Prepara o cross-compile para Windows (make win): instala o MinGW (apt) e
# baixa o SDL2 para MinGW em third_party/SDL2-win (ignorado pelo git).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SDL_VERSION="2.30.0"
DEST="${ROOT}/third_party/SDL2-win"
URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL2-devel-${SDL_VERSION}-mingw.tar.gz"

if ! command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1; then
	if ! command -v apt-get >/dev/null 2>&1; then
		echo "Instale à mão o MinGW-w64 (x86_64-w64-mingw32-g++). Este script cobre só apt." >&2
		exit 1
	fi
	sudo apt-get update
	sudo apt-get install -y g++-mingw-w64-x86-64
fi

if [[ -d "${DEST}/x86_64-w64-mingw32" ]]; then
	echo "SDL2 para Windows já está em ${DEST}."
	exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "${TMP}"' EXIT
curl -fL "${URL}" -o "${TMP}/sdl2.tar.gz"
tar -xzf "${TMP}/sdl2.tar.gz" -C "${TMP}"
mkdir -p "${DEST}"
cp -r "${TMP}/SDL2-${SDL_VERSION}/x86_64-w64-mingw32" "${DEST}/"
echo "Pronto. Próximo passo: make win"
