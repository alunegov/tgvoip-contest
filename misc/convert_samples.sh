#/bin/sh

for entry in ../tgvoipcall-runner/samples/*
do
  echo "$entry"
  ./../tgvoipcall/cmake-build-debug/opusfile-reader/opusfile-reader-test $entry $entry.wav
done
