#!/bin/bash

# Update system and install dependencies
echo "Updating the system and installing dependencies..."

# Install curl if not installed
if ! command -v curl &>/dev/null; then
  echo "curl not found. Installing curl..."
  sudo apt install -y curl
fi

# Check if Git is installed, if not install it
if ! command -v git &>/dev/null; then
  echo "Git not found. Installing Git..."
  curl -fsSL https://github.com/git/git/archive/refs/tags/v2.41.0.tar.gz -o git.tar.gz
  tar -xzf git.tar.gz
  cd git-*
  make prefix=/usr/local all
  sudo make prefix=/usr/local install
  cd ..
  rm -rf git-*
  rm git.tar.gz
fi

# Check if CMake is installed, if not install it
if ! command -v cmake &>/dev/null; then
  echo "CMake not found. Installing CMake..."
  curl -fsSL https://github.com/Kitware/CMake/releases/download/v3.27.6/cmake-3.27.6-linux-x86_64.sh -o cmake.sh
  sudo bash cmake.sh --prefix=/usr/local --skip-license
  rm cmake.sh
fi

# Check if MinGW is installed (for Windows toolchain), if not install it
if ! command -v gcc &>/dev/null; then
  echo "GCC not found. Installing GCC and MinGW..."
  curl -fsSL https://sourceforge.net/projects/mingw-w64/files/latest/download -o mingw-installer.exe
  chmod +x mingw-installer.exe
  ./mingw-installer.exe --install
  rm mingw-installer.exe
fi

# Check if GLFW and GLEW are installed, if not install them
echo "Installing GLFW and GLEW dependencies..."
curl -fsSL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.tar.gz -o glfw.tar.gz
curl -fsSL https://github.com/nigels-com/glew/releases/download/2.2.0/glew-2.2.0.tgz -o glew.tar.gz
tar -xzf glfw.tar.gz
tar -xzf glew.tar.gz
cd glfw-3.3.8
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..

cd glew-2.2.0
make
sudo make install
cd ..

# Clone the repository if not already done
if [ ! -d "Image-Fragmentizer" ]; then
  echo "Cloning the repository..."
  git clone https://github.com/urikdmitro/Image-Fragmentizer.git
fi

cd Image-Fragmentizer

# Create the build directory and navigate to it
mkdir -p build
cd build

# Run CMake to configure the project
echo "Configuring the project using CMake..."
cmake ..

# Build the project
echo "Building the project..."
make

# Completion message
echo "Build finished. If everything went well, the project is ready to use."
