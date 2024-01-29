FROM ubuntu:22.04

# 初始设置
ENV TZ=Asia/Shanghai
ENV LANG C.UTF-8
WORKDIR /opt/aiagent

# 安装基础环境
#RUN echo 'deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ jammy main restricted universe multiverse\n\
#    deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ jammy-updates main restricted universe multiverse\n\
#    deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ jammy-backports main restricted universe multiverse\n\
#    deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/ jammy-security main restricted universe multiverse\n'\
#    > /etc/apt/sources.list
RUN apt-get -y update \
    && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone \
    && apt-get install -y --no-install-recommends \
    language-pack-zh-hans	\
    fonts-wqy-zenhei	\
    libctemplate-dev \
    libopengl-dev \
    libglew2.2	\
    libglfw3	\
    libnss3	\
    libatk1.0-0	\
    libatk-bridge2.0-0	\
    libcups2	\
    libxcomposite1	\
    libxdamage1	\
    xvfb	\
    libavcodec-dev \
    libavdevice-dev \
    libavfilter-dev \
    libavformat-dev \
    libavutil-dev

COPY build/project/aiagent/Release/ /opt/aiagent

CMD /bin/bash ./start.sh
