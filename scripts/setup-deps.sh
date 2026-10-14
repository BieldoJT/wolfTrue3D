#!/usr/bin/env bash
# Instala o que o Makefile não traz no repositório.
# Não é um virtualenv: GLM fica em third_party/glm; SDL2, OpenGL e o
# compilador são pacotes do sistema. Drivers de GPU não são instalados.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GLM_HEADER="${ROOT}/third_party/glm/glm/glm.hpp"

if [[ ! -f "${GLM_HEADER}" ]]; then
	echo "GLM vendido não está em third_party/glm (esperado: ${GLM_HEADER})." >&2
	echo "Esse header vem com o repositório. Este script não baixa o GLM." >&2
	exit 1
fi

if [[ ! -r /etc/os-release ]]; then
	echo "Não encontrei /etc/os-release." >&2
	echo "Este script cobre Debian/Ubuntu (apt), Fedora/RHEL (dnf) e Arch (pacman)." >&2
	exit 1
fi

# shellcheck disable=SC1091
. /etc/os-release
ID="${ID:-}"
ID_LIKE="${ID_LIKE:-}"

run_root() {
	if [[ "$(id -u)" -eq 0 ]]; then
		"$@"
	else
		if ! command -v sudo >/dev/null 2>&1; then
			echo "Precisa de root ou sudo para instalar o compilador, SDL2 e OpenGL." >&2
			exit 1
		fi
		sudo "$@"
	fi
}

family() {
	local id="${ID,,}"
	local like="${ID_LIKE,,}"
	case "${id}" in
		debian|ubuntu|linuxmint|pop|elementary|zorin|raspbian|kali)
			echo apt
			return
			;;
		fedora|rhel|centos|rocky|almalinux|ol|amzn)
			echo dnf
			return
			;;
		arch|manjaro|endeavouros|garuda|artix)
			echo pacman
			return
			;;
	esac
	case "${like}" in
		*debian*|*ubuntu*)
			echo apt
			;;
		*fedora*|*rhel*|*centos*)
			echo dnf
			;;
		*arch*)
			echo pacman
			;;
		*)
			echo unknown
			;;
	esac
}

install_apt() {
	if ! command -v apt-get >/dev/null 2>&1; then
		echo "Distro identificada como Debian/Ubuntu, mas apt-get não está no PATH." >&2
		exit 1
	fi
	export DEBIAN_FRONTEND=noninteractive
	run_root apt-get update
	run_root apt-get install -y build-essential libsdl2-dev libgl1-mesa-dev
}

install_dnf() {
	local installer=""
	if command -v dnf >/dev/null 2>&1; then
		installer="dnf"
	elif command -v yum >/dev/null 2>&1; then
		installer="yum"
	else
		echo "Distro identificada como Fedora/RHEL, mas dnf e yum não estão no PATH." >&2
		exit 1
	fi
	if ! run_root "${installer}" install -y gcc-c++ make SDL2-devel libglvnd-devel; then
		run_root "${installer}" install -y gcc-c++ make SDL2-devel mesa-libGL-devel
	fi
}

install_pacman() {
	if ! command -v pacman >/dev/null 2>&1; then
		echo "Distro identificada como Arch, mas pacman não está no PATH." >&2
		exit 1
	fi
	run_root pacman -Sy --needed --noconfirm base-devel sdl2 mesa
}

echo "Sem virtualenv. Pacotes de sistema: compilador C++, Make, SDL2 e OpenGL."
echo "GLM já está vendido em third_party/glm."

case "$(family)" in
	apt)
		install_apt
		;;
	dnf)
		install_dnf
		;;
	pacman)
		install_pacman
		;;
	*)
		echo "Distro não suportada: ID=${ID:-desconhecida} ID_LIKE=${ID_LIKE:-nenhuma}." >&2
		echo "Coberto: Debian/Ubuntu (apt), Fedora/RHEL (dnf ou yum), Arch (pacman)." >&2
		echo "GLM está em third_party/. Instale à mão um compilador C++, Make, SDL2 e OpenGL 3.3." >&2
		exit 1
		;;
esac

CHECK="$(mktemp)"
trap 'rm -f "${CHECK}"' EXIT
if ! c++ -std=c++14 -I"${ROOT}/third_party/glm" -x c++ - -o "${CHECK}" -lSDL2 -ldl <<'EOF'
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
int main()
{
	glm::vec3 v(1.0f);
	return SDL_WasInit(0) || (v.x == 1.0f) ? 0 : 1;
}
EOF
then
	echo "O compilador c++ não conseguiu usar SDL2 e o GLM em third_party/glm." >&2
	echo "Confira se o pacote de desenvolvimento do SDL2 instalou e se c++ acha a biblioteca padrão." >&2
	exit 1
fi

echo "Pronto. Próximo passo, na raiz do repositório: make && ./wolf"
