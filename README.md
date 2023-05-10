# C++ Asio Tcp Server w/ Visual Client
 project is an experimental
## Intro
 ASIO TCP 소켓통신 클라이언트 / 서버

### Common used
- [ASIO 1.24](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio.html)
- [Flatbuffer](https://google.github.io/flatbuffers/)
- [entt 3.11](https://github.com/skypjack/entt)
- [box2d](https://box2d.org/)
## Visual Client
![image](https://user-images.githubusercontent.com/101116747/226402628-ff28b8e6-7617-4c06-a2ef-576a5829866c.png)

recast navimesh/Detour 라이브러리를 사용한 pathfinding.
Opengl 3 사용 내비메쉬 시각화 및 스크린 레이충돌 추가.
Entt 라이브러리를 활용한 ECS 구조 개발.
Excel 테이블 csv , C#/C++ 소스코드 자동변환.

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
