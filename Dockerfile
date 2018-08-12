#----------------------------
# EOSBAY-DEV-Base
# Digital Scarcity base image of EOSBAY-DEV
#----------------------------
# VERSION 1.1.0
# AUTHOR: Merouane Benthameur
# DESCRIPTION: base image of EOSBAY-DEV

FROM eosio/eos-dev
MAINTAINER Raphael Gaudreault <valnnan@gmail.com>
LABEL authors="valnnan@gmail.com"

COPY ./contracts-entrypoint.sh /

RUN chmod +x ./contracts-entrypoint.sh
