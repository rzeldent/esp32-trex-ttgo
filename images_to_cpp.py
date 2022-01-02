# pip install Pillow

from functools import reduce
import os
import re
import sys
from PIL import Image

def add_to_file_group(file_name, groups):
    group_name = re.match(r'^(\w+)(#\d+)?\.', file_name).groups()[0]
    if (group_name not in groups):
        groups[group_name] = []
    groups[group_name].append(file_name)
    return groups

def main():
    if (len(sys.argv) < 3):
        print('Usage: images_to_cpp.py <input_dir> <file.h>')
        sys.exit(1)

    input_dir = sys.argv[1]
    file_h = sys.argv[2]

    files = os.listdir(input_dir)
    # Remove all the hidden files and directories
    files = filter(lambda x: x[0] != '.' and os.path.isfile(os.path.join(input_dir, x)), files)

    file_groups = reduce(lambda acc, v: add_to_file_group(v, acc), files, {})
    for group_files in file_groups.values():
        group_files.sort()

    output_file = open(file_h, 'w')
    output_file.write('#pragma once\n')
    output_file.write('#include <stdint.h>\n')
    output_file.write('\n')

    output_file.write('//*******************************************************************************\n')
    output_file.write('// Type: R5G6B5\n')
    output_file.write('// Scan: top_to_bottom then forward\n')
    output_file.write('// ******************************************************************************\n')

    output_file.write('\n')
    output_file.write('typedef struct {\nconst unsigned short width;\nconst unsigned short height;\nconst unsigned short *data;\n} image_t;\n')
    output_file.write('\n')
    output_file.write('typedef struct {\nconst unsigned short width;\nconst unsigned short height;\nconst unsigned short frames;\nconst unsigned short **data;\n} sprite_t;\n')
    output_file.write('\n')

    for file_group, file_names in file_groups.items():
        for file_name in file_names:
            print('Processing: ' + file_name)

            sprite_index = str(file_names.index(file_name)) if len(file_names) > 1 else ''
            image_path = os.path.join(input_dir, file_name)

            image = Image.open(image_path)
            image = image.convert('RGBA')

            width, height = image.size
            pixels = list(image.getdata())

            output_file.write('\n')
            output_file.write('static const uint16_t image_data_' + file_group + sprite_index + '[' + str(height * width) + '] = {\n')

            def convert_R8G8B8_to_R5G6B5(rgb): return (
                (rgb[0] >> 3) << 11) | ((rgb[1] >> 2) << 5) | (rgb[2] >> 3)

            for y in range(0, height):
                for x in range(width):
                    value = pixels[y * width + x]
                    output_file.write('0x' + hex(convert_R8G8B8_to_R5G6B5(value))[2:].zfill(4))
                    if (y*width+x < width*height - 1):
                        output_file.write(',')
                output_file.write('\n')

            output_file.write('};\n')

            image.close()

        if (len(file_names) > 1):
            output_file.write('static const uint16_t *image_data_' + file_group + '[] = {\n')
            for i in range(len(file_names)):
                output_file.write('\timage_data_' + file_group + str(i) + ',\n')
            output_file.write('};\n')
            output_file.write('static const sprite_t image_' + file_group + ' = { ' + str(width) + ', ' + str(height) + ', ' + str(len(file_names)) + ', image_data_' + file_group + ' };\n')
        else:
            output_file.write('static const image_t image_' + file_group + ' = { ' + str(width) + ', ' + str(height) + ', image_data_' + file_group + ' };\n')

    output_file.close()

    print('Done.')

if (__name__ == '__main__'):
    main()