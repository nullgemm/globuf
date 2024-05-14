FROM alpine:edge

COPY ./scripts /scripts

WORKDIR /scripts
RUN /scripts/install.sh

CMD /scripts/build_appkit.sh development appkit software none osxcross
