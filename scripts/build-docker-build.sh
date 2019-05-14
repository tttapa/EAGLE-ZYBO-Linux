#!/usr/bin/env bash
cd "${0%/*}"/../docker/build
docker build --target build --network=host -t eagle1/build:v1 .