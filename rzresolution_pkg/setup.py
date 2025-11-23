from setuptools import setup, find_packages

setup(
    name='rzresolution',
    version='1.0',
    packages=find_packages(),
    package_data={'rzresolution': ['rzresolution.cpython-313-x86_64-linux-gnu.so']},
    include_package_data=True,
    zip_safe=False,
)
