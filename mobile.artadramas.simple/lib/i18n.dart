class S {
  static const ru = {
    'title': 'Цех',
    'pin': 'Введите PIN',
    'settings': 'Настройки',
    'server': 'Адрес сервера',
    'art': 'Номер цеха',
    'storeOut': 'Склад выхода ингредиентов',
    'storeIn': 'Склад входа готовой продукции',
    'save': 'Сохранить',
    'next': 'Далее',
    'language': 'Язык',
    'goods': 'Полуфабрикаты',
    'drafts': 'Черновики',
    'qty': 'Количество',
    'done': 'Готово',
    'cancel': 'Отмена',
    'delete': 'Удалить',
    'deleteDraft': 'Удалить черновик?',
    'logout': 'Выход',
    'refresh': 'Обновить',
    'emptyGoods': 'Нет товаров с этим номером цеха. У товара должен быть состав комплектации.',
    'emptyDrafts': 'Черновиков нет. После сохранения бухгалтером строка пропадает.',
    'saved': 'Записано',
    'needStores': 'Выберите склад выхода ингредиентов и склад входа готовой продукции',
    'fillSettings': 'Укажите сервер и номер цеха',
    'signInForStores': 'Склады выбираются после входа по PIN',
    'emptyStores': 'Список складов пуст',
    'loadFail': 'Не удалось загрузить',
    'networkFail': 'Не удалось связаться с сервером',
  };

  static const hy = {
    'title': 'Արտադրամաս',
    'pin': 'Մուտքագրեք PIN',
    'settings': 'Կարգավորումներ',
    'server': 'Սերվերի հասցե',
    'art': 'Արտադրամասի համար',
    'storeOut': 'Բաղադրիչների ելքի պահեստ',
    'storeIn': 'Պատրաստի արտադրանքի մուտքի պահեստ',
    'save': 'Պահպանել',
    'next': 'Առաջ',
    'language': 'Լեզու',
    'goods': 'Կիսաֆաբրիկատներ',
    'drafts': 'Սևագրեր',
    'qty': 'Քանակ',
    'done': 'Պատրաստ է',
    'cancel': 'Չեղարկել',
    'delete': 'Ջնջել',
    'deleteDraft': 'Ջնջե՞լ սևագիրը',
    'logout': 'Ելք',
    'refresh': 'Թարմացնել',
    'emptyGoods': 'Այս համարով ապրանքներ չկան։ Ապրանքը պետք է կոմպլեկտացիայի բաղադրություն ունենա։',
    'emptyDrafts': 'Սևագրեր չկան։ Հաշվապահի պահպանումից հետո տողը անհետանում է։',
    'saved': 'Գրանցվեց',
    'needStores': 'Ընտրեք բաղադրիչների ելքի և պատրաստի արտադրանքի մուտքի պահեստները',
    'fillSettings': 'Նշեք սերվերը և արտադրամասի համարը',
    'signInForStores': 'Պահեստները ընտրվում են PIN մուտքից հետո',
    'emptyStores': 'Պահեստների ցանկը դատարկ է',
    'loadFail': 'Բեռնել չհաջողվեց',
    'networkFail': 'Սերվերի հետ կապ չհաջողվեց',
  };

  static String t(String lang, String key) {
    final dict = lang == 'hy' ? hy : ru;
    return dict[key] ?? ru[key] ?? key;
  }
}

double? parseQty(String raw) {
  final text = raw.trim().replaceAll(',', '.');
  if (text.isEmpty || text == '.') {
    return null;
  }
  final value = double.tryParse(text);
  if (value == null || value < 0.0001) {
    return null;
  }
  return value;
}

String formatQty(double qty) {
  final text = qty.toStringAsFixed(3);
  return text.replaceFirst(RegExp(r'\.?0+$'), '');
}
