"""
Kernel Module loader
"""
import ctypes


class ModuleLoader:
    """
    Kernel module loader

    Currently amd64 only.
    """

    SYS_INIT_MODULE = 175
    SYS_DELETE_MODULE = 176

    def __init__(self):
        libc = ctypes.CDLL(None, use_errno=True)

        self._init_module = libc.syscall
        self._init_module.restype = ctypes.c_int
        self._init_module.argtypes = \
            ctypes.c_long, \
            ctypes.c_void_p, \
            ctypes.c_ulong, \
            ctypes.c_char_p

        self._delete_module = libc.syscall
        self._delete_module.restype = ctypes.c_int
        self._delete_module.argtypes = \
            ctypes.c_long, \
            ctypes.c_char_p, \
            ctypes.c_uint

    def load(self, module, param_values=""):
        res = self._init_module(
            self.SYS_INIT_MODULE,
            module,
            len(module),
            param_values.encode('ascii')
        )
        errno = 0 if res == 0 else ctypes.get_errno()
        return (res, errno)

    def unload(self, module, flags=0):
        res = self._delete_module(
            self.SYS_DELETE_MODULE,
            module.encode('ascii'),
            flags
        )
        errno = 0 if res == 0 else ctypes.get_errno()
        return (res, errno)
