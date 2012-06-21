#!/bin/bash

APPROOT=$(cd $(dirname $0); pwd)

test_func() {
  REFERENCE="$APPROOT/answer_data$1.txt"
  QUERY="$APPROOT/result.txt"

  BINARY="$APPROOT/../bin_regression"
  INPUT="$APPROOT/../dataset$1.csv"

  $BINARY $INPUT > $QUERY

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
