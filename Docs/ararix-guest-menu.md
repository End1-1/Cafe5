# Меню и заказ с буклета на столе

Гость сканирует QR на буклете **своего стола**. Сайт показывает меню и тем же ключом отправляет заказ на этот стол. Ключ не секрет кассы и не ключ трекинга QRNeto.

Базовый адрес: `https://<хост>/engine/v2`.

## Модель: один буклет = один стол

Ключ буклета привязан к `h_tables.f_id`. Стол в теле `order/create` передавать нельзя — сервер берёт его из ключа.

```text
QR на столе N  →  guest key (f_table_id=N)  →  menu/get + order/create  →  o_header.f_table = N
```

В QR кладётся сам ключ или ссылка: `https://<сайт-меню>/?key=<token>`. В QR не кладётся адрес API и не нужен отдельный `table=` в URL.

## Ключи (staff)

Нужна сессия сотрудника, Bearer как у кассы. Заголовок версии приложения для staff-методов — как обычно у FrontDesk.

### Список залов и столов

```http
POST /ararix/menu/tables
Authorization: Bearer <сессия сотрудника>
Content-Type: application/json

{}
```

Ответ: `halls[]` с вложенными `tables[]` (`f_id`, `f_name`, `f_has_key`, `f_token` если ключ уже выпущен).

### Выпуск / перепечатка ключа стола

```http
POST /ararix/menu/issue-key
Authorization: Bearer <сессия сотрудника>
Content-Type: application/json

{ "table_id": 5 }
```

```json
{
  "status": 1,
  "partner": "guest",
  "token": "................",
  "rotated": false,
  "table": { "f_id": 5, "f_name": "5" },
  "hall": { "f_id": 1, "f_name": "Зал" }
}
```

`rotate: true` заменяет ключ — старые QR этого стола перестают работать. Без `rotate` повторный вызов возвращает тот же токен.

Код `qrneto` в настройках API keys — только трекинг заказов, к буклету не относится.

## Меню

Сайт читает ключ из QR:

- заголовок `Authorization: Bearer <token>`
- поле JSON `key`
- query `?key=<token>`

```http
POST /ararix/menu/get
Authorization: Bearer <token>
Content-Type: application/json

{ "locale": "ru" }
```

`locale`: `hy`, `ru`, `en`.

Успех: `status = 1`, плюс `locale`, `groups`, `parts`, `dishes`, `dish_total`, `limit`, `offset`, **`table`**, **`hall`** (стол этого буклета).

Без `limit` — все блюда и картинки base64 в `f_image` (киоск). Для сайта гостя так не качать.

Порциями без base64:

```http
GET /ararix/menu/get?key=<token>&locale=hy&group=24&limit=10&offset=0&with_image=0
```

При `with_image=0` приходит `f_image_url` (PNG), только если картинка реально отдаётся:

```json
"f_image_url": "/engine/v2/ararix/menu/image?dish=315&key=<token>"
```

Группа: `?group=24`. Нет картинки — пустой `f_image_url`, у `/image` HTTP 404 `NOT_FOUND`.

| Код | Когда |
| --- | --- |
| 401 `UNAUTHORIZED` | ключ не передан |
| 403 `FORBIDDEN` | ключ не тот |
| 403 `INVALID_TABLE` | ключ без стола или стол удалён |

## Как собрать экран

Экран повторяет киоск. Правила разбора `f_data`, пакетов Type/Size, модификаторов и связанных напитков — без изменений относительно киоска (см. ниже).

**Группы** (`groups`) — колонка слева. **Блюда** (`dishes`): `f_group` = `f_id` группы. Id позиции — `f_dish`.

`f_type`: `1` товар, `2` напиток, `5` пакет.

`f_stoplist = -999` — можно в корзину. Иначе — стоп-лист, не класть.

### Разбор `f_data`

`f_data` — строка JSON-объекта (`JSON.parse`). `null` / `""` / `[]` → `{}`.

- `f_cooking_time`, `f_dietary_badge`, `f_bju`, `f_dynamic_attributes` (Type/Size), `f_modificators`, `f_related_drink`, `f_related_other` — как у киоска.

Цена пакета без модификаторов: `f_price пакета + price Type + price Size`. В корзину кладётся `f_goods` выбранной строки комплектации.

## Валидация корзины

Если меню долго открыто, перед подтверждением вызовите validate. Сервер пересчитает цены, стоп-лист, type/size и модификаторы.

```http
POST /ararix/order/validate
Authorization: Bearer <token>
Content-Type: application/json
```

```json
{
  "locale": "hy",
  "dishes": [
    {
      "f_dish": 40,
      "qty": 1,
      "type": "Курица",
      "size": "Большой",
      "modifiers": [501],
      "f_price": 2500
    }
  ]
}
```

`f_price` в строке необязателен: если передан и не совпал с текущим — `ok=false`, `error.code=PRICE_CHANGED`, в `dishes` актуальные суммы.

Успех без смены цены: `ok=true`, `amount_total`, `dishes[]`.

`create` при расхождении цены отвечает HTTP 409 `PRICE_CHANGED` и заказ не создаёт.

## Заказ

```http
POST /ararix/order/create
Authorization: Bearer <token>
Content-Type: application/json
```

```json
{
  "locale": "hy",
  "idempotency_key": "8f3c2a1b-9d4e-4c7a-b012-abcdef012345",
  "dishes": [
    {
      "f_dish": 40,
      "qty": 1,
      "type": "Курица",
      "size": "Большой",
      "modifiers": [501, 502],
      "comment": "без лука"
    },
    {
      "f_dish": 88,
      "qty": 1
    }
  ]
}
```

