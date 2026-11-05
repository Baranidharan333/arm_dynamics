from setuptools import find_packages
from setuptools import setup

setup(
    name='hw_interface',
    version='0.0.0',
    packages=find_packages(
        include=('hw_interface', 'hw_interface.*')),
)
