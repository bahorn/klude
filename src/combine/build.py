import os
import base64
import zlib

from elftools.elf.elffile import ELFFile
from jinja2 import Template


def modinfo_to_dict(data):
    elf = ELFFile(data)
    res = {}
    for section in elf.iter_sections():
        if section.name == '.modinfo':
            values = section.data().split(b'\x00')
            for value in values:
                try:
                    k, v = value.split(b'=', 1)
                    res[k] = v
                except ValueError:
                    continue
            break
    return res


def process_stage1():
    stage1_path = './stage1/code/stage1.ko'
    # Strip the binary
    os.system(f'strip --strip-debug {stage1_path}')
    # Now encode the binary
    with open(stage1_path, 'rb') as stage1:
        encoded_stage1 = base64.b64encode(zlib.compress(stage1.read()))
        stage1.seek(0)
        name = modinfo_to_dict(stage1)[b'name'].decode('ascii')

    return name, encoded_stage1.decode('ascii')


def process_stage2():
    bcode_path = './stage2/stage2.bin'
    with open(bcode_path, 'rb') as bcode:
        encoded_bcode = base64.b64encode(zlib.compress(bcode.read()))
    return encoded_bcode.decode('ascii')


def build():
    stage1_name, stage1_data = process_stage1()
    bcode_data = process_stage2()

    with open('./stage0/loader.py', 'r') as f:
        loader = f.read()

    with open('./stage2/scripts/script.py', 'r') as f:
        bytecode_runner = f.read()

    with open('./combine/template.py.jinja2', 'r') as f:
        template = Template(f.read())

    res = template.render(
        module_loader_class=loader,
        module_bytecode_runner=bytecode_runner,
        stage1_name=stage1_name,
        encoded_stage1=stage1_data,
        encoded_bytecode=bcode_data
    )
    print(res)


if __name__ == "__main__":
    build()
