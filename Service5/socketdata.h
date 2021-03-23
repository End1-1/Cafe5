#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include <QByteArray>

struct DataAddress {
    int start;
    int end;
    int length;
    DataAddress() {
        start = 0;
        end = 0;
        length = 0;
    }
    DataAddress (int s, int e, int l) {
        start = s;
        end = e;
        length = l;
    }
};

enum SocketType {
    Invalid,
    RawData,
    HttpRequest
};

enum HttpRequestMethod {
    UNKNOWN_REQUEST_METHOD,
    INCOMPLETE,
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS,
    UNSUPPORTED
};

enum ContentType {
    UNKNOWN_CONTENT_TYPE,
    MultipartFromData,
    UrlEncoded
};

QByteArray getData(const QByteArray &data, const DataAddress &da);

#endif // SOCKETDATA_H
