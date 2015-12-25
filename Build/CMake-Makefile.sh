# Cleanup first.
sh ./CMake-Cleanup.sh

# Create an empty directory.
mkdir "CMake-Makefile"
cd "CMake-Makefile"

# Generate makefiles.
cmake -G "Unix Makefiles" ./..
