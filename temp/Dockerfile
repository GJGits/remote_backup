# BUILD A NEW IMAGE TO PUSH ON DOCKER HUB
FROM gjcode/gjcode-pds:1.0.7
LABEL author="GJCode"
RUN apt-get -y --purge remove libboost-all-dev libboost-doc libboost-dev \
    && rm -f /usr/lib/libboost_* \
    && apt-get -y install build-essential g++ python-dev autotools-dev libicu-dev libbz2-dev \
    && apt-get install wget -y \
    && wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz \
    && tar -zxvf boost_1_70_0.tar.gz \
    && cd boost_1_70_0/ \
    && cpuCores=`cat /proc/cpuinfo | grep "cpu cores" | uniq | awk '{print $NF}'` \
    && ./bootstrap.sh \
    && ./b2 --with=all -j $cpuCores install



