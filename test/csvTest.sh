#!/bin/bash

APPROOT=$(cd $(dirname $0); pwd)

reference_func() {
  lineNum=0
  while read line; do
    echo "line $lineNum : $line"
    lineNum=$(expr $lineNum + 1)
  done < $1
}

test_func() {
  REFERENCE="$APPROOT/reference.txt"
  QUERY="$APPROOT/result.txt"

  BINARY="$APPROOT/../bin_regression"
  INPUT="$APPROOT/../dataset$1.csv"


  reference_func $INPUT > $REFERENCE
  $BINARY $INPUT -v | grep "^line" > $QUERY

  ##  compare query with reference  ##
  diff $REFERENCE $QUERY
  if [ $? -eq 0 ]; then
    echo "Test $1 PASS"
  else
    echo "Test $1 !!!NG!!!"
  fi
  rm $QUERY
}

for testNo in $(seq -f "%02g" 1 3); do
  test_func $testNo
done
