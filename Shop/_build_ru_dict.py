# -*- coding: utf-8 -*-
"""Build Shop/ru_dict.json — professional RU translations for Picasso Shop POS."""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCES = (ROOT / "_all_sources.txt").read_text(encoding="utf-8").splitlines()
FD_DICT = Path(__file__).resolve().parent.parent / "FrontDesk" / "ru_dict.json"

# Shop-only / missing from FrontDesk — natural Armenian retail Russian
SHOP_EXTRA: dict[str, str] = {
    # Arcus / card acquiring
    "Arcus address is not configured": "Адрес Arcus не настроен",
    "Arcus port is not configured": "Порт Arcus не настроен",
    "Arcus API key is not configured": "API-ключ Arcus не настроен",
    "Invalid payment amount": "Некорректная сумма оплаты",
    "Invalid Arcus URL": "Некорректный URL Arcus",
    "Invalid Arcus response": "Некорректный ответ Arcus",
    "Card payment was not approved": "Оплата картой не одобрена",
    # Armenian UI → Russian by meaning
    "Գումարի չափ": "Сумма",
    "Նկարագրություն": "Описание",
    "Պահպանել": "Сохранить",
    "Փակել": "Закрыть",
    "Դրամարկղի ելք": "Расход из кассы",
    "Քարտի գրանցում": "Регистрация карты",
    "Հաճախորդի կոդ": "Код клиента",
    "Կուտակային տոկոս": "Накопительный процент",
    "Հաճախորդ": "Клиент",
    "Կոդ": "Код",
    "Հեռախոս": "Телефон",
    "ՀԴՄ Կ/Տ": "ФР К/Б",
    "Կանխիկ": "Наличные",
    "Անկանխիկ": "Безналичный",
    "վերադարձը": "возврат",
    "Տպել պիտակները": "Печать этикеток",
    "Կուտակված և": "Накоплено и",
    "Կանխավճար": "Предоплата",
    "Օգտագործում": "Использование",
    "Ուժի մեջ է մինջև": "Действует до",
    # Orders / receipts / returns
    "Find order": "Найти заказ",
    "Enter receipt number": "Введите номер чека",
    "Document is not exists": "Документ не существует",
    "Register": "Регистрация",
    "No price selected": "Цена не выбрана",
    "Searchi in": "Искать в",
    "Reserve": "Резерв",
    "Bank transfer": "Банковский перевод",
    "Cash in": "Внесение наличных",
    "Telcell": "Telcell",
    "Pay (Enter)": "Оплата (Enter)",
    "Back (Esc)": "Назад (Esc)",
    "Check amounts": "Проверить суммы",
    "Card payment": "Оплата картой",
    " Cancel": " Отмена",
    "Incorrect card code": "Неверный код карты",
    "Partner is not selected": "Контрагент не выбран",
    "Card value cannot be zero": "Сумма карты не может быть нулевой",
    "Card registered": "Карта зарегистрирована",
    "Contact person": "Контактное лицо",
    "Firm name": "Название фирмы",
    "Cannot add partner without data": "Нельзя добавить контрагента без данных",
    "Box": "Коробка",
    "Emarks": "Этикетки",
    "Check discount": "Проверить скидку",
    "Learn accumulate goods": "Учитывать накопительные товары",
    "Return reason": "Причина возврата",
    "An instance of application already running": "Экземпляр приложения уже запущен",
    "Store is not defined.": "Склад не задан.",
    "Sales": "Продажи",
    "Total mode": "Режим итогов",
    "Item mode": "Режим позиций",
    "Print tax Z": "Печать Z-отчёта",
    "Print tax X": "Печать X-отчёта",
    "Order details": "Детали заказа",
    "Item return": "Возврат позиции",
    "Close menu": "Закрыть меню",
    "Sale type code": "Код типа продажи",
    "№№": "№№",
    "##": "##",
    "Deliverman": "Курьер",
    "Reserved": "Зарезервировано",
    "Store Code": "Код склада",
    "Goods Code": "Код товара",
    "reservations": "резервы",
    "View all": "Показать все",
    "reservation": "резерв",
    "Total quantity": "Общее количество",
    "Select number and press enter": "Выберите номер и нажмите Enter",
    "Delivery man": "Курьер",
    "Order number": "Номер заказа",
    "Saler": "Продавец",
    "Return qty": "Кол-во возврата",
    "Print receipt A4": "Печать чека A4",
    "Fiscal return": "Фискальный возврат",
    "Print fiscal": "Печать фискального",
    "Make draft": "Сделать черновик",
    "Items return": "Возврат товаров",
    "Print receipt": "Печать чека",
    "Print partner TIN": "Печать ИНН контрагента",
    "You cannot return this item": "Нельзя вернуть эту позицию",
    "Sel": "Выб.",
    'Enter return quantity (max %1)': "Введите количество возврата (макс. %1)",
    'Mark rows and enter quantity in column "Return qty"':
        'Отметьте строки и укажите количество в колонке «Кол-во возврата»',
    "No fiscal exists for this order": "Нет фискального чека по этому заказу",
    "Not implemented": "Не реализовано",
    "Confirm to print fiscal": "Подтвердите печать фискального чека",
    "Cannot print tax twice": "Нельзя печатать фискальный чек дважды",
    "Order is not closed": "Заказ не закрыт",
    "Fiscal receipt exists. Print taxback and return this sale to editing?":
        "Есть фискальный чек. Напечатать возврат и вернуть продажу на редактирование?",
    "Return this sale to editing?": "Вернуть эту продажу на редактирование?",
    "Working window is not available": "Рабочее окно недоступно",
    "No free tables in hall %1. Create more tables in h_tables or close unused sale tabs.":
        "Нет свободных столов в зале %1. Создайте столы в h_tables или закройте неиспользуемые вкладки продаж.",
    "Fiscal machine is not configured": "Фискальный регистратор не настроен",
    "The return reason must be specified.": "Необходимо указать причину возврата.",
    "Invalid qty": "Некорректное кол-во",
    "Nothing to return": "Нечего возвращать",
    "Cashbox session is not open": "Смена кассы не открыта",
    "Store is not defined": "Склад не задан",
    "Fiscal error": "Ошибка фискализации",
    "Return completed": "Возврат выполнен",
    "Grand total": "Итого",
    "Goods code (F7)": "Код товара (F7)",
    "Delivery": "Доставка",
    "MyPay": "MyPay",
    "Print tin": "Печать ИНН",
    "Quick debt": "Быстрый долг",
    "Discount mode": "Режим скидки",
    "Discount factor": "Коэффициент скидки",
    "Invalid taxpayer tin": "Некорректный ИНН налогоплательщика",
    "Empty order": "Пустой заказ",
    "Invalid price": "Некорректная цена",
    "A partner must be selected for debt / bank transfer":
        "Для долга / банковского перевода нужно выбрать контрагента",
    "Receipt": "Чек",
    "Error in state": "Ошибка состояния",
    "Subtotal": "Подытог",
    "Prepaid amount": "Сумма предоплаты",
    "Total due": "К оплате",
    "Amount paid": "Оплачено",
    "Sample": "Образец",
    "Enter discount value": "Введите значение скидки",
    "Enter discount amount": "Введите сумму скидки",
    "Partner have a accumulate card, use?": "У контрагента есть накопительная карта, использовать?",
    "Accumulate": "Накопление",
    "Printing fiscal check": "Печать фискального чека",
    "Connection with fiscal machine lost": "Связь с фискальным регистратором потеряна",
    "Try again": "Повторить",
    "Do not print fiscal": "Не печатать фискальный",
    "No active cashbox session. Please open a new cashbox session to create an order.":
        "Нет активной смены кассы. Откройте новую смену, чтобы создать заказ.",
    "Open cashbox session": "Открыть смену кассы",
    "Cashbox is not configured for this workstation.":
        "Касса не настроена для этого рабочего места.",
    "Open new cashbox session": "Открыть новую смену кассы",
    "Working": "Работа",
    "New retail": "Новая розница",
    "New whosale": "Новый опт",
    "Write order": "Записать заказ",
    "Help": "Справка",
    "Costumer display": "Дисплей покупателя",
    "Gift card": "Подарочная карта",
    "Manual tax receipt": "Ручной фискальный чек",
    "Booking": "Бронирование",
    "Sales report": "Отчёт по продажам",
    "Database connection": "Подключение к базе",
    "WebSocket: connecting": "WebSocket: подключение",
    "newPreorder": "newPreorder",
    "Empty barcode": "Пустой штрихкод",
    "WebSocket: connected": "WebSocket: подключено",
    "WebSocket: not connected": "WebSocket: нет соединения",
    "Session": "Смена",
    "Closing session": "Закрытие смены",
    "Operations": "Операции",
    "Shift total": "Итого смены",
    "Expected cash": "Ожидаемые наличные",
    "Counted cash": "Пересчитанные наличные",
    "Cash Overage Act": "Акт излишка кассы",
    "Cash Shortage Act": "Акт недостачи кассы",
    "Expected amount": "Ожидаемая сумма",
    "Signature": "Подпись",
    "Default hall is not set for this workstation (f_default_hall_id)":
        "Для этого рабочего места не задан зал по умолчанию (f_default_hall_id)",
    "No tables found in h_tables for hall %1": "В h_tables нет столов для зала %1",
    "Goods reserved": "Товар зарезервирован",
    "The product is out of stock": "Товар отсутствует на складе",
    "Confirm to close application": "Подтвердите закрытие приложения",
    "Search goods in the storages": "Поиск товаров на складах",
    "Input staff at the work": "Отметить сотрудника на работу",
    "Output staff from the work": "Снять сотрудника с работы",
    "Total today": "Итого за сегодня",
    "Show log": "Показать журнал",
    "Open new store input document": "Открыть новый приход на склад",
    "List of workers at work": "Список сотрудников на смене",
    "Do you want to close active session?": "Закрыть активную смену?",
    "Cash in drawer": "Наличные в ящике",
    # keep-as-is technical / formats present only in Shop miss list
    "TITLE": "TITLE",
    "*******": "*******",
    "999999999": "999999999",
    "0.0": "0.0",
    "F2": "F2",
    "CD": "CD",
    "⭕": "⭕",
    "9,999,999,999.99": "9,999,999,999.99",
    "01/01/2026": "01/01/2026",
    "Ctrl+A": "Ctrl+A",
    "closed": "closed",
    "ops": "ops",
    "": "",
}