Обязательно:

- `idempotency_key` — 8…64 символа (UUID клиента). Повтор с тем же ключом и тем же буклетом возвращает **тот же** заказ (`idempotent_replay: true`), второй раз на кухню не уходит.
- `dishes` — только блюда. Цену считает сервер.

Не передавать: `table`, `table_id`, зал, номер заказа, оплату.

`f_dish` — id карточки меню. Для пакета (`f_type=5`) — id пакета; `type`/`size` — выбранные значения. Связанный напиток — отдельный элемент `dishes`.

Успех `status=1`: заказ открыт (`f_state=1`), строки на кухню (печать service). Оплаты нет.

```json
{
  "status": 1,
  "order": {
    "f_id": "uuid",
    "f_prefix": "A00012",
    "f_state": 1,
    "f_amounttotal": 4500,
    "f_public_token": "................",
    "table": { "f_id": 5, "f_name": "5" },
    "hall": { "f_id": 1, "f_name": "Зал" },
    "dishes": [ ]
  }
}
```

### Пример полного заказа (пицца + добавки + напиток)

Клиентский запрос (подставьте реальные `f_dish` / модификаторы из `menu/get` вашего стенда):

```json
{
  "locale": "hy",
  "idempotency_key": "demo-order-2026-04-01-001",
  "dishes": [
    {
      "f_dish": 40,
      "qty": 1,
      "type": "ԼԱ ՊԻՑՑԱ",
      "size": "Մեծ",
      "modifiers": [501, 502],
      "comment": "լավ տապակած"
    },
    {
      "f_dish": 88,
      "qty": 2
    }
  ]
}
```

Ожидание: в ArariX заказ на столе буклета, на кухне распечатан service-чек; `order.table.f_id` совпадает со столом ключа.

## Статус заказа

```http
POST /ararix/order-status/by-token
Content-Type: application/json

{ "token": "<f_public_token>", "locale": "hy" }
```

Ключ буклета не нужен — доступ по энтропии токена. CORS открыт.

| Поле | Значения |
| --- | --- |
| `order.state` / `state_code` | `1` `in_progress`, `2` `closed`, `3` `cancelled`, `5` `preorder` |
| `kitchen_status` / `kitchen_status_code` | `0` `pending`, `1` `accepted`, `2` `cooking`, `3` `ready`, `4` `served` |

По позициям — те же `kitchen_status_*`. Общий kitchen status — минимум по строкам с процессом.

Опрос: каждые **3–5 секунд** (`polling_hint_sec` в ответе). **Webhook нет.**

Пример ответа:

```json
{
  "status": 1,
  "polling_hint_sec": 4,
  "webhook": false,
  "order": {
    "token": "...",
    "number": "A00012",
    "state": 1,
    "state_code": "in_progress",
    "state_name": "In progress",
    "kitchen_status": 2,
    "kitchen_status_code": "cooking",
    "table_name": "5",
    "hall_name": "Зал",
    "amount_total": 4500,
    "items": []
  }
}
```

## Коды ошибок (JSON)

Всегда:

```json
{ "error": { "code": "STOPLIST", "message": "...", "details": {} } }
```

| code | HTTP | Когда |
| --- | --- | --- |
| `UNAUTHORIZED` | 401 | нет ключа |
| `FORBIDDEN` | 403 | неверный ключ |
| `INVALID_TABLE` | 403/404 | ключ без стола / стол не найден |
| `TABLE_NOT_ALLOWED` | 400 | в create передали table |
| `SESSION_CLOSED` | 409 | нет открытой смены кассы WS |
| `EMPTY_DISHES` | 400 | пустой список |
| `DISH_NOT_FOUND` | 422 | блюда нет в меню |
| `STOPLIST` | 409 | стоп-лист |
| `TYPE_REQUIRED` / `SIZE_REQUIRED` | 422 | не выбран тип/размер пакета |
| `COMBO_UNAVAILABLE` | 422 | нет такой пары Type/Size |
| `REQUIRED_MODIFIER` | 422 | нет обязательной добавки |
| `UNKNOWN_MODIFIER` | 422 | чужой id модификатора |
| `PRICE_CHANGED` | 409 | цена устарела (create) |
| `VALIDATION_FAILED` | 400/500 | прочая валидация / конфиг |
| `NOT_FOUND` | 404 | картинка / статус токена |

## Тестовая среда

- Рабочая станция сайта: `webapi_config_id` в `cnf.php` (тип shop/webapi с `f_cashbox_id`).
- Нужна **открытая смена** этой кассы.
- Заказ уходит на кухню реально (service print) — для теста используйте стол без гостей или отменяйте заказ в Waiter вручную.
- Ключи столов выпускайте только через staff `issue-key`; не светите токены в публичных чатах.
- Миграция БД: `run_update.php` (v251) — `external_api_key.f_table_id`, `guest_order_idempotency`.

## Вне скоупа сейчас

Дозаказ на тот же стол, общий счёт, сплит, оплата с буклета — позже. Цель фазы: один QR → правильный стол → один заказ → статус.

---

## Справка киоска: модификаторы и пакеты

### Модификаторы

Из `f_data` карточки, по которой нажали. Независимые галочки. `f_required: true` обязателен в `modifiers` при create. Цена штуки += сумма `f_price` отмеченных.

### Пакет Type / Size

Выбор показывать, если различных Type (или Size) больше одного. Пара допустима только если есть строка комплектации с обоими значениями.

### Связанные напитки / добавки

`f_related_drink` / `f_related_other` — отдельные строки корзины (отдельные элементы `dishes`), не модификаторы пакета.
