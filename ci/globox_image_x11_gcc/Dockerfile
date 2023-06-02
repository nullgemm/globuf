FROM alpine:edge

COPY ./scripts /scripts

RUN /scripts/install.sh

CMD /scripts/build_x11.sh development x11 software none native
