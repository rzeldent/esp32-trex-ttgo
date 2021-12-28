# pip install Pillow

import os
import sys
from PIL import Image

if (len(sys.argv) < 3):
    print('Usage: images_to_cpp.py <input_dir> <file.h>')
    sys.exit(1)

input_dir = sys.argv[1]
file_h = sys.argv[2]

converted = []

file_names = os.listdir(input_dir)
file_names = filter(lambda x: x[0] != '.' and os.path.isfile(os.path.join(input_dir, x)), file_names)

output_file = open(file_h, 'w')
output_file.write('#pragma once\n')
output_file.write('#include <stdint.h>\n')
output_file.write('\n')

output_file.write('//*******************************************************************************\n')
output_file.write('// Type: R5G6B5\n')
output_file.write('// Scan: top_to_bottom then forward\n')
output_file.write('// ******************************************************************************\n')

output_file.write('\n')
output_file.write("""
typedef struct
{
    const unsigned short width;
    const unsigned short height;
    const uint16_t *data;
} image_t;
""")
output_file.write('\n')

for file_name in file_names:
    print('Processing: ' + file_name)

    image_path = os.path.join(input_dir, file_name)
    base_name = os.path.splitext(file_name)[0]

    image_data_name = 'image_data_' + base_name
    image = Image.open(image_path)
    image = image.convert('RGBA')

    width, height = image.size
    pixels = list(image.getdata())

    output_file.write('\n')
    output_file.write('static const uint16_t ' + image_data_name + '[' + str(height * width) + '] = {\n')

    convert_R8G8B8_to_R5G6B5 = lambda rgb: (rgb[0] >> 3) << 11 | (rgb[1] >> 2) << 5 | (rgb[2] >> 3)

    for y in range(0, height):
        for x in range(width):
            value = pixels[y * width + x]
            output_file.write('0x' + hex(convert_R8G8B8_to_R5G6B5(value))[2:].zfill(4))
            if (y*width+x < width*height - 1):
                output_file.write(',')
        output_file.write('\n')

    output_file.write('};\n')
    output_file.write('const image_t ' + base_name + ' = { ' + str(width) + ', ' + str(height) + ', ' + image_data_name + ' };\n')

    image.close()

    converted.append(base_name)

output_file.close()

print('Done.')