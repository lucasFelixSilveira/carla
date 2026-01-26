<div align="center">
  <h1>What is "Carla"?</h1>
  <p>Carla is a programming language project with a different approach than others.</p>
  <a href="#what-does-this-mean">See more about the differences and fundamentals!</a>
</div>

<br> 
<img width="200%" src="./assets/small.png">
<br>

# And what is Carla's objective?
Unlike languages that are simple to embed, such as C, Carla has a different objective. It aims to be complete, easily embedded, have EASY integration with all possible systems where the language can be embedded, and of course, direct community input within the language's code generation through [**Morgana extensors**](https://github.com/Carla-Corp/extensors).

### What does this mean?

- Carla has an extensive core, but a 100% independent code generator. This allows us to optimize Carla for all possible platforms, whether CPUs/GPUs or even MCUs.
- Carla uses [**Morgana**](https://github.com/lucasFelixSilveira/morgana) as her machine code generator backend. This means: Much more creative freedom for the language! Without being tied to the constraints of LLVM and/or MLIR.

<br>

# And the Carla CLI?

Carla have some CLI commands, like:
- `build`: Build the project
<img width="300px" align="right" src="./assets/hi-there-carla.png">

- `run`: Build and run the project
- `init`: Create the `target.toml` file, and make an template project

### Compilation flags
- `-v`: Build the project with verbose output
- `-o`: Define the target architecture
- `-m`: Define the main file

### [**Morgana**](https://github.com/lucasFelixSilveira/morgana) also is important to know
- `build`: Build the project
- `install`: Use it to install a new extensor by the git did config on `target.toml`

### Compilation flags
- `-v`: Build the project with verbose output
- `-o`: Define the target architecture
- `-O`: Define the optimization flag as true
- `-n`: Define the name of project for the Morgana Compiler
- `-m`: Define the main file

<div align="center">

#

<table>
  <tr>
    <!-- Documentation -->
    <td>Documentation</td>
    <td> 
      <a href="./docs/carla.md">
        <img src='https://img.shields.io/badge/Docs-v1-blue.svg'/>
      </a>
    </td>
  </tr>
  <tr>
    <!-- Carla protocol -->
    <td>Carla protocol</td>
    <td> 
      <a href="./LICENSE">
      <a href="./PROTOCOL.md">
        <img src='https://img.shields.io/badge/Carla_Protocol-view-purple'/>
      </a>
    </td>
  </tr>
</table>
</div>
