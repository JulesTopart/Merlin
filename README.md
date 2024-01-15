# Merlin

## Overview
Merlin is a GPU computing and rendering engine designed as a sandbox instead of CUDA or OpenCL. While still in development, Merlin offers basic 3D rendering capabilities and a compute shader pipeline. The framework is geared towards facilitating physics experiments, but it's worth noting that it's still evolving and might not yet match the full capabilities of more established tools.

## Features
- **GPU Computing**: Utilizes GPU for computations, aiming to offer an alternative to CUDA.
- **Basic 3D Rendering**: Provides fundamental 3D rendering functionalities.
- **Compute Shader Pipeline**: Includes a pipeline for compute shaders, suitable for simple to moderately complex calculations.
- **Class Abstraction**: Offers class abstraction to ease the process of setting up and managing physics experiments.

## Getting Started
Merlin is in a developmental stage and might require some familiarity with GPU computing and rendering concepts.

1. **Clone the Repository**
   ```
   git clone https://github.com/yourusername/Merlin.git
   ```

2. **Build the Engine**
   - Navigate to the Merlin directory.
   - Run `build.bat` to compile the source code.
   ```
   cd Merlin
   build.bat
   ```

3. **Open MVSC Solution**
   - Open the `.sln` file in Microsoft Visual Studio.

## Example Usage
The code sample provided gives an insight into the usage of Merlin. It involves creating a camera, loading models, shaders, and managing a simple scene with lighting. It demonstrates the engine's capabilities in handling basic 3D objects and scene management.

## Limitations and Development Status
Merlin is a work in progress and may not yet be fully optimized or bug-free. Users should be prepared for potential issues and are encouraged to contribute to its development.

## Contributing
Contributions are welcome, especially in areas of bug fixing, feature enhancement, and documentation improvement. Please submit pull requests for any contributions.

## Support and Issues
For support or to report issues, please use the GitHub issue tracker associated with the repository.

## License
Merlin is released under DO WHATEVER YOU WANT. Please review the license terms before using or modifying the software.

---

Note: As Merlin is in development, this README aims to provide a balanced view of its current state, highlighting its potential while acknowledging its ongoing development status.