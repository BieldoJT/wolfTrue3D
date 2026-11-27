#!/usr/bin/env bash
# Roda ./mapcheck em cada mapa de tests/maps/ e compara com a 1ª linha "// espera: ok|aviso|erro".
# Também confere que os mapas de maps/ carregam sem erro.
set -u
cd "$(dirname "$0")/.."

CATALOG=data/textures.txt
falhas=0
total=0

for mapa in tests/maps/*.txt; do
	total=$((total + 1))
	esperado=$(head -n 1 "$mapa" | sed -n 's#^// espera: *\([a-z]*\).*#\1#p')
	saida=$(./mapcheck --catalog "$CATALOG" "$mapa")
	if grep -q ': erro:' <<<"$saida"; then
		obtido=erro
	elif grep -q ': aviso:' <<<"$saida"; then
		obtido=aviso
	else
		obtido=ok
	fi
	if [ "$obtido" = "$esperado" ]; then
		echo "ok    $mapa ($obtido)"
	else
		echo "FALHA $mapa: esperado '$esperado', obtido '$obtido'"
		echo "$saida" | sed 's/^/      /'
		falhas=$((falhas + 1))
	fi
done

for mapa in maps/*.txt; do
	total=$((total + 1))
	if ./mapcheck --catalog "$CATALOG" "$mapa" >/dev/null; then
		echo "ok    $mapa (carrega)"
	else
		echo "FALHA $mapa não carrega:"
		./mapcheck --catalog "$CATALOG" "$mapa" | sed 's/^/      /'
		falhas=$((falhas + 1))
	fi
done

echo
if [ "$falhas" -ne 0 ]; then
	echo "❌ $falhas de $total casos falharam."
	exit 1
fi
echo "✅ $total casos passaram."
