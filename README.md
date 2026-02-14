# CPRE 5500 Project 1 RPC
This project set out to learn RPC using SunRPC. This project consists of a client program and a server program that communicate via multiple RPC calls that allow the user to access remote information.
## Three Methods for running code
### Docker Container
The latest build of the code has been compiled into a minimal docker image that contains all of the needed dependencies to run the two programs
```
docker pull willgalles/rpc_container:Init
docker run -it --network=host willgalles/rpc_container:Init /bin/bash
cd /home
./server &
./client localhost 
```

### Run using the included vscode dev container
```
git clone https://github.com/waterboy741/CPRE_550_Proj_1.git
open folder up in vscode
click on prompt that asks to open up folder in container


```