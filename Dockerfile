FROM mtd/builder:noDNN as builder
ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /builder/

RUN git clone git@github.com:MetidaAI/Core.git
RUN cd Core &&\
      cmake . -DBUILD_WITH_REDIS=ON && \
      make install -j"$(nproc)"

COPY . .
RUN cmake . && make -j4

FROM ubuntu:latest

COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /usr/local/lib /usr/local/lib

WORKDIR /app

COPY --from=builder /builder/Telemetry .
RUN ldconfig

CMD ["./Telemetry"]