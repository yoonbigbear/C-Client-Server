# c++ Asio Tcp Server w/ Visual Client
 project is an experimental
## Intro
This project is a server engine(BB engine) under development for the purpose of creating a TCP MMO server using ECS(Entity Component System).
Traditional object-oriented programming tends to have a complex structure as the project grows.
ECS is a data-based programming that improves performance, but it uses components to develop and maintain more flexible content.

Commercial client engines have more features than you need. Once you develop your own functions according to the specifications you need, you can focus more on server development in the future.

### Common used
- [ASIO 1.24](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio.html)
- [Flatbuffer](https://google.github.io/flatbuffers/)
- [entt 3.11](https://github.com/skypjack/entt)
- [box2d](https://box2d.org/)
## Visual Client
![image](https://user-images.githubusercontent.com/101116747/224535294-11481ca8-87a1-4240-adba-2866c6ec096b.png)

It was developed for the convenience of client-server synchronization development during server development.
We are making it possible to use the same code as the server.
Additionally, features such as adding dummy bot clients, manipulating basic clients, and checking crash boxes are also available.
### 3rdparty
- [SDL2](https://github.com/ocornut/imgui)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Opengl3](https://github.com/ocornut/imgui)
## Server
![image](https://user-images.githubusercontent.com/101116747/224535326-f43fb51f-722d-4135-af23-2d29ab4592b8.png)

The purpose is to develop a TCPMMO server engine with good commercial services and benchmark performance.
Currently, only 2d collisions are available.The database uses nanodbc. I'm going to use it as MsSql or MySql

The game content code will be shared with the visual client. Components and systems are shared through ECS.

Broadphase is currently using dynamic abb tree using box2d. Navigation is used by importing the recast detour library into the server as it is.

### 3rdparty
- [nanodbc 2.13.0](https://github.com/nanodbc/nanodbc)
- [spdlog 1.x](https://github.com/gabime/spdlog)
