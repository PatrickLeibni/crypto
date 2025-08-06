#!/usr/bin/env python3
"""
数字水印系统安装配置
"""

from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# 直接定义依赖，不再读取requirements.txt
requirements = [
    "opencv-python>=4.5.0",
    "numpy>=1.19.0",
    "pillow>=8.0.0",
    "matplotlib>=3.3.0",
]

setup(
    name="digital-watermark",
    version="1.0.0",
    author="Digital Watermark Team",
    author_email="watermark@example.com",
    description="基于DCT变换的数字水印系统",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/example/digital-watermark",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Scientific/Engineering :: Image Processing",
        "Topic :: Security :: Cryptography",
    ],
    python_requires=">=3.7",
    install_requires=requirements,
    extras_require={
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.0",
            "black>=21.0",
            "flake8>=3.8",
        ],
    },
    entry_points={
        "console_scripts": [
            "watermark-demo=examples.demo:main",
            "watermark-test=tests.test_installation:main",
        ],
    },
    include_package_data=True,
    package_data={
        "": ["*.txt", "*.md", "*.jpg", "*.png"],
    },
) 