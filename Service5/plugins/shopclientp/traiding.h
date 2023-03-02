#ifndef TRAIDING_H
#define TRAIDING_H

#include "rawmessage.h"
#include "database.h"

void create_empty_draft(RawMessage &rm, Database &db, const QByteArray &in, int user);
void get_drafts_headers(RawMessage &rm, Database &db, const QByteArray &in, int user);
void add_goods_to_draft(RawMessage &rm, Database &db, const QByteArray &in, int user);
void open_draft_header(RawMessage &rm, Database &db, const QByteArray &in);
void open_draft_body(RawMessage &rm, Database &db, const QByteArray &in);

#endif // TRAIDING_H
