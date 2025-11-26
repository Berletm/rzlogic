from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os
import shutil

MODULE_NAME = 'rzlogic'

class PrebuiltExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])

class CopyPrebuiltCmd(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        output_path = self.get_ext_fullpath(ext.name)
        os.makedirs(os.path.dirname(output_path), exist_ok=True)

        build_dir = 'build'
        found_so = None
        
        if os.path.exists(build_dir):
            for file in os.listdir(build_dir):
                if file.startswith(MODULE_NAME) and file.endswith('.so'):
                    found_so = os.path.join(build_dir, file)
                    break
        
        if not found_so:
            raise FileNotFoundError(f"Couldn't find {MODULE_NAME}*.so in build directory")

        print(f"Copying {found_so} to {output_path}")
        shutil.copyfile(found_so, output_path)

setup(
    name='rzlogic',
    version='1.0',
    description='RZlogic Python binding',
    ext_modules=[PrebuiltExtension(MODULE_NAME)],
    cmdclass={'build_ext': CopyPrebuiltCmd},
    zip_safe=False,
)