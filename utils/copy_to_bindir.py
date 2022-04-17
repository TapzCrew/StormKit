#!/usr/bin/env python3

import os, sys, shutil

input_path = os.path.join(
    sys.argv[1])

output_path = os.path.join(
    sys.argv[2])

print("Copying {} to {}".format(input_path, output_path))

os.makedirs(output_path, exist_ok=True)

shutil.copy(input_path, output_path)
