import os
import sys
from loader import ModuleLoader


def main():
    ml = ModuleLoader()
    module = open(sys.argv[1], 'rb').read()
    res = ml.load(module)
    if res[0] != 0:
        print(f'DID NOT LOAD MODULE: {os.strerror(res[1])}')
        return -1

    module_name = os.path.splitext(os.path.basename(sys.argv[1]))[0]
    res = ml.unload(module_name)
    if res[0] != 0:
        print(f'DID NOT UNLOAD MODULE: {os.strerror(res[1])}')
        return -1

    return 0


if __name__ == "__main__":
    main()
