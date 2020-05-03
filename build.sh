#!/usr/bin/env bash

sudo docker build -t gba-dev .
sudo docker run --rm --volume $PWD:/build -it --workdir "/build" gba-dev make "$@"