# Optional Shop-context overrides when FrontDesk wording is OK but Shop prefers glossary/typo fixes
SHOP_OVERRIDE: dict[str, str] = {
    "New whosale": "Новый опт",
    "New whosale traid": "Новая оптовая продажа",
    "Whosale": "Опт",
    "Whosale price": "Оптовая цена",
    "Whosale sale": "Оптовая продажа",
    "Whosale discounted": "Опт со скидкой",
    "Retaile discounted": "Розница со скидкой",
    "Goods special whosale price": "Спец. оптовая цена товара",
    "Diff whosale": "Разница опт",
    "Costumer debts pay": "Погашение долгов клиентов",
    "Costumer debts report": "Отчёт по долгам клиентов",
    "Costumer display": "Дисплей покупателя",
    "Enter your username and pin": "Введите имя пользователя и PIN",
    "Parnert ID": "ID контрагента",
    "Partner addres": "Адрес контрагента",
    "Spesial price": "Спец. цена",
    "Expcense account": "Счёт расходов",
    "Goods consuption, draft": "Расход товаров, черновик",
    "Reason for consuption": "Причина расхода",
    "Goods reservatoins": "Резервы товаров",
    "Menu availbility": "Доступность меню",
    "Chage password": "Сменить пароль",
    "Change the store of oupput": "Сменить склад расхода",
    "Avaiable cash": "Доступные наличные",
    "Simple  item": "Простая позиция",
    "Searchi in": "Искать в",
    "None  secure": "Без защиты",
    "Connection  type": "Тип подключения",
}


