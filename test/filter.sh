#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

exec $DIR/../build/filter "$DIR/../res/shader/filter/" $1