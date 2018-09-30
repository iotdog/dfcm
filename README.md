# How to use

## modify file name in main.cpp

```
// 加载数据
std::vector<double> data = {};
load("原始0.txt", &data); // 修改文件名
std::cout << "data size: " << data.size() << std::endl;
```

## run code on linux host
```
git clone https://github.com/iotdog/dfcm.git
cd dfcm
make
./dfcm
```

## run code in docker

install [Docker](https://www.docker.com/) first.

```
git clone https://github.com/iotdog/dfcm.git
cd dfcm
# for linux
docker run -v $(pwd):/root/dfcm -i -t --rm iotdog/motionexp-docker /bin/bash
# for windows
docker run -v %cd%:/root/dfcm -i -t --rm iotdog/motionexp-docker /bin/bash
```

after entering linux bash shell, then type:

```
cd /root/dfcm
make
./dfcm
```