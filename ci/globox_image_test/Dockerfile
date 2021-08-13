FROM debian:bullseye-slim

COPY ./scripts /scripts

WORKDIR /scripts
RUN /scripts/install.sh

CMD /scripts/test_cppcheck.sh
