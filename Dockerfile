FROM resin/rpi-raspbian:latest

RUN apt-get update -qq && apt-get install -y qtbase5-dev python3 ruby ruby-dev build-essential pkg-config
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1
RUN gem install bundler --no-ri --no-rdoc

ADD . /stockmonster

WORKDIR /stockmonster

RUN ./waf configure
RUN ./waf
RUN ./waf install

CMD bash
