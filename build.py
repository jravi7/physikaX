# one build script to rule them all
"""
build.py
Run this script to config and build the solution using cmake.
For e.g., python build.py
Command line args:
    --clean: removes/cleans build folder
    --build: configs and runs the build. This is the default behaviour
"""
import subprocess
import argparse
import shutil


def setup_argparser():
    """
    Setup Argparser
    Sets up argument parsing routines for the script
    @return: returns an argparser arg object
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-c", "--clean", help="Clean build.", action="store_true")
    parser.add_argument("-b", "--build", help="Build project.", action="store_true",
                        default="store_true")
    return parser.parse_args()


def main():
    """
    Main entry point for this script
    """
    args = setup_argparser()
    if args.clean:
        shutil.rmtree("./build/")
    elif args.build:
        config_cmd = ["cmake.exe", ".", "-B./build",
                      "-GVisual Studio 16 2019", "-Ax64"]
        subprocess.call(config_cmd)
        build_cmd = ["cmake.exe", "--build", "build"]
        subprocess.call(build_cmd)


if __name__ == "__main__":
    main()
