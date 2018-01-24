#!/bin/bash

command -v pasmo >/dev/null 2>&1 || { echo >&2 "Pasmo assembler not found. Aborting."; exit 1; }

if [ ! -e _bin/oysterpac/oysterpac ] ; then
  echo "missing oysterpac, grab it from github..."
  command -v git >/dev/null 2>&1 || \
    { echo >&2 "Need git to fetch oysterpac. Please install git, or obtain oysterpac manually and copy it to _bin/oysterpac"; \
    exit 1; }
  [ ! -d _bin ] && mkdir -p _bin
  cd _bin
  ping -c1 -q github.com >/dev/null 2>&1 || { echo >&2 "Cannot reach github.com. Aborting."; cd ..; exit 1; }
  git clone https://github.com/utz82/oysterpac.git || { echo >&2 "Failed to clone oysterpac repository. Aborting."; cd ..; exit 1; }
  cd oysterpac
  g++ -O2 -s -Wall -o oysterpac oysterpac.cpp || { echo >&2 "Failed to compile oysterpac. Aborting."; cd ../..; exit 1; }
  cd ../..
fi

[ ! -d _build ] && mkdir -p _build 
[ ! -e _build/oysterpac ] && cp _bin/oysterpac/oysterpac _build/oysterpac

case $1 in
  -82) MODEL=1 ;;
  -83) MODEL=2 ;;
  -8x) MODEL=3 ;;
  -82p) MODEL=4 ;;
  -82parcus) MODEL=4 ;;
  -8xs) MODEL=5 ;;
  -8xsmall) MODEL=5 ;;
esac

FILENAMES=( none ht2.82p ht2.83p ht2.8xp ht2p.82p ht2s.8xp )

for i in {1..5} ; do
  [[ -z $MODEL || "$MODEL" = "$i" ]] && echo "building ${FILENAMES[$i]}" \
    && pasmo  --equ MODEL=$i --alocal main.asm _build/main.bin main.sym && cd _build \
    && ./oysterpac main.bin ${FILENAMES[$i]} ht2 && cd ..
done

[ -e _build/oysterpac ] && rm _build/oysterpac
[ -e _build/main.bin ] && rm _build/main.bin

for ARG in $* ; do
  if [ "$ARG" = "-docs" ] ; then
    command -v pdflatex >/dev/null 2>&1 || { echo >&2 "Need pdflatex to build docs. Aborting."; exit 1; }
    cd docs; pdflatex -halt-on-error manual && pdflatex -halt-on-error manual; cd ..
  fi
  [[ "$ARG" = "-test" && -e _scripts/test.sh ]] && source "_scripts/test.sh"
done

exit 0;
