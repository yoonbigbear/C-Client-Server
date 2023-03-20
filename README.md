# C++ Asio Tcp Server w/ Visual Client
 project is an experimental
## Intro
This project is a server engine(BB engine) under development for the purpose of creating a TCP MMO server using ECS(Entity Component System).
Traditional object-oriented programming tends to have a complex structure as the project grows.
ECS is a data-based programming that improves performance, but it uses components to develop and maintain more flexible content.

Commercial client engines have more features than you need. Once you develop your own functions according to the specifications you need, you can focus more on server development in the future.

TCP MMO 서버를 제작하고 있습니다. ECS 구조를 통해 게임 컨텐츠를 유연하게 확장 및 개발할 수 있는 방향으로 현재 실험중인 프로젝트 입니다.

구조적인 관점에서 봤을때 객체지향 프로그래밍은 규모가 커질수록 확장 및 수정이 어려워져 관리가 힘들어지는 경향이 있습니다. 또한 대부분의 OOP구조들이 캐시미스가 자주 발생할 수 있습니다. 위 단점들을 개선해보고자 ECS구조를 채택하여 R&D 중입니다.

### Common used
- [ASIO 1.24](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio.html)
- [Flatbuffer](https://google.github.io/flatbuffers/)
- [entt 3.11](https://github.com/skypjack/entt)
- [box2d](https://box2d.org/)
## Visual Client
![image](https://user-images.githubusercontent.com/101116747/226402628-ff28b8e6-7617-4c06-a2ef-576a5829866c.png)

It was developed for the convenience of client-server synchronization development during server development.
We are making it possible to use the same code as the server.
Additionally, features such as adding dummy bot clients, manipulating basic clients, and checking crash boxes are also available.

추가적으로 Visual client를 동시에 직접 제작하고 있는데, 상용 게임엔진들 보다 가볍고 필요한 기능들만 사용하여 서버-클라이언트 동기화 및 성능 벤치마킹, 모니터링 등 다양한 목적으로 사용할 수 있도록 개발하고 있습니다.

### 3rdparty
- [SDL2](https://github.com/ocornut/imgui)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Opengl3](https://github.com/ocornut/imgui)
- [recast&detour](https://github.com/recastnavigation/recastnavigation)
## Server
![image](https://user-images.githubusercontent.com/101116747/224535326-f43fb51f-722d-4135-af23-2d29ab4592b8.png)

The purpose is to develop a TCPMMO server engine with good commercial services and benchmark performance.
Currently, only 2d collisions are available.The database uses nanodbc. I'm going to use it as MsSql or MySql

The game content code will be shared with the visual client. Components and systems are shared through ECS.

Broadphase is currently using dynamic abb tree using box2d. Navigation is used by importing the recast detour library into the server as it is.

### 3rdparty
- [nanodbc 2.13.0](https://github.com/nanodbc/nanodbc)
- [spdlog 1.x](https://github.com/gabime/spdlog)
