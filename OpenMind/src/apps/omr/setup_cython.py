from setuptools import setup
from Cython.Build import cythonize
setup(ext_modules=cythonize(['_fast_match.pyx', '_fast_omr_eval.pyx'], compiler_directives={'boundscheck': False, 'wraparound': False, 'cdivision': True, 'language_level': 3}))