def keep_as_is(s: str) -> bool:
    if not s.strip():
        return True
    if re.fullmatch(r"f_[A-Za-z0-9_]+", s):
        return True
    if s.startswith("form_"):
        return True
    if s in {
        "uuid", "int", "double", "kb", "x", "H", "M", "S",
        "I", "II", "III", "IV", "V", "VI", "VII",
        "QR", "NN", "TLD", "STAFF", "A1", "A2", "A3",
        "C", "MR", "M+", "IN/OUT", "In", "X", "ID", "Crn", "Adgt",
        "TextLabel", "CommandLinkButton", "Picasso", "UUID",
        "AS invoice", "AS Retail", "Idram", "Telcell", "MyPay",
        "Breeze service", "<html>msg</html>",
        "000111222", "+374 99 999-999", "+374  -",
        "27.07.2019 22:16", "01/01/2024 - 01/01/2025", "01/01/2026",
        "...", "TITLE", "*******", "999999999", "0.0", "F2", "CD", "⭕",
        "9,999,999,999.99", "Ctrl+A", "closed", "ops", "newPreorder",
        "№№", "##", "MS Excel", "CAPS", "Tab",
    }:
        return True
    if re.fullmatch(r"[\d\s.,:+\-*/÷=%‹›⏎—\\]*", s) and any(
        ch.isdigit() or ch in "+-*/÷=.%‹›⏎—," for ch in s
    ):
        return True
    if s in {"+", "-", "*", "/", "=", ".", ",", "÷", "+/-", "⏎", "/"}:
        return True
    if re.fullmatch(r"%[0-9n]", s):
        return True
    if re.fullmatch(r"[a-z][a-z0-9_]*", s) and "_" in s:
        return True
    return False


def main() -> None:
    fd: dict[str, str] = {}
    if FD_DICT.exists():
        fd = json.loads(FD_DICT.read_text(encoding="utf-8"))

    out: dict[str, str] = {}
    missing: list[str] = []
    reused = 0
    extra = 0
    kept = 0
    overridden = 0

    for src in SOURCES:
        if src in out:
            continue

        if src in SHOP_OVERRIDE:
            out[src] = SHOP_OVERRIDE[src]
            overridden += 1
            continue

        if src in SHOP_EXTRA:
            out[src] = SHOP_EXTRA[src]
            extra += 1
            continue

        if src in fd:
            out[src] = fd[src]
            reused += 1
            continue

        if keep_as_is(src):
            out[src] = src
            kept += 1
            continue

        missing.append(src)
        out[src] = src

    out_path = ROOT / "ru_dict.json"
    out_path.write_text(
        json.dumps(out, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )

    uniq = len(out)
    print(
        f"keys={uniq} source_lines={len(SOURCES)} unique_lines={len(set(SOURCES))} "
        f"reused_fd={reused} shop_extra={extra} override={overridden} keep={kept} missing={len(missing)}"
    )
    if missing:
        (ROOT / "_ru_missing.txt").write_text("\n".join(missing), encoding="utf-8")
        print("wrote _ru_missing.txt")
        for m in missing:
            print("MISSING:", repr(m))


if __name__ == "__main__":
    main()
