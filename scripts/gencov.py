#! /usr/bin/python3

import os
import shutil
import subprocess
import argparse
from typing import Any
from pathlib import Path


def ParseArguments() -> Any:
    parser = argparse.ArgumentParser("Generate coverage")
    parser.add_argument("target", help="Test target.")
    return parser.parse_args()


def main():
    arguments = ParseArguments()
    target = arguments.target
    testName = arguments.target + "Test"
    libraryIncludeDirectory = Path(
        "/home/atohs/cpp/CppLearning/build/lib/CMakeFiles/CppLearning.dir/",
        target,
    )
    testIncludeDirectory = Path(
        "/home/atohs/cpp/CppLearning/build/tests/",
        testName,
        "CMakeFiles",
        f"{testName}.dir",
    )
    output = Path(
        os.path.join("/home/atohs/cpp/CppLearning/tests/", testName)
    )
    html = output.joinpath("HTML")
    coverage = output.joinpath("coverage.info")

    if html.exists():
        shutil.rmtree(html)
    os.mkdir(html)

    # lcov
    subprocess.run(
        [
            "lcov",
            "-c",
            "-d",
            libraryIncludeDirectory,
            "-d",
            testIncludeDirectory,
            "-o",
            coverage,
        ],
        check=True,
    )
    # genhtml
    subprocess.run(["genhtml", "-o", html.as_posix(), coverage.as_posix()], check=True)


if __name__ == "__main__":
    main()
