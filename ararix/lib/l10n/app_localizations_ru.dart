// ignore: unused_import
import 'package:intl/intl.dart' as intl;
import 'app_localizations.dart';

// ignore_for_file: type=lint

/// The translations for Russian (`ru`).
class AppLocalizationsRu extends AppLocalizations {
  AppLocalizationsRu([String locale = 'ru']) : super(locale);

  @override
  String get appName => 'ArariX Solutions';

  @override
  String get welcome => 'Добро пожаловать';

  @override
  String get registerWithPhone => 'Зарегистрируйтесь по номеру телефона';

  @override
  String get prefix => 'Код';

  @override
  String get phoneNumber => 'Номер телефона';

  @override
  String get sms => 'SMS';

  @override
  String get whatsapp => 'WHATSAPP';

  @override
  String get orWith => 'или через';

  @override
  String get google => 'Google';

  @override
  String get facebook => 'Facebook';

  @override
  String get termsFooter =>
      'Продолжая, вы принимаете Условия использования, Политику конфиденциальности и Cookies';

  @override
  String get selectCountry => 'Выбор страны';

  @override
  String get searchCountry => 'Поиск страны';

  @override
  String get armenia => 'Армения';

  @override
  String get phoneValidation => 'Подтверждение телефона';

  @override
  String get enterTheCode => 'Введите код';

  @override
  String insertCodeWhatsapp(String phone) {
    return 'Введите 4-значный код, отправленный в WhatsApp на $phone';
  }

  @override
  String insertCodeSms(String phone) {
    return 'Введите 4-значный код, отправленный по SMS на $phone';
  }

  @override
  String resendIn(String time) {
    return 'Нет кода? Отправить снова через $time';
  }

  @override
  String get resendNow => 'Отправить код снова';

  @override
  String get account => 'Аккаунт';

  @override
  String get enterYourName => 'Введите имя';

  @override
  String get enterYourEmail => 'Введите email';

  @override
  String get changePassword => 'Изменить или задать пароль';

  @override
  String get addAddresses => 'Добавить адреса';

  @override
  String get managePayments => 'Управление оплатой';

  @override
  String get profile => 'Профиль';

  @override
  String get orderHistory => 'История заказов';

  @override
  String get paymentMethods => 'Способы оплаты';

  @override
  String get language => 'Язык';

  @override
  String get faq => 'FAQ';

  @override
  String get notifications => 'Уведомления';

  @override
  String get deleteAccount => 'Удалить аккаунт и данные';

  @override
  String get silverStatus => 'Статус Silver';

  @override
  String get changePhoneNumber => 'Смена телефона';

  @override
  String get enterYourPhoneNumber => 'Введите номер телефона';

  @override
  String get payments => 'Оплата';

  @override
  String get addNewCard => 'Добавить карту';

  @override
  String get helpAndSupport => 'Помощь и поддержка';

  @override
  String get chooseLanguage => 'Выберите язык';

  @override
  String get home => 'Главная';

  @override
  String get menu => 'Меню';

  @override
  String get orders => 'Заказы';

  @override
  String get comingSoon => 'Скоро';

  @override
  String get socialLoginSoon => 'Социальный вход будет позже';

  @override
  String get invalidPhone => 'Введите корректный номер';

  @override
  String get invalidOtp => 'Введите 4-значный код';

  @override
  String get errorGeneric => 'Что-то пошло не так';

  @override
  String get save => 'Сохранить';

  @override
  String get name => 'Имя';

  @override
  String get email => 'Email';

  @override
  String get english => 'English';

  @override
  String get russian => 'Russian';

  @override
  String get armenian => 'Armenian';

  @override
  String get englishNative => 'English';

  @override
  String get russianNative => 'Русский';

  @override
  String get armenianNative => 'Հայերեն';

  @override
  String get searchRestaurant => 'Введите для поиска';

  @override
  String get delivery => 'Доставка';

  @override
  String get takeaway => 'С собой';

  @override
  String get dineIn => 'В зале';

  @override
  String get topRestaurants => 'Топ рестораны';

  @override
  String get dishes => 'Блюда';

  @override
  String get topOffer => 'Предложение дня';

  @override
  String get retry => 'Повторить';

  @override
  String get addons => 'Добавки';

  @override
  String get fats => 'Жиры';

  @override
  String get carbs => 'Углеводы';

  @override
  String get protein => 'Белки';

  @override
  String get calories => 'Калории';

  @override
  String get next => 'Далее';

  @override
  String get minShort => 'Мин';

  @override
  String get yourOrder => 'Ваш заказ';

  @override
  String get emptyCart => 'Корзина пуста';

  @override
  String get total => 'Итого';

  @override
  String get noDishesInGroup => 'В этой группе нет блюд';

  @override
  String get checkoutSoon => 'Оформление заказа скоро будет доступно';

  @override
  String addNFor(int qty, String price) {
    return 'Добавить $qty за $price';
  }

  @override
  String get personalize => 'Настроить';

  @override
  String get drinkStep => 'Напиток';

  @override
  String get extraStep => 'Дополнительно';

  @override
  String get chooseDrink => 'Выберите напиток';

  @override
  String get addExtra => 'Добавить ещё';

  @override
  String get attrType => 'Тип';

  @override
  String get attrSize => 'Размер';

  @override
  String get comboUnavailable => 'Такая комбинация недоступна. Измените выбор.';

  @override
  String get packageNoOptions => 'У этого набора нет настроенных опций.';

  @override
  String customizePackage(String name) {
    return 'Настройте $name';
  }

  @override
  String get back => 'Назад';
}
