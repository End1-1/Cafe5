// ignore: unused_import
import 'package:intl/intl.dart' as intl;
import 'app_localizations.dart';

// ignore_for_file: type=lint

/// The translations for Armenian (`hy`).
class AppLocalizationsHy extends AppLocalizations {
  AppLocalizationsHy([String locale = 'hy']) : super(locale);

  @override
  String get appName => 'ArariX Solutions';

  @override
  String get welcome => 'Բարի գալուստ';

  @override
  String get registerWithPhone => 'Գրանցվեք հեռախոսահամարով';

  @override
  String get prefix => 'Կոդ';

  @override
  String get phoneNumber => 'Հեռախոսահամար';

  @override
  String get sms => 'SMS';

  @override
  String get whatsapp => 'WHATSAPP';

  @override
  String get orWith => 'կամ';

  @override
  String get google => 'Google';

  @override
  String get facebook => 'Facebook';

  @override
  String get termsFooter =>
      'Շարունակելով՝ դուք ընդունում եք Պայմանները, Գաղտնիության և Cookies քաղաքականությունը';

  @override
  String get selectCountry => 'Ընտրել երկիր';

  @override
  String get searchCountry => 'Որոնել երկիր';

  @override
  String get armenia => 'Հայաստան';

  @override
  String get phoneValidation => 'Հեռախոսի հաստատում';

  @override
  String get enterTheCode => 'Մուտքագրեք կոդը';

  @override
  String insertCodeWhatsapp(String phone) {
    return 'Մուտքագրեք 4-նիշանոց կոդը, որը ուղարկել ենք WhatsApp-ով $phone';
  }

  @override
  String insertCodeSms(String phone) {
    return 'Մուտքագրեք 4-նիշանոց կոդը, որը ուղարկել ենք SMS-ով $phone';
  }

  @override
  String resendIn(String time) {
    return 'Կոդ չկա՞ Ուղարկել նորից՝ $time';
  }

  @override
  String get resendNow => 'Ուղարկել կոդը նորից';

  @override
  String get account => 'Հաշիվ';

  @override
  String get enterYourName => 'Մուտքագրեք անունը';

  @override
  String get enterYourEmail => 'Մուտքագրեք email';

  @override
  String get changePassword => 'Փոխել կամ մուտքագրել գաղտնաբառը';

  @override
  String get addAddresses => 'Ավելացնել հասցեներ';

  @override
  String get managePayments => 'Կառավարել վճարումները';

  @override
  String get profile => 'Պրոֆիլ';

  @override
  String get orderHistory => 'Պատվերների պատմություն';

  @override
  String get paymentMethods => 'Վճարման եղանակներ';

  @override
  String get language => 'Լեզու';

  @override
  String get faq => 'ՀՏՀ';

  @override
  String get notifications => 'Ծանուցումներ';

  @override
  String get deleteAccount => 'Ջնջել հաշիվը և տվյալները';

  @override
  String get silverStatus => 'Silver կարգավիճակ';

  @override
  String get changePhoneNumber => 'Փոխել հեռախոսահամարը';

  @override
  String get enterYourPhoneNumber => 'Մուտքագրեք հեռախոսահամարը';

  @override
  String get payments => 'Վճարումներ';

  @override
  String get addNewCard => 'Ավելացնել քարտ';

  @override
  String get helpAndSupport => 'Օգնություն և աջակցություն';

  @override
  String get chooseLanguage => 'Ընտրեք լեզուն';

  @override
  String get home => 'Գլխավոր';

  @override
  String get menu => 'Մենյու';

  @override
  String get orders => 'Պատվերներ';

  @override
  String get comingSoon => 'Շուտով';

  @override
  String get socialLoginSoon => 'Սոցիալական մուտքը շուտով';

  @override
  String get invalidPhone => 'Մուտքագրեք վավեր համարը';

  @override
  String get invalidOtp => 'Մուտքագրեք 4-նիշանոց կոդը';

  @override
  String get errorGeneric => 'Ինչ-որ սխալ տեղի ունեցավ';

  @override
  String get save => 'Պահպանել';

  @override
  String get name => 'Անուն';

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
  String get searchRestaurant => 'Մուտքագրեք որոնման համար';

  @override
  String get delivery => 'Առաքում';

  @override
  String get takeaway => 'Տանել';

  @override
  String get dineIn => 'Տեղում';

  @override
  String get topRestaurants => 'Թոփ ռեստորաններ';

  @override
  String get dishes => 'Ուտեստներ';

  @override
  String get topOffer => 'Օրվա առաջարկ';

  @override
  String get retry => 'Կրկնել';

  @override
  String get addons => 'Հավելումներ';

  @override
  String get fats => 'Ճարպեր';

  @override
  String get carbs => 'Ածխաջրեր';

  @override
  String get protein => 'Սպիտակուցներ';

  @override
  String get calories => 'Կալորիաներ';

  @override
  String get next => 'Հաջորդ';

  @override
  String get minShort => 'Րոպե';

  @override
  String get yourOrder => 'Ձեր պատվերը';

  @override
  String get emptyCart => 'Զամբյուղը դատարկ է';

  @override
  String get total => 'Ընդամենը';

  @override
  String get noDishesInGroup => 'Այս խմբում ուտեստներ չկան';

  @override
  String get checkoutSoon => 'Պատվերի ձևակերպումը շուտով հասանելի կլինի';

  @override
  String addNFor(int qty, String price) {
    return 'Ավելացնել $qty՝ $price';
  }

  @override
  String get personalize => 'Կարգավորել';

  @override
  String get drinkStep => 'Խմիչք';

  @override
  String get extraStep => 'Լրացուցիչ';

  @override
  String get chooseDrink => 'Ընտրեք խմիչք';

  @override
  String get addExtra => 'Ավելացնել լրացուցիչ';

  @override
  String get attrType => 'Տեսակ';

  @override
  String get attrSize => 'Չափս';

  @override
  String get comboUnavailable =>
      'Այս համադրությունը հասանելի չէ։ Փոխեք ընտրությունը։';

  @override
  String get packageNoOptions => 'Այս փաթեթը կարգավորված տարբերակներ չունի։';

  @override
  String customizePackage(String name) {
    return 'Կարգավորեք $name';
  }

  @override
  String get back => 'Հետ';
}
