FROM alpine:edge

COPY ./scripts /scripts

WORKDIR /scripts
RUN /scripts/install.sh

CMD /scripts/build.sh
