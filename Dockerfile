FROM alpine as build-env

RUN apk --no-cache add build-base cmake cjson-dev openssl-dev sqlite-dev curl-dev

WORKDIR /app
COPY . .

WORKDIR /app/test
RUN mkdir -p build
WORKDIR /app/test/build
RUN cmake ..
RUN make

CMD ["./opensync-test"]
