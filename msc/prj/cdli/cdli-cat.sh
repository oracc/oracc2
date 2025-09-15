#!/bin/sh
cdli export \
      --host https://cdli.earth/ \
      --entities artifacts \
      --format csv \
      --output-file cdli.csv
