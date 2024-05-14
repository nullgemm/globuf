FROM alpine:edge

COPY ./scripts /scripts

RUN /scripts/install.sh

CMD /scripts/build_wayland.sh development wayland software none native